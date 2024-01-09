#include<sys/types.h>
#include<sys/socket.h>   
#include<sys/stat.h>
#include<dirent.h>  
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>
#include<stdio.h>
#include<iostream>
#include<thread>
#include<mutex>
#include<vector>
#include<unordered_map>
#include<sstream>
#include<fstream>
#include<mysql/mysql.h>
#include <unordered_map>
#include"PRIVILEDGE.h"
#include"PAGE_CHANGE.h"
#include"QUERY.h"
#include"CODES.h"
#include"DATAS.h"
// #include"FileSystem/inode.h"
#include"inode.h"
#include"COURSE.h"
#include"USER.h"
#define SERVER_PORT 8888
#define BUFF_LEN 2048
#define IP_MAXSIZE 9999

//g++ -o A1 T.cpp QUERY.cpp PAGE_CHANGE.cpp PRIVILEDGE.cpp CODES.cpp inode.cpp txt.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
//-----------------------------------------------------------
using namespace std;
string identity_to_name[4] = {
    "",
    "student",
    "teacher",
    "administrator"
};

int FD;//
int NUM;
bool IP_STATE[IP_MAXSIZE];
int pointer = 0;
mutex LOCK;
unordered_set<string>ID_search;
MYSQL mysql;


//文件系统全局变量
SuperBlock superBlock;
struct InodeList inodeListMem[MAX_INODELIST_NUMBER];//inodelist空间,一共有124个inodelist   //212*124  //有层次结构，不需要用bitmap
struct FileBlock blockMem[BLOCK_NUMBER];//存block的空间，一共有128个block  //512*1120

string DEAL_THE_CODE(string op_code, priviledge* user_information,
                     int* if_extra_input,bool* error_detect,int* step, string extra, 
                    vector<string*>basic_information, string* target_path,User* user,string memory){
    cout<<"dealing option:"<<op_code<<endl;
    cout<<"if_extra_input:"<<*if_extra_input<<endl;
    string output = "";
    if(*if_extra_input==0){
        //**part1**
        if(extra.size()==0){
            cout<<"enter part1"<<endl;
            //deal basic options 
            //基本的选项，没有返回值，本身只执行换页操作，不需要修改，已经完成
            if(op_code=="11"||op_code=="12"||op_code=="13"||op_code=="14"||op_code=="15"||op_code=="16"||op_code=="17"||op_code=="18"){
                NEXT(user_information,op_code);
            }
            //deal code 19 291 3911 which related to 'back'
            //处理所有涉及到返回的代码，每一个页面都会包含一条返回，不需要修改，已完成
            if(op_code[op_code.size()-1] == 'b'){
                PREVIOUS(user_information);
            }
            //deal the second layer
            if(op_code=="211"){
                //查询所有可选的课程，就是管理员开设的开放选择的课程，
                //输出的时候将 output 的引用或指针传入，在里面对output进行操作，
                //格式要预先设计好因为output是不处理直接显示在用户端的
                // getAllChooseCourses(mysql,output,superBlock,inodeListMem,blockMem);
                
                string sql = "select * from course";
                cout<<sql<<endl;
                MYSQL_RES *  result = NULL;
                MYSQL_ROW row;
                basic_search(&mysql, &result, sql);
                vector<string>courses;
                while ((row = mysql_fetch_row(result))) {

                    string courseID = row[0];
                    Course tempCourse(courseID,superBlock,inodeListMem,blockMem);
                    if(tempCourse.getChoose()){
                         courses.push_back(courseID);
                    }
                    // std::cout << std::endl;
                }
                if(courses.size()==0){output+="No courses can be chosen";}
                else{output+=FORMAT("CourseID",10)+FORMAT("Name",10)+FORMAT("Credit",10)+"\n";}
                for(string courseID:courses){
                    Course course(courseID,superBlock,inodeListMem,blockMem);
                    if(course.getChoose()){
                        output+=course.getInfo()+"\n";
                    }
                }
            }
            else if(op_code=="212"){
                //Selection选课，选课操作output引导用户输入，
                //用户输入后存在extra里面，在下面的part2里面进行处理
                // getAllChooseCourses(mysql,output,superBlock,inodeListMem,blockMem);
                string sql = "select * from course";
                cout<<sql<<endl;
                MYSQL_RES *  result = NULL;
                MYSQL_ROW row;
                basic_search(&mysql, &result, sql);
                vector<string>courses;
                while ((row = mysql_fetch_row(result))) {

                    string courseID = row[0];
                    Course tempCourse(courseID,superBlock,inodeListMem,blockMem);
                    if(tempCourse.getChoose()){
                         courses.push_back(courseID);
                    }
                    // std::cout << std::endl;
                }
                if(courses.size()==0){output+="No courses can be chosen";}
                else{output+=FORMAT("CourseID",10)+FORMAT("Name",10)+FORMAT("Credit",10)+"\n";}
                for(string courseID:courses){
                    Course course(courseID,superBlock,inodeListMem,blockMem);
                    if(course.getChoose()){
                        output+=course.getInfo()+"\n";
                    }
                }
                output+="input course ID:\n";
                *if_extra_input=1;
            }
            else if(op_code=="221"){//Choose course
                //这里用户选择了一门课程，需要执行下一页操作 
                //复制此代码在结尾执行下一页操作，后面的代码同理
                //，有第三级的代码都需要下一页 NEXT(user_information,op_code);
                vector<Course>Courses;
                if(user->getIdentity()==1){
                    unordered_map<string,pair<int,int>>courseMap;
                    user->getInfo(courseMap);
                    for(auto x:courseMap){
                        Course course(x.first,superBlock,inodeListMem,blockMem);
                        Courses.push_back(course);
                    }
                }
                output+=FORMAT("Course",10)+FORMAT("Name",20)+FORMAT("Credit",10)+"\n";
                for(Course course:Courses){
                    output+=course.getInfo()+"\n";  
                }
                output+="input courseID:\n";
                *if_extra_input=1;
            }
            else if(op_code=="222"){
                //这里仅仅展示已经结课了课，输出在output里面规划      

                unordered_map<string,pair<int,int>>courseMap;
                user->initStudent(superBlock,inodeListMem,blockMem);
                user->getInfo(courseMap);
                //展示结课课程
                output+=FORMAT("Course",10)+FORMAT("Name",20)+FORMAT("Credit",10)+FORMAT("Grade",10)+"\n";

                for(auto x:courseMap){
                    Course course(x.first,superBlock,inodeListMem,blockMem);
                    if(course.getClose()){
                        output+=course.getInfo()+FORMAT(to_string(x.second.second),10)+"\n";
                    }
                }
            }
            else if(op_code=="3211"){
                //查看课程作业，之前选择的课程，查看老师的作业，返回到output
                output+= "HW_name:";
                Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
                vector<string>hwName=course.getHWName();
                for(string hw:hwName){
                    output+=hw+" ; ";
                }
                output+="\ninput HW name:\n";
                *if_extra_input=1;
            }
            else if(op_code=="3212"){
                //上交作业，需要进行而此操作，我给你定义在part2
                output+= "HW_name:";
                Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
                vector<string>hwName=course.getHWName();
                for(string hw:hwName){
                    output+=hw+" ; ";
                }
                output+="\ninput HW_name\n";
                *if_extra_input=1;
            }
            else if(op_code=="3213"){
                //查看作业反馈，这里需要读取老师的反馈，在output中规划
                output+= "HW_name: ";
                Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
                vector<string>hwName=course.getHWName();
                for(string hw:hwName){
                    output+=hw+" ; ";
                }
                output+="\ninput HW_name:\n";
                *if_extra_input=1;
            }
            else if(op_code=="231"){
                //布置作业，这里需要额外输入，在part2中，在output中引导正确的输入格式
                vector<string>courses;
                user->getCourses(courses);
                output+= "Courses:";
                for(string courseID:courses){
                    Course course(courseID,superBlock,inodeListMem,blockMem);
                    if(course.getOpen()){
                        output+=courseID+" ; ";
                    }
                    
                }
                output+="\ninput course:\n";
                *if_extra_input=1;
            }
            else if(op_code=="232"){
                //查看已交作业 这里可以使用output输出所有上交的作业的名称或者学号，并且引导用户输入ID查看作业，这里需要在part2中2处理输入的ID或名称
            
                vector<string>courses;
                user->getCourses(courses);
                output+="Course:";
                // for(string course:courses){
                //     output+=course+" ; ";
                // }
                for(string courseID:courses){
                    Course course(courseID,superBlock,inodeListMem,blockMem);
                    if(course.getOpen()){
                        output+=courseID+" ; ";
                    }
                    
                }
                output+="\ninput course:\n";
                *if_extra_input=1;
            }
            else if(op_code=="233"){
                //在output中输出所有上交作业的同学的ID以及作业的名称，
                //并且在output中引导用户输入正确的格式比如（ID rate）这种，
                //具体你们自己决定

                vector<string>courses;
                user->getCourses(courses);
                output+="Course:";
                // for(string course:courses){
                //     output+=course+" ; ";
                // }
                for(string courseID:courses){
                    Course course(courseID,superBlock,inodeListMem,blockMem);
                    if(course.getOpen()){
                        output+=courseID+" ; ";
                    }
                    
                }
                output+="\ninput course:\n";
                *if_extra_input=1;
            }
            else if(op_code=="234"){
                //总成绩
                vector<string>courses;
                user->getCourses(courses);
                output+="Course:";
                // for(string course:courses){
                //     output+=course+" ; ";
                // }
                for(string courseID:courses){
                    Course course(courseID,superBlock,inodeListMem,blockMem);
                    if(course.getOpen()){
                        output+=courseID+" ; ";
                    }
                }
                output+="\ninput course:\n";
                *if_extra_input=1;
            }
            else if(op_code=="241"){
                //用于管理员展示当前系统之中所有的用户的信息，可以很全面，在output中规划即可，上面定义的 ID_search 的容器记录的是当前在线的用户，你甚至可以展示当前用户的状态
                string sql = "select * from users;";
                MYSQL_RES *  result = NULL;
                MYSQL_ROW row;
                basic_search(&mysql, &result, sql);
                // vector<User*>users;
                vector<int>users;
                while ((row = mysql_fetch_row(result))) {
                    // Iterate through columns and print values
                    string info[4];
                    for (unsigned int i = 0; i < mysql_num_fields(result); i++) {
                        // std::cout << row[i] << "\t";
                        info[i]=row[i];
                    }
                    if(info[0]!="200000"){
                        int userID = atoi(info[0].c_str());
                        users.push_back(userID);

                        // int identity = atoi(info[3].c_str());
                        // pair<int,int>user(userID,identity);
                        // users.push_back(user);

                        // if(identity==1){
                        //     User* user=new Student(userID,superBlock,inodeListMem,blockMem);
                        // }
                        // else if(identity==2){
                        //     User* user=new Teacher(userID,superBlock,inodeListMem,blockMem);
                        // }
                        // else if(identity==3){
                        //     User* user=new Manager(userID,superBlock,inodeListMem,blockMem);
                        // }
                        // users.push_back(user);
                    }
                }
                output+= FORMAT("UserID",10)\
                        +FORMAT("Identity",10)\
                        +FORMAT("ChangeOtherPrivilege",25)+\
                        FORMAT("OtherFilePrivilege_NUM",25)+'\n';
                for(int userID :users ){
                    User user;
                    user.init(userID,superBlock,inodeListMem,blockMem);
                    output+= FORMAT(to_string(user.getUserID()),10)\
                            +FORMAT(to_string(user.getIdentity()),10)\
                            +FORMAT((user.getChangeOtherPrivilege()==1?"true":"false"),25)\
                            +FORMAT(to_string(user.getOtherPrivilegeNum()),25)+'\n';
                }
                output+="More detail:(Y/N)\n";
                *if_extra_input=1;

            }
            else if(op_code=="242"){
                //用户权限分配页面进入选项，需要执行下一页操作
                NEXT(user_information,op_code);
            }
            else if(op_code=="3421"){
                //单用户分配 在output中引导用户的输入格式，成功识别了之后只需要执行和权限命令行差不多的函数就行了，虽然有点重复但是没关系
                string sql = "select * from users;";
                MYSQL_RES *  result = NULL;
                MYSQL_ROW row;
                basic_search(&mysql, &result, sql);
                // vector<User*>users;
                vector<int>users;
                while ((row = mysql_fetch_row(result))) {
                    // Iterate through columns and print values
                    string info[4];
                    for (unsigned int i = 0; i < mysql_num_fields(result); i++) {
                        // std::cout << row[i] << "\t";
                        info[i]=row[i];
                    }
                    // int userID = atoi(info[0].c_str());
                    // int identity = atoi(info[3].c_str());
                    if(info[0]!="200000"){
                        int userID = atoi(info[0].c_str());
                        users.push_back(userID);
                        // if(identity==1){
                        //     User* user=new Student(userID,superBlock,inodeListMem,blockMem);
                        // }
                        // else if(identity==2){
                        //     User* user=new Teacher(userID,superBlock,inodeListMem,blockMem);
                        // }
                        // else if(identity==3){
                        //     User* user=new Manager(userID,superBlock,inodeListMem,blockMem);
                        // }
                        // users.push_back(user);
                    // std::cout << std::endl;
                    }
                    else{
                        continue;
                    }
                }
               output+= FORMAT("UserID",10)+FORMAT("Identity",10)+FORMAT("ChangeOtherPrivilege",25)+FORMAT("OtherFilePrivilege_NUM",25)+'\n';
                for(int userID :users ){
                    User user;
                    user.init(userID,superBlock,inodeListMem,blockMem);
                    output+= FORMAT(to_string(user.getUserID()),10)\
                            +FORMAT(to_string(user.getIdentity()),10)\
                            +FORMAT((user.getChangeOtherPrivilege()==1?"true":"false"),25)\
                            +FORMAT(to_string(user.getOtherPrivilegeNum()),25)+'\n';
                    // output+= FORMAT(to_string(user->getUserID()),10)+FORMAT(to_string(user->getIdentity()),10)+FORMAT((user->getChangeOtherPrivilege()==1?"true":"false"),20)+FORMAT(to_string(user->getOtherPrivilegeNum()),20)+'\n';
                }
                output+="input ID&identity:\n";
                *if_extra_input=1;
            }
            else if(op_code=="3422"){
                //组分配 同样的，规定一定的格式，在part2中处理
                Group group(superBlock,inodeListMem,blockMem);
                unordered_map<string,vector<pair<int,int>>>groups= group.getGroups();
                output+=FORMAT("Group",10)+FORMAT("Members",10)+"\n";
                for(pair<string,vector<pair<int,int>>>x:groups){
                    output+=x.first+" : ";
                    for(pair<int,int>y:x.second){
                        output+=to_string(y.first)+ ",";
                    }
                    output+="\n";
                }
                output+="\ninput group:\n";
                *if_extra_input=1;
            }
            else if(op_code=="251"){
                //自由分组 在output中打印用户ID和身份，
                //并且引导用户输入正确的格式，比如说第一个位置是组的名称，
                //后面的位置按照空格分割，输入用户的ID（MATH_CLUB 100000 100001）
                Group group(superBlock,inodeListMem,blockMem);
                unordered_map<string,vector<pair<int,int>>>groups= group.getGroups();
                output+=FORMAT("Group",15)+FORMAT("Members",10)+"\n";
                for(pair<string,vector<pair<int,int>>>x:groups){
                    output+=x.first+" : ";
                    for(pair<int,int>y:x.second){
                        output+=to_string(y.first)+","+to_string(y.second)+" ; ";
                    }
                    output+="\n";
                }
                output+="input group:\n";
                // output+="\ninput group&<userID&userIdentity>&<userID&userIdentity>&<...>:\n";
                *if_extra_input=1;
                
                output+=op_code+"_"+to_string(*if_extra_input)+"_"+to_string(*step)+"\n";
            }
            else if(op_code=="252"){
                //根据课程分组功能选项入口**这里是已经开放选课还没开设的课程**，
                //仅需要进入下一页，如果需要改变路径等操作，可以尝试增加这个函数的参数
                //，丰富函数功能
                // output+=FORMAT("CourseID",10)+FORMAT("Name",10)+FORMAT("Credit",10)+FORMAT("Teacher",10)+FORMAT("Students Num",20);
                // string sql = "select * from course;";
                // MYSQL_RES *  result = NULL;
                // MYSQL_ROW row;
                // basic_search(&mysql, &result, sql);
                // vector<Course>courses;
                // while ((row = mysql_fetch_row(result))) {
                //     // Iterate through columns and print values
                //     string info[3];
                //     for (unsigned int i = 0; i < mysql_num_fields(result); i++) {
                //         // std::cout << row[i] << "\t";
                //         info[i]=row[i];
                //     }
                //     Course tempCourse(info,superBlock,inodeListMem,blockMem);
                //     courses.push_back(tempCourse);
                //     // std::cout << std::endl;
                // }
                // for(Course x:courses){
                //     if(x.getChoose()){
                //         output+=x.getInfo()+FORMAT(to_string(x.getTeacher()),10)+FORMAT(to_string(x.getStudentNum()),20)+"\n";
                //     }
                // }
                // output+="input course:";
                *if_extra_input=0;
                NEXT(user_information,op_code);
            }
            else if(op_code=="3521"){
                //在output中返回所有没有开设的课程，
                //遍历课程的config或者遍历你创建的课程类对象，找带未开设的课程，
                //并且输出ID，名称，学分

                vector<string>courses;
                // getAllCourse(mysql,courses);
                courses=getAllCourse(mysql,superBlock,inodeListMem,blockMem);
                output+=FORMAT("CourseID",10)+FORMAT("Name",20)+FORMAT("Credit",10)+FORMAT("Status",10)+FORMAT("Teacher",10)+FORMAT("Students Num",20)+"\n";
                for(string courseID:courses){
                    Course course(courseID,superBlock,inodeListMem,blockMem);
                    if(course.getChoose()||((!course.getChoose())&&(!course.getOpen())&&(!course.getClose()))){
                        //
                        output+=course.getInfo();
                        if(course.getChoose()){
                            output+=FORMAT("choose",10);
                        }else{
                            output+=FORMAT("no choose",10);
                        }
                        output+=FORMAT(to_string(course.getTeacher()),10)+FORMAT(to_string(course.getStudentNum()),20)+"\n";
                    }
                }
            }
            else if(op_code=="3522"){
                //output中输出课程的ID和课程名称，引导用户输入课程ID进行开设
                 
                vector<string>courses;
                // getAllCourse(mysql,courses);
                courses=getAllCourse(mysql,superBlock,inodeListMem,blockMem);
                // output+=FORMAT("CourseID",10)+FORMAT("Name",10)+FORMAT("Credit",10);
                output+=FORMAT("CourseID",10)+FORMAT("Name",10)+FORMAT("Credit",10)+FORMAT("Status",10)+FORMAT("Teacher",10)+FORMAT("Students Num",20)+"\n";
                for(string courseID:courses){
                    Course course(courseID,superBlock,inodeListMem,blockMem);
                    if(course.getChoose()||((!course.getChoose())&&(!course.getOpen())&&(!course.getClose()))){
                        //
                        output+=course.getInfo();
                        if(course.getChoose()){
                            output+=FORMAT("choose",10);
                        }else{
                            output+=FORMAT("no choose",10);
                        }
                         output+=FORMAT(to_string(course.getTeacher()),10)+FORMAT(to_string(course.getStudentNum()),20)+"\n";
                    }
                }
                output+="input Course:\n";
                *if_extra_input=1;
            }
            else if(op_code=="261"){
                //开放选课选项，这里需要放上进入下一页的函数，另外的操作自行协商
                NEXT(user_information,op_code);
                *if_extra_input=0;
            }
            else if(op_code=="3611"){
                //在output中展示所有课程的信息，不管开没开设，什么状态，需要展示课程的ID，名称，学分，状态
                vector<string>courses;
                // getAllCourse(mysql,courses);
                courses=getAllCourse(mysql,superBlock,inodeListMem,blockMem);
                output+=FORMAT("CourseID",10)+FORMAT("Name",20)+FORMAT("Credit",10)+FORMAT("Status",10)+"\n";
                for(string courseID:courses){
                    Course course(courseID,superBlock,inodeListMem,blockMem);
                    output+=course.getInfo();
                    if(course.getChoose()){
                        output+=FORMAT("Choose",10);
                    }
                    else if(course.getOpen()){
                         output+=FORMAT("Open",10);
                    }
                    else if(course.getClose()){
                         output+=FORMAT("Close",10);
                    }
                    else{
                         output+=FORMAT("No choose",10);
                    }
                    output+="\n";
                }
                output+="\n";
                // output+="input Course:";
                // *if_extra_input=1;
            }
            else if(op_code=="3612"){
                //根据ID开放课程，output引导用户进行输入正确的课程ID
                vector<string>courses;
                // getAllCourse(mysql,courses);
                courses=getAllCourse(mysql,superBlock,inodeListMem,blockMem);
                output+=FORMAT("CourseID",10)+FORMAT("Name",20)+FORMAT("Credit",10)+FORMAT("Status",10)+"\n";
                for(string courseID:courses){
                    Course course(courseID,superBlock,inodeListMem,blockMem);
                    output+=course.getInfo();
                    if(course.getChoose()){
                        output+=FORMAT("Choose",10);
                    }
                    else if(course.getOpen()){
                         output+=FORMAT("Open",10);
                    }
                    else if(course.getClose()){
                         output+=FORMAT("Close",10);
                    }
                    else{
                         output+=FORMAT("No choose",10);
                    }
                    output+="\n";
                }
                output+="input Course&teacherID:\n";
                *if_extra_input=1;
            }
            else if(op_code=="262"){
                //结课页面，进入下一页，需要下一页函数
                NEXT(user_information,op_code);
                *if_extra_input=0;
            }
            else if(op_code=="3621"){
                //输出正在进行的课程，需要展示课程的ID，名称，学分，状态
                vector<string>courses;
                // getAllCourse(mysql,courses);
                courses=getAllCourse(mysql,superBlock,inodeListMem,blockMem);
                output+=FORMAT("CourseID",10)+FORMAT("Name",20)+FORMAT("Credit",10)+FORMAT("Status",10)+"\n";
                for(string courseID:courses){
                        Course course(courseID,superBlock,inodeListMem,blockMem);
                        output+=course.getInfo();
                    if(course.getChoose()){
                        output+=FORMAT("Choose",10);
                    }
                    else if(course.getOpen()){
                         output+=FORMAT("Open",10);
                    }
                    else if(course.getClose()){
                         output+=FORMAT("Close",10);
                    }
                    else{
                         output+=FORMAT("No choose",10);
                    }
                    output+="\n";
                }

            }
            else if(op_code=="3622"){
                //根据课程ID结课，在output中引导用户输入课程ID

                vector<string>courses;
                // getAllCourse(mysql,courses);
                courses=getAllCourse(mysql,superBlock,inodeListMem,blockMem);
                output+=FORMAT("CourseID",10)+FORMAT("Name",20)+FORMAT("Credit",10)+FORMAT("Status",10)+"\n";
                for(string courseID:courses){
                     Course course(courseID,superBlock,inodeListMem,blockMem);
                     output+=course.getInfo();
                    if(course.getChoose()){
                        output+=FORMAT("Choose",10);
                    }
                    else if(course.getOpen()){
                         output+=FORMAT("Open",10);
                    }
                    else if(course.getClose()){
                         output+=FORMAT("Close",10);
                    }
                    else{
                         output+=FORMAT("No choose",10);
                    }
                    output+="\n";
                }
                output+="input Course:\n";
                *if_extra_input=1;
            }
            else if(op_code=="271"){
                //执行文件快照，执行那个start什么什么的函数即可
                start_copy(superBlock,inodeListMem,blockMem);
                *if_extra_input=0;
            }
            else if(op_code=="272"){
                //根据快照，恢复文件，这里需要进行二次确认，
                //在这里，output输出当前的log文件中的内容，
                //并且引导用户输入确认的格式比如说（Y/N）
                char path[100];
                string path_str="/COPY/log";
                string_to_char100(path_str,path);
                string content;
                readFile(path,content,superBlock,inodeListMem,blockMem);
                output+=content+"\n";
                output+="\nRecover(Y/N):";
                *if_extra_input=1;
            }
            else if(op_code=="281"){
                //查看组信息，查看自己在哪个组之中，在output中进行编辑
                Group group(superBlock,inodeListMem,blockMem);
                unordered_map<string,vector<pair<int,int>>>groups=group.getGroups();

                // for(pair<string,vector<pair<int,int>>>x:groups){
                //     output+=x.first+" : ";
                //     for(pair<int,int>y:x.second){
                //         output+=to_string(y.first)+" , "+to_string(y.second)+" ; ";
                //     }
                //     output+="\n";
                // }
                int userID =user->getUserID();
                // output+=
                output+=to_string(userID)+" Group:";
                // unordered_map<string,vector<pair<int,int>>> groups=group.getGroups();
                for(pair<string,vector<pair<int,int>>> x:groups){
                    for(pair<int,int> y:x.second){
                        if(y.first==userID){
                            output+=x.first+" ; ";
                            break;
                        }
                    }
                }
                output+="\n";
                *if_extra_input=0;
            }
            else if(op_code=="282"){
                query_basic_information(basic_information,output);
            }
            else if(op_code=="283"){
                *if_extra_input = 1;
                output+="Please input your old password and your new password(2 inputs required, don't input redundant spaces), splitting by blank space\n";
            }

        }
        else{
            //deal system code here
            cout<<"system command"<<endl;
            if(extra=="help"){
                output+=output_for_help;
            }
            else{
                bool flag_bit = true;
                pair<vector<int>,vector<string>> get_result = deal_command(flag_bit,extra,*user,superBlock,inodeListMem,blockMem,output,*if_extra_input,"");
                cout<<"command type:"<<get_result.first[0]<<endl;
                cout<<"director:"<<get_result.first[1]<<endl;
                cout<<"secondary command type:"<<get_result.first[2]<<endl;
                cout<<"path:"<<get_result.second[0]<<endl;
                cout<<"user_id:"<<get_result.second[1]<<endl;
                if(!flag_bit){
                    output+="Inputs that are not recognized!\n";
                }
                else{
                    //deal the code here
                }
            }
        }
    }
    else{
        //**part2**
        cout<<"enter part2"<<endl;
        if(op_code=="212"){
            //在这里处理用户选课
            string courseID(extra);
            Course course(courseID,superBlock,inodeListMem,blockMem);
            int res=0;
            // //debug 
            // unordered_map<string,pair<int,int>>map;
            // user->getInfo(map);
            // output+="User course:";
            // for(auto course:map){
            //     output+=course.first+" ; ";
            // }
            // output+='\nafter appending course\n';
            // user->appendCourse(courseID,course.getCredit(),res,superBlock,inodeListMem,blockMem);
            // output+= res==1?"Successfully append Course\n":"Already have this course\n";
            // user->getInfo(map);
            // output+="User course:";
            // for(auto course:map){
            //     output+=course.first+" ; ";
            // }
            // output+='\n';
            // //course
            // output+=courseID+" Students:";
            // vector<int>students=course.getStudents();
            // for(int x:students){
            //     output+=to_string(x)+" ; ";
            // }
            // output+='\nafter appending\n';
            // output+=course.AddStudent(user->getUserID(),superBlock,inodeListMem,blockMem);
            // students=course.getStudents();
            // for(int x:students){
            //     output+=to_string(x)+" ; ";
            // }
            // output+='\n';
            // //debug
            if(course.getChoose()){
                user->appendCourse(courseID,course.getCredit(),res,superBlock,inodeListMem,blockMem);
                output+= res==1?"Successfully append Course":"Already have this course";
                output+="\t";
                output+=course.AddStudent(user->getUserID(),superBlock,inodeListMem,blockMem);
                output+="\n";
            }
            else{
                output+=courseID+" can't be chosen\n";
            }
            *if_extra_input=0;
        }
        else if(op_code=="221"){//Choose course
                //这里用户选择了一门课程，需要执行下一页操作 
                //复制此代码在结尾执行下一页操作，后面的代码同理
                //，有第三级的代码都需要下一页 NEXT(user_information,op_code);
                string courseID(extra);
                user->setCurCourse(courseID);
                output+="choosing courseID\n";
                NEXT(user_information,op_code);
                *if_extra_input=0;
        }
        else if(op_code=="3211"){
            //show hw content
            string hwName(extra);
            Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
            output="Homework:\n"+course.getHW(hwName,superBlock,inodeListMem,blockMem)+"\n";
            *if_extra_input= 0;
        }
        else if(op_code=="3212"){
            //上交作业，需要创建作业文件，文件名你们自己协调好就行。
            if(*step==1){
                string hwname(extra);
                user->setCurHWname(hwname);
                Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
                output+="HW: "+course.getHW(hwname,superBlock,inodeListMem,blockMem);
                output+="\ninput your hw content:\n";
                *if_extra_input=1;
            }
            else if(*step==2){
                string hw(extra);
                Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
                course.handInHW(user->getCurHWname(),hw,user->getUserID(),superBlock,inodeListMem,blockMem);
                *if_extra_input=0;
            }
        }
        else if(op_code=="3213"){
            //查看作业反馈，这里需要读取老师的反馈，在output中规划
            Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
            string hwName(extra);
            output+=user->getCurCourse()+" "+hwName+" feedback:\n";
            output+=course.getFeedback(user->getUserID(),hwName,superBlock,inodeListMem,blockMem);
            output+="\n---end\n";
            *if_extra_input=0;
            }
        else if(op_code=="231"){
            //这里是布置作业，需要处理extra，包含作业的名称和作业的作业的描述
            output+=op_code+"_"+to_string(*if_extra_input)+"_"+to_string(*step)+"\n";
            if(*step==1){
                //输入课程名
                string courseID(extra);
                user->setCurCourse(courseID);
                Course course(courseID,superBlock,inodeListMem,blockMem);
                vector<string>hwName= course.getHWName();
                output+="HW name:";
                for(int i=0;i<hwName.size();i++){
                    output+=hwName[i];
                }
                output+="\n";
                output+="\ninput hw name:\n";
                *if_extra_input=1;
            }
            else if(*step==2){
                //输入作业名
                string hwName(extra);
                Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
                user->setCurHWname(hwName);
                // output+=user->getCurCourse()+"_"+user->getCurHWname()+"\n";
                if(course.HWisUnique(hwName)){
                    output+=user->getCurCourse()+"_"+user->getCurHWname()+"\ninput content:\n";
                    *if_extra_input=1;
                }
                else{
                    output+="hw name isn't unique\nplease input hw name again:";
                    *if_extra_input=1;
                    *step--;
                }
            }
            else if(*step==3){
                //输入作业内容
                string hw(extra);
                Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
                course.assignHW(user->getCurHWname(),hw,superBlock,inodeListMem,blockMem);
                *if_extra_input=0;
            }
            else{
                *if_extra_input=0;
            }
        }
        else if(op_code=="232"){
            //处理返回的用户ID或作业名称
            if(*step==1){
                //
                string courseID(extra);
                user->setCurCourse(courseID);
                Course course(courseID,superBlock,inodeListMem,blockMem);
                vector<string>HWname=course.getHWName();
                output+="HW:";
                for(string x:HWname){
                    output+=x+" ; ";
                }
                output+="\n input HW name:\n";
                *if_extra_input=1;
            }
            else if(*step==2){
                string hwName(extra);
                user->setCurHWname(hwName);
                Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
                unordered_map<string,vector<int>> map=course.getHW_S();
                output+="HW_S size:"+to_string(map.size())+"\n";
                output+="students:";
                for(int x:map[hwName]){
                    output+=to_string(x)+" ; ";
                }
                output+="\n input Student ID:\n";
                *if_extra_input=1;
            }
            else if(*step==3){
                int studnetID=atoi(extra.c_str());
                Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
                output+="HW context:";
                output+=course.getStudent_HW(studnetID,user->getCurHWname(),superBlock,inodeListMem,blockMem)+"\n";
                *if_extra_input=0;
            }
        }
        else if(op_code=="233"){
            //处理用户返回的分数和ID
             if(*step==1){
                string courseID(extra);
                user->setCurCourse(courseID);
                Course course(courseID,superBlock,inodeListMem,blockMem);
                vector<string>HWname=course.getHWName();
                output+="HW:";
                for(string x:HWname){
                    output+=x+" ; ";
                }
                output+="\n input HW name:";
                *if_extra_input=1;
            }
            else if(*step==2){
                string hwName(extra);
                user->setCurHWname(hwName);
                Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
                unordered_map<string,vector<int>> map=course.getHW_S();
                output+="students:";
                for(int x:map[hwName]){
                    output+=to_string(x)+" ; ";
                }
                output+="\n input Student ID&Score&comment:";
                *if_extra_input=1;
            }
            else if(*step==3){
                vector<string>token=spiltInput(extra,"&");
                if(token.size()<3){
                    
                    output+="Error Input!!\n";
                }
                else{
                    int studnetID=atoi(token[0].c_str());
                    int score =atoi(token[1].c_str());
                    string comment = token[2];
                    Course course(user->getCurCourse(),superBlock,inodeListMem,blockMem);
                    course.setFeedback(studnetID,user->getCurHWname(),score,comment,superBlock,inodeListMem,blockMem);
                }
                
                *if_extra_input=0;
            }
        }
        else if(op_code=="234"){
            //总成绩
            if(*step==1){
                string courseID(extra);
                user->setCurCourse(courseID);
                Course course(courseID,superBlock,inodeListMem,blockMem);
                vector<int>students=course.getStudents();
                output+="Students:";
                for(int x:students){
                    output+=to_string(x)+" ; ";
                }
                output+="\n input studentID&score:";
                *if_extra_input=1;
            }
            else if(*step==2){
                vector<string>token=spiltInput(extra,"&");
                if(token.size()<2){
                    output+="Error Input!\n";
                }
                else{
                    int studentID=atoi(token[0].c_str());
                    int score =atoi(token[1].c_str());
                    Student s (studentID,superBlock,inodeListMem,blockMem);
                    int res=0;
                    s.setGrade(user->getCurCourse(),score,res,superBlock,inodeListMem,blockMem);
                    output+=res==1?"Set grade successfully.":"Student id is wrong!";
                    output+="\n";
                }
                
                *if_extra_input=0;
            }
        }
        else if(op_code=="241"){
            if(*step==1){
                if(extra=="Y")
                {
                    output+="input userid&userIdentity:\n";
                    *if_extra_input=1;
                }
                else{
                    *if_extra_input=0;
                }
            }
            else if(*step==2){
                vector<string>token=spiltInput(extra,"&");
                if(token.size()==2){
                     int userid = atoi(token[0].c_str());
                    int identity = atoi(token[1].c_str());
                    User* user=createUser(userid,identity,superBlock,inodeListMem,blockMem);
                    unordered_map<string,char>permission=user->getOtherFilePrivilege();
                    output+=FORMAT("Path",25)+FORMAT("Permission",10)+"\n";
                    for(pair<string,char> x:permission){
                        string pathPermission=Char_to_Eight_Bit(x.second);
                        output+=FORMAT(x.first,25)+FORMAT(pathPermission,10)+"\n";
                    }
                }
                else{
                    output+="Error input!\n";
                }
                *if_extra_input=0;
            }
        }
        else if(op_code=="3421"){
            //输入单用户权限分配
            if(*step==1){
                vector<string>token=spiltInput(extra,"&");
                if(token.size()==2){
                    int id = atoi(token[0].c_str());
                    int identity=atoi(token[1].c_str());
                    user->setChangeUserID(id);
                    user->setChangeUserIdentity(identity);
                    output+="input '<path>&8-bit privilege' or ChangeP&true/false:\n";
                    *if_extra_input=1;
                }else{
                    output+="Error input!!\n";
                    *if_extra_input=0;
                }
                
            }
            else if(*step==2){
                int id=0;
                int identity=0;
                user->getChangeUserID(id);
                user->getChangeUserIdentity(identity);
                // User* changeUser = createUser(id,identity,superBlock,inodeListMem,blockMem);
                User changeUser;
                changeUser.init(id,superBlock,inodeListMem,blockMem);
                vector<string>token=spiltInput(extra,"&");
                for(int i=0;i<token.size();i++){
                    cout<<token[i]<<"\t";
                }
                cout<<endl;
                if(token.size()==2){
                    if(token[0]=="ChangP"){
                        bool on = token[1]=="true"?true:false;
                        changeUser.setChangeOtherPrivilege(on,superBlock,inodeListMem,blockMem);
                        // changeUser->setChangeOtherPrivilege(on,superBlock,inodeListMem,blockMem);
                    }
                    else{
                        if(changeUser.getFilePrivilege(token[0])!=""){
                            changeUser.changeFilePrivilege(token[0],token[1],superBlock,inodeListMem,blockMem);
                        }else{
                            changeUser.appendFilePrivilege(token[0],token[1],superBlock,inodeListMem,blockMem);
                        }
                        // if(changeUser->getFilePrivilege(token[0])!=""){
                        //     changeUser->changeFilePrivilege(token[0],token[1],superBlock,inodeListMem,blockMem);
                        // }else{
                        //     changeUser->appendFilePrivilege(token[0],token[1],superBlock,inodeListMem,blockMem);
                        // }
                    }
                }
                else{
                    output+="Error input!!\n";
                }
                *if_extra_input=0;
            }

        }
        else if(op_code=="3422"){
            //输入多用户权限分配 这里需要对于group文件进行检查和操作
            //首先检查是否有这个group，如果有这个group，就读取group下的用户ID表，按照顺序，执行单用户权限分配操作
            Group group(superBlock,inodeListMem,blockMem);
            if(*step==1){
                bool in=group.getGroups(extra);
                if(in){
                    user->setCurGroup(extra);
                    output+="input '<path>&8-bit privilege' or ChangeP&true/false:\n";
                    *if_extra_input=1;
                }
                else{
                    output+="No such group!";
                    *if_extra_input=0;
                }
            }
            else if(*step==2){
                vector<pair<int,int>>id_identity=group.getGroups()[user->getCurGroup()];
                vector<string>token=spiltInput(extra,"&");
                if(token[0]=="ChangP"){
                    bool on = token[1]=="true"?true:false;
                    for(pair<int,int>userID_Identity:id_identity){
                        int userID = userID_Identity.first;
                        User user;
                        user.init(userID,superBlock,inodeListMem,blockMem);
                        user.setChangeOtherPrivilege(on,superBlock,inodeListMem,blockMem);
                    }
                }
                else{
                    for(pair<int,int>userID_Identity:id_identity){
                        int userID = userID_Identity.first;
                        User user;
                        user.init(userID,superBlock,inodeListMem,blockMem);
                         if(user.getFilePrivilege(token[0])!=""){
                            user.changeFilePrivilege(token[0],token[1],superBlock,inodeListMem,blockMem);
                        }else{
                            user.appendFilePrivilege(token[0],token[1],superBlock,inodeListMem,blockMem);
                        }
                    }
                }
                *if_extra_input=0;
            }
        }
        else if(op_code=="251"){
            //处理自由分组，在这里需要对于group文件进行操作
            Group group(superBlock,inodeListMem,blockMem);
            output+=op_code+"_"+to_string(*if_extra_input)+"_"+to_string(*step)+"\n";
            // vector<string>token=spiltInput(extra,"&");
            // string groupName = token[0];
            // if(!group.getGroups(groupName)){
            //     output+=group.createGroup(groupName,superBlock,inodeListMem,blockMem);
            //     output+=group.getGroups(groupName)?"Successfully":"Fail to";
            //     output+=" create "+groupName+"\n";
            // }
            
            // for(int i=1;i<token.size();){
            //         int id = atoi(token[i].c_str());
            //         int identity = atoi(token[i+1].c_str());
            //         output+=group.addMember(id,identity,groupName,superBlock,inodeListMem,blockMem);
            //         i+=2;
            // }
            // *if_extra_input=0;
            if(*step==1){
                // vector<pair<int,int>>users;
                if(group.getGroups(extra)){
                    //append or remove
                    unordered_map<string,vector<pair<int,int>>>groups = group.getGroups();
                    output+="Members in Group "+extra+"\n";
                    output+=FORMAT("userID",10)+FORMAT("Identity",10)+"\n";
                    for(pair<int,int> x : groups[extra]){
                        output+=FORMAT(to_string(x.first),10)+FORMAT(to_string(x.second),10)+"\n";
                    } 
                }   
                else{
                    //create new group
                    output+=group.createGroup(extra,superBlock,inodeListMem,blockMem);
                    vector<pair<int,int>>users;
                     output+="All Members \n";
                    // getAllUser(mysql,users);
                    users=getAllUser(mysql);
                    output+=FORMAT("userID",10)+FORMAT("Identity",10)+"\n";
                    for(pair<int,int> x : users){
                        output+=FORMAT(to_string(x.first),10)+FORMAT(to_string(x.second),10)+"\n";
                    } 
                }
                user->setCurGroup(extra);
                output+="input <userID&userIdentity>(&<userID&userIdentity>):";
            }
            else if(*step==2){
                vector<string>token=spiltInput(extra,"&");
                for(int i=0;i<token.size();){
                    int id = atoi(token[i].c_str());
                    int identity = atoi(token[i+1].c_str());
                    output+=group.addMember(id,identity,user->getCurGroup(),superBlock,inodeListMem,blockMem);
                    i+=2;
                }
                *if_extra_input=0;
            }
            else{
                *if_extra_input=0;
            }
        }
        else if(op_code=="252"){
            // //处理课程分组，在这里需要对于group文件进行操作
            // string courseID(extra);
            // Group group(superBlock,inodeListMem,blockMem);
            // group.createGroup(courseID,superBlock,inodeListMem,blockMem);
            // Course course(courseID,superBlock,inodeListMem,blockMem);
            // vector<int>students=course.getStudents();
            // group.addMember(course.getTeacher(),2,courseID,superBlock,inodeListMem,blockMem);
            // for(int id:students){
            //     group.addMember(id,1,courseID,superBlock,inodeListMem,blockMem);
            // }
            // // group.addGroup(course,superBlock,inodeListMem,blockMem);
            // *if_extra_input=0;
        }
        else if(op_code=="3522"){
            //处理用户返回的课程ID开设课程
            string courseID(extra);
            Course course(courseID,superBlock,inodeListMem,blockMem);
            if(course.getChoose()){
                //
                course.openCourse(superBlock,inodeListMem,blockMem);
                output+="Success open "+courseID+"\n";
                //处理课程分组，在这里需要对于group文件进行操作
                Group group(superBlock,inodeListMem,blockMem);
                group.createGroup(courseID,superBlock,inodeListMem,blockMem);
                output+=group.getGroups(extra)?"Successfully":"Fail to";
                output+=" create group "+extra+"\n";
                vector<int>students=course.getStudents();
                group.addMember(course.getTeacher(),2,courseID,superBlock,inodeListMem,blockMem);
                
                for(int id:students){
                    group.addMember(id,1,courseID,superBlock,inodeListMem,blockMem);
                }
                output+="Members:";
                 unordered_map<string,vector<pair<int,int>>>groups=group.getGroups();
                 for(pair<int,int>x:groups[extra]){
                    output+=to_string(x.first)+" , "+to_string(x.second)+" ; ";
                 }
                output+='\n';
                *if_extra_input=0;
            }
            else{
                output+=courseID+" have already opened or no chosen.\n";
            }
            // course.openCourse(superBlock,inodeListMem,blockMem);
            *if_extra_input=0;
        }
        else if(op_code=="3612"){
            //处理用户输入的课程ID开放课程
            vector<string>token = spiltInput(extra,"&");
            if(token.size()!=2){
                output+="Error input!!!\nPlease input Course&teacherID(CS201&700313):";
            }
            else{
                string courseID(token[0]);
                int teacherID = atoi(token[1].c_str());
                Course course(courseID,superBlock,inodeListMem,blockMem);
                course.chooseCourse(superBlock,inodeListMem,blockMem);
                output+= ((course.getChoose())?"Successfully":"Fail to");
                output+=" choose course\n";
                course.setTeacher(teacherID,superBlock,inodeListMem,blockMem);
                output+= ((course.getTeacher()==teacherID)?"Successfully":"Fail to");
                output+= " append teacher "+to_string(teacherID)+"\n";
                Teacher teacher(teacherID,superBlock,inodeListMem,blockMem);
                int res =0;
                teacher.appendCourse(courseID,res,superBlock,inodeListMem,blockMem);
                output+=to_string(teacherID)+(res==1?" successfully ":" fail to ")+"append course.\n";
            }
            *if_extra_input=0;
        }
        else if(op_code=="3622"){
            //处理用户输入的课程ID结束课程
            //这里需要将结束的课程进行检查，是否所有学生都拿到了分数。
            //如果成功结束课程，需要将学生的分数同步的写入的User中，以及学生的文件中。
            Course course(extra,superBlock,inodeListMem,blockMem);
            vector<int> students= course.getStudents();
            bool close=true;
            for(int id:students){
                Student s (id,superBlock,inodeListMem,blockMem);
                int grade=0;
                s.getGrade(extra,grade);
                if(grade ==-1){
                    close=false;
                    output+=to_string(id)+" dont have final grade.Please set his/her final grade before close "+extra+"\n";
                    break;
                }
            }
            if(close){
                course.closeCourse(superBlock,inodeListMem,blockMem);
                output+=extra+" close successfully.\n";
            }
            *if_extra_input=0;
        }
        else if(op_code=="272"){
            //处理用户返回的操作，看看是否执行快照操作
            if(extra=="Y"){
                //start
                recover_From_Log(superBlock,inodeListMem,blockMem,&mysql);
                output+="recover.\n";
            }
            (*if_extra_input)=0;
         }
        else if(op_code=="283"){
            //输入密码后的操作

            //解析数据，我这里是按照空格分隔    旧密码 新密码    这种格式
            stringstream ss(extra);
            vector<string>result;
            string str;
            while(getline(ss, str, ' ')){
                result.push_back(str);
            }

            //格式判断，输入的总对象数不是2就返回
            if(result.size()!=2){
                //如果想用户重新输入也可以把这个保持1，并且在output中进行规划
                *if_extra_input = 0;
                //规划
                output+="Input error! Operation fails.\n";
            }
            else{
                //这里即数据合法
                if(*basic_information[4]!=result[0]){
                    //密码错误，返回，进行引导
                    *if_extra_input = 0;
                    output+="Password is wrong!\n";
                }
                else{
                    //密码正确执行操作
                    *if_extra_input = 0;
                    output+="Operation success\n";
                    writeLog_for_DB(8,atoi((*basic_information[1]).c_str()),*basic_information[4], superBlock, inodeListMem, blockMem);
                    *basic_information[4] = result[1];
                    string sql = "update users set Password = '"+result[1]+"' where ID = '"+*basic_information[1]+"'";
                    mysql_query(&mysql,sql.c_str());
                }
            }
        }
        else{
            bool flag_bit = true;
            pair<vector<int>,vector<string>> get_result = deal_command(flag_bit,memory,*user,superBlock,inodeListMem,blockMem,output,*if_extra_input,extra);
            if(!flag_bit){
                output+="Inputs that are not recognized!\n";
            }
            else{
                output+="operation success\n";
                //deal the code here
            }
        }
    }
    // output+="step:"+to_string(*step)+"\n";
    if(*if_extra_input==1){
        (*step)++;
        // output+="step++:"+to_string(*step)+"\n";
    }
    else{
        *step = 0;
        // output+="step reset to 0:"+to_string(*step)+"\n";
    }
    // delete user;
    return output;
}


void Mul_handle(string user_id, struct sockaddr* dst){
    priviledge user_P;
    User* user;//user
    string USER_ID,USER_IDENTITY,USER_NAME,USER_PASSWORD;
    string target_path = "./";
    struct sockaddr_in clent_addr;
    socklen_t len = sizeof(clent_addr);
    LOCK.lock();
    NUM++;
    cout<<"Current users:"<<NUM<<endl;
    LOCK.unlock();  

    cout<<user_id<<": start receiving!"<<endl;
    WAIT1://wait for login options
    target_path = "./";
    char buf[BUFF_LEN];
    memset(buf,0,BUFF_LEN);
    recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
    cout<<"receive package from"<<user_id<<":"<<buf<<endl;
    string str_temp(buf);
    if(!isneed(LOGIN_PERSON_CODE,TERMINAL_ID,str_temp)){
        cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
        sleep(1);
        goto WAIT1;
    }
    string str_user_id = get_region(str_temp);
    str_temp = decode(str_temp);
    cout<<user_id<<":"<<buf<<endl;


    //quit option
    if(str_temp[0]=='5'){
        cout<<"User login choice:"<<decode(buf)<<", QUIT!"<<endl;

        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s",LOGOUT_CONFIRMATION,user_id.c_str(),TERMINAL_ID);
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);

        LOCK.lock();
        NUM--;
        IP_STATE[atoi(str_user_id.c_str())-10001] = false;
        cout<<"Current users:"<<NUM<<endl;
        LOCK.unlock();
        return;
    }
    //login code:S,T,M
    else if(str_temp[0]=='1'||str_temp[0]=='2'||str_temp[0]=='3'){
        //feedback
        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s",LOGIN_PERMIT,user_id.c_str(),TERMINAL_ID);
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
        WAIT4://wait for user password and id
        // check id and password
        memset(buf,0,BUFF_LEN);
        recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        cout<<"receive package from"<<user_id<<":"<<buf<<endl;
        string str_temp2(buf);
        if(!isneed(LOGIN_PERSON_CODE2,TERMINAL_ID,str_temp2)){
            cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
            goto WAIT4;
        }
        //split id and password 
        str_temp2 = decode(str_temp2);//id and password
        stringstream ss(str_temp2);
        string temp_user_information[2];
        int pos = 0;
        string temp;
        while(getline(ss, temp, ',')){
            temp_user_information[pos] = temp;
            pos++;
        }
        //query
        cout<<"start query:"<<endl;
        string sql = "select * from users where ID = '"+temp_user_information[0]+"'";
        cout<<sql<<endl;
        mysql_query(&mysql,sql.c_str());
        MYSQL_RES * result = mysql_store_result(&mysql);
        MYSQL_ROW row;
        row = mysql_fetch_row(result);
        cout<<"row:"<<(row==nullptr?"null":"err") <<endl;
        if(ID_search.find(temp_user_information[0])!=ID_search.end()){
            mysql_free_result(result);
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_FAIL,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
            goto WAIT1;
        }

        if(row==nullptr){
            mysql_free_result(result);
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_FAIL,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
            goto WAIT1;
        }

        cout<<"!!@@#"<<endl;
        if(strcmp(row[3],&str_temp[0])==0&&strcmp(row[2],temp_user_information[1].c_str())==0){
            // cout<<"!!!"<<row[3]<<endl;
            ID_search.emplace(row[0]);
            USER_ID = row[0];
            USER_NAME = row[1];
            USER_PASSWORD = row[2];
            USER_IDENTITY = row[3];
            mysql_free_result(result);
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s%s",LOGIN_FEEDBACK_CODE_SUCCESS,user_id.c_str(),TERMINAL_ID,row[1]);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
        }
        else{
            cout<<"hello"<<endl;
            mysql_free_result(result);
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_FAIL,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
            goto WAIT1;
        }

        //login successfully
        user_P.set(row[3]);
        //User类完成之后在这里初始化
        //*************************************************************************************
        //string USER_ID,USER_IDENTITY,USER_NAME,USER_PASSWORD;
        //这是前面定义的变量，如果登录成功之后这些变量都会被赋值，直接使用就可以了
        int identity = atoi(USER_IDENTITY.c_str());
        int id = atoi(USER_ID.c_str());
        cout<<"create user\n";
        if(identity==1){
            //S
            user = new Student(id,superBlock,inodeListMem,blockMem);
        }
        else if(identity==2){
            //T
            user = new Teacher(id,superBlock,inodeListMem,blockMem);
        }
        else if(identity==3){
            //M
            user = new Manager(id,superBlock,inodeListMem,blockMem);
        }
        //************************************************************************************
        cout<<"initialization finished"<<endl;
    }
    else if(str_temp[0]=='4'){
        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s",LOGIN_PERMIT,user_id.c_str(),TERMINAL_ID);
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);

        WAIT2://wait for root password
        memset(buf,0,BUFF_LEN);
        recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        cout<<"receive package from"<<user_id<<":"<<buf<<endl;
        string str_temp(buf);
        if(!isneed(LOGIN_PERSON_CODE2,TERMINAL_ID,str_temp)){
            cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
            goto WAIT2;
        }

        cout<<"start query:"<<endl;

        str_temp = decode(str_temp);//get password

        string sql = "select * from users where ID = '200000'";
        mysql_query(&mysql,sql.c_str());
        MYSQL_RES * result = mysql_store_result(&mysql);
        MYSQL_ROW row;
        row = mysql_fetch_row(result);

        if(strcmp(row[2],str_temp.c_str())==0){
            mysql_free_result(result);
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_ROOT_PASSWORD_RIGHT,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);

            WAIT3:
            memset(buf,0,BUFF_LEN);
            recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
            if(!isneed(ROOT_OP,TERMINAL_ID,buf)){
                cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                sleep(1);
                goto WAIT3;
            }
            cout<<"receive from "<<user_id<<":"<<buf<<endl;
            string str_temp_get(buf);
            str_temp_get = decode(str_temp_get);
            if(str_temp_get[0]=='1'){
                //create user
                cout<<"start creating"<<endl;
                string information[4];
                str_temp_get = str_temp_get.substr(2);
                stringstream ss(str_temp_get);
                int pos = 0;
                string temp;
                while(getline(ss, temp, ',')){
                    information[pos] = temp;
                    pos++;
                }
                for(int i = 0;i<4;i++){
                    cout<<information[i]<<endl;
                }
                
                user_P.set(information[3]);
                //create file
                //students
                //************************************************************************************************************
                //information[] 长度为4 0：ID 1：昵称  2：密码  3：身份  类型：string（string information[4]）
                //在这里创建用户的文件，不用初始化用户对象，因为创建完用户直接会回到登录界面，不会直接登录
                // user.init(atoi(information[0].c_str()),superBlock,inodeListMem,blockMem);
                int id = atoi(information[0].c_str());
                string name = information[1];
                int identity = atoi(information[3].c_str());
                if(identity==1){
                    //S
                    Student student(id,name,identity,superBlock,inodeListMem,blockMem);
                }
                else if(identity==2){
                    //T
                    Teacher teacher(id,name,identity,superBlock,inodeListMem,blockMem);
                }
                else if(identity==3){
                    //M
                    Manager m(id,name,identity,superBlock,inodeListMem,blockMem);
                }
                //***********************************************************************************************************
                sql = "insert into users values('"+information[0]+"','"+information[1]+"','"+information[2]+"',"+information[3]+")";
                cout<<"query:"<<sql<<endl;
                mysql_query(&mysql,sql.c_str());

                writeLog_for_DB(7,atoi(information[0].c_str()),information[2], superBlock, inodeListMem, blockMem);

                memset(buf,0,BUFF_LEN);
                sprintf(buf,"%c%s%s",ROOT_OP_FEEDBACK,user_id.c_str(),TERMINAL_ID);
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                cout<<"done"<<endl;
                sleep(1);
                goto WAIT1;
            }
            else{
                mysql_free_result(result);
                memset(buf,0,BUFF_LEN);
                sprintf(buf,"%c%s%s",ROOT_OP_FEEDBACK,user_id.c_str(),TERMINAL_ID);
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                cout<<"back"<<endl;
                sleep(1);
                goto WAIT1;
            }

        }//right 
        else{
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_ROOT_PASSWORD_WRONG,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
            goto WAIT1;
        }//wrong

    }


    //----------
    int extra_input_value = 0;  //0 represent user isn't asked to input an extra information, while 1 does
    string extra_output = "Extra information(input 'help' if need):\n";
    string options = "";
    string temp_buf = "";
    string last_time_input = "";
    target_path = "./";
    bool error_detect = true;
    int steps = 0;
    bool is_write = false;
    while(user_P.pos.first!='0'){
        //wait for ASK_FOR_OPTION_SHOW
        WAIT5:
        memset(buf,0,BUFF_LEN);
        recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        cout<<"receive applyment"<<endl;
        if(!isneed_accurate(ASK_FOR_OPTION_SHOW,TERMINAL_ID,user_id,buf)){
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                sleep(1);
                goto WAIT5;
        }
        if(extra_input_value==0){
            //prepare ASK_FOR_OPTION_SHOW
            user_P.select();
            for(int i = 0;i<user_P.true_options.size();i++){
                options+= to_string(i+1)+". "+user_P.true_options[i]+'\n';
            }
            options+= "Please enter your choice(or system command):\n";
        }
        else{
            options += "Input:\n";
        }
        // answer ASK_FOR_OPTION_SHOW
        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s%c%d%s%c%s",ASK_FOR_OPTION_SHOW_FEEDBACK,user_id.c_str(),TERMINAL_ID,char(user_P.true_options.size()+48),extra_input_value,extra_output.c_str(),'|',options.c_str());
        cout<<buf<<endl;
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
        // wait for POS_UPDATE_ASK
        WAIT6:
        memset(buf,0,BUFF_LEN);
        recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        if(!isneed_accurate(POS_UPDATE_ASK,TERMINAL_ID,user_id,buf)){
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                sleep(1);
                goto WAIT6;
        }
        // 
        extra_output = "Extra information(input 'help' if need):\n"; //initialize extra_output
        options = "";
        //deal the code here, decode(buf) to get the code
        //write a function to deal the code and send pos.first + pos.second + decode(buf) in to the function to get an extra output and change pos
        vector<string*> basic_information;
        basic_information.push_back(&user_id);//ip
        basic_information.push_back(&USER_ID);//
        basic_information.push_back(&USER_NAME);
        basic_information.push_back(&USER_IDENTITY);
        basic_information.push_back(&USER_PASSWORD);
        //code-----------------
        if(extra_input_value==0){
            string judge_temp = decode(buf);
            if(judge_temp.size()==1){
                steps = 0;
                error_detect = true;
                string buf_temp = decode(buf);
                string str_to_ch = buf_temp.substr(0,1);
                temp_buf = user_P.pos.first+user_P.pos.second+user_P.search_option(str_to_ch);
                extra_output += DEAL_THE_CODE(user_P.pos.first+user_P.pos.second+user_P.search_option(decode(buf)),
                &user_P,&extra_input_value,&error_detect,&steps,"",basic_information,
                &target_path,user,last_time_input);
                is_write = false;
                // extra_output += DEAL_THE_CODE(user_P.pos.first+user_P.pos.second+user_P.search_option(decode(buf)),
                // &user_P,&extra_input_value,&error_detect,&steps,"",basic_information,
                // &target_path,USER_ID,USER_IDENTITY);
            }
            else{
                extra_output += DEAL_THE_CODE("",&user_P,&extra_input_value,
                &error_detect,&steps,decode(buf),basic_information,&target_path,user,last_time_input);
                is_write = true;
                // extra_output += DEAL_THE_CODE(temp_buf,&user_P,&extra_input_value,
                // &error_detect,&steps,decode(buf),basic_information,&target_path,USER_ID,USER_IDENTITY);
            }
        }
        else{
            if(is_write){
                extra_output += DEAL_THE_CODE("",&user_P,&extra_input_value,&error_detect,&steps,decode(buf),
            basic_information,&target_path,user,last_time_input);
                is_write = false;
                // if()
            }
            else{
                extra_output += DEAL_THE_CODE(temp_buf,&user_P,&extra_input_value,&error_detect,&steps,decode(buf),
            basic_information,&target_path,user,last_time_input);
            }
            // extra_output += DEAL_THE_CODE(temp_buf,&user_P,&extra_input_value,&error_detect,&steps,decode(buf),
            // basic_information,&target_path,USER_ID,USER_IDENTITY);
        }
        //---------------------
        last_time_input = decode(buf);
        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s%c",POS_UPDATE_FEEDBACK,user_id.c_str(),TERMINAL_ID,user_P.pos.first);
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
        cout<<"pos message sends!"<<buf<<endl;

        //--i-node extend check
        LOCK.lock();
        inodeExtend(superBlock,inodeListMem,blockMem);
        LOCK.unlock();
    }
    //user quit, change current quality of users and release the ip
    LOCK.lock();
    NUM--;
    cout<<"Current users:"<<NUM<<endl;
    IP_STATE[atoi(user_id.c_str())-10001] = false;
    ID_search.erase(USER_ID);
    cout<<"Who is on:"<<endl;
    cout<<"----------------"<<endl;
    for(auto&n : ID_search){
        cout<<n<<endl;
    }
    cout<<"----------------"<<endl;
    LOCK.unlock();
}
void handle_msg(int fd,struct sockaddr* dst){

    char buf[BUFF_LEN];
    socklen_t len;
    int count;
    int user = 0;
    struct sockaddr_in clent_addr;
    while(true){
        BEGINS:
        char buf_temp[BUFF_LEN];
        //Init
        memset(buf_temp,0,BUFF_LEN);
        memset(buf,0,BUFF_LEN);
        //Listening
        len = sizeof(clent_addr);
        count = recvfrom(fd,buf_temp,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        if(count == -1){
            cout<<"Receive data error!"<<endl;
            return;
        }
        string str_temp(buf_temp);
        if(!isneed(LOGIN_CODE,TERMINAL_ID,str_temp)){
            //Not Log in
            sendto(fd,buf_temp,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
            goto BEGINS;
        }

        string str_temp2 = decode(str_temp);//Command,
        string str_user_code = get_region(str_temp);//userID

        //contribute ip to user
        int true_ip = 0;
        int temp_pointer = pointer;
        int flag = 0;
        while(flag!=2&&IP_STATE[pointer]){
            if(pointer==temp_pointer){
                flag++;
            }
            if(pointer==IP_MAXSIZE-1){
                pointer = 0;
            }
            pointer++;
        }
        //full
        if(flag==2){
            memset(buf,0,BUFF_LEN);
            //fail
            sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_FAIL,str_user_code.c_str(),TERMINAL_ID);
            sendto(fd,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
            goto BEGINS;
        }
        //success and send back
        true_ip = 10000+pointer+1;
        LOCK.lock();
        IP_STATE[pointer] = true;
        LOCK.unlock();
        // memset(buf,0,BUFF_LEN);
        // sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_SUCCESS,str_user_code.c_str(),TERMINAL_ID);
        cout<<"Receive:"<<true_ip<<" is on!"<<endl;
        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s%d",LOGIN_FEEDBACK_CODE_SUCCESS,str_user_code.c_str(),TERMINAL_ID,true_ip);
        sendto(fd,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len); 
        //login option receive
        thread(Mul_handle,to_string(true_ip),dst).detach();
        goto BEGINS;
    }

}
int main(){
    cout<<"Terminal program is on"<<endl;
    
    int UDP_fd = socket(AF_INET,SOCK_DGRAM,0);
    int RET;

    struct sockaddr_in server_address;
    if(UDP_fd == -1){
        cout<<"Network initialization error!"<<endl;
        exit(0);
    }
    else{
        cout<<"Network initialization success!"<<endl;
    }

    cout<<"-------------------------------------------"<<endl;

    cout<<"Server initialization begins:"<<endl;
    memset(&server_address,0,sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(SERVER_PORT);
    cout<<"Done!"<<endl;

    cout<<"Bind server to the socket:"<<endl;
    RET = bind(UDP_fd,(struct sockaddr*)&server_address,sizeof(server_address));
    if(RET<0){
        cout<<"Failed!"<<endl;
        exit(0);
    }
    else{
        cout<<"Done!"<<endl;
    }
    cout<<"IP state initialization begins:"<<endl;
    for(int i = 0;i<IP_MAXSIZE;i++){
        IP_STATE[i] = false;
    }
    pointer = 0;
    cout<<"Done!"<<endl;

    cout<<"Mysql initialization begins:"<<endl;
    mysql_init(&mysql); 
    //vicky
    // if(mysql_real_connect(&mysql,"localhost","root","123456","Project",0,NULL,0) == NULL){
    if(mysql_real_connect(&mysql,"localhost","Manager","123456","OS",0,NULL,0) == NULL){
	    cout << "connect fail!\n";
	    exit(0);
    }
    else{
        cout<<"connect success!\n";
    }

    cout<<"file system initialization begins:"<<endl;
    init_inode(superBlock,inodeListMem,blockMem);
    cout<<"Done!"<<endl;

    cout<<"-------------------------------------------"<<endl;
    cout<<"Start listening:"<<endl;
    FD = UDP_fd;
    handle_msg(UDP_fd, (struct sockaddr*)&server_address);


    cout<<"Terminal is off! Thanks for usage"<<endl;
}