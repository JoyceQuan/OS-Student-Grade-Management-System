#include <iostream>
#include "inode.h"
#include "COURSE.h"
#include "USER.h"
#include "QUERY.h"
#include<mysql/mysql.h>
#include "CODES.h"
#include "PRIVILEDGE.h"
using namespace std;
void CourseTest(MYSQL mysql);
void StudentTest(MYSQL mysql);
void TeacherTest(MYSQL mysql);
void ManagerTest(MYSQL mysql);
void InitUser(MYSQL mysql);
void initCourse(MYSQL mysql);
SuperBlock superBlock;
struct InodeList inodeListMem[MAX_INODELIST_NUMBER];//inodelist空间,一共有124个inodelist   //212*124  //有层次结构，不需要用bitmap
struct FileBlock blockMem[BLOCK_NUMBER];//存block的空间，一共有128个block  //512*1120
int main() 
{
    //初始化部分
    init_inode(superBlock,inodeListMem,blockMem);
   
    //mysql init and connect
    MYSQL mysql;
    mysql_init(&mysql); 
    //vicky
    // if(mysql_real_connect(&mysql,"localhost","root","123456","Project",0,NULL,0) == NULL){
    if(mysql_real_connect(&mysql,"localhost","Manager","123456","OS",0,NULL,0) == NULL){
	    cout << "connect fail\n";
	    return -1;
    }
    else{
        cout<<"connect success\n";
    }
    //Test
    
    // StudentTest( mysql);
    // TeacherTest(mysql);
    // ManagerTest(mysql);
    // InitUser( mysql);
    // start_copy(superBlock,inodeListMem,blockMem);
    // initCourse(mysql);
    // start_copy(superBlock,inodeListMem,blockMem);
    int running = 1;
    int choice;
    while (running == 1) {
        printf("The system supports following commands:\n"
               "1. Create a directory\n"
               "2. Delete a directory\n"
               "3. List Files\n"
               "4. Create a file\n"
               "5. Delete a file\n"
               "6. Read a file\n"
               "7. Write a file\n"
               "9. Exit\n"
               "Input your command number:\n"
               );
        scanf("%i", &choice);
        char path[100];
        std::cout<<"Input the path:"<<std::endl;
        scanf("%s", path);
        std::string outputString;
        std::string inputLine;

        switch (choice)
        {
        case 1:{
            std::cout<<"input vector string:"<<std::endl;
            std::vector<std::string> P(3);
            // P[0]="11101010";
            // P[1]="11101010";
            // P[2]="11101010";
            std::cin>>P[0]>>P[1]>>P[2];
            //createDirectory(path,outputString,P,superBlock,inodeListMem,blockMem);
            
            CreateD(path,outputString,P,superBlock,inodeListMem,blockMem);
            break;
        }
        case 2:
            //deleteDirectory(path,outputString,superBlock,inodeListMem,blockMem);
            DeleteD(path,outputString,superBlock,inodeListMem,blockMem);
            break;

        case 3:
            listFiles(path,outputString,superBlock,inodeListMem,blockMem);
            //ls_only_name(path,outputString,0,superBlock,inodeListMem,blockMem);
            break;

        case 4:
            //createFile(path,outputString,superBlock,inodeListMem,blockMem);
            CreateF(path,outputString,superBlock,inodeListMem,blockMem);
            break;

        case 5:
            //deleteFile(path,outputString,superBlock,inodeListMem,blockMem);
            DeleteF(path,outputString,superBlock,inodeListMem,blockMem);
            break;

        case 6:
            readFile(path,outputString,superBlock,inodeListMem,blockMem);
            break;

        case 7:
            std::cout<<"Input the file:"<<std::endl;
            std::getline(std::cin, inputLine, '$');
            //writeFile(path,outputString,inputLine,superBlock,inodeListMem,blockMem);
            WriteF(path,outputString,inputLine,superBlock,inodeListMem,blockMem);
            break;

        case 8:
        {
            start_copy(superBlock,inodeListMem,blockMem);
            break;
        }
        case 10:
            writeLog_for_DB(8,64700141,"im the best",superBlock,inodeListMem,blockMem);
            writeLog_for_DB(7,64690213,"hihihihi",superBlock,inodeListMem,blockMem);
            break;

        case 11:
            // recover_From_Log(superBlock,inodeListMem,blockMem);
            break;

        case 9:
            exit(0);
        
        default:
            printf("Please input a valid command number\n");
            break;
        }
        std::cout<<"path:"<<path<<std::endl;
        std::cout<<outputString;
        //每次运行均检查是否需要拓展inode
        //inodeExtend(superBlock,inodeListMem,blockMem);
        // printf("The system supports following commands:\n"
        //        "1. Create a directory\n"
        //        "2. Delete a directory\n"
        //        "3. List Files\n"
        //        "4. Create a file\n"
        //        "5. Delete a file\n"
        //        "6. Read a file\n"
        //        "7. Write a file\n"
        //        "0. Exit\n"
        //        "Input your command number:\n"
        //        );
        // scanf("%i", &choice);
        // char path[100];
        // std::cout<<"Input the path:"<<std::endl;
        // scanf("%s", path);
        // std::string outputString;
        // std::string inputLine;
        // //std::cout<<check_Privilige_by_Path(choice-1,1,path,1,superBlock,inodeListMem,blockMem)<<std::endl;
        // switch (choice)
        // {
        // case 1:{
        //     std::cout<<"input vector string:"<<std::endl;
        //     std::vector<std::string> P(3);
        //     std::cin>>P[0]>>P[1]>>P[2];
        //     // check_Privilige_by_Path(choice,0,path,0,superBlock,inodeListMem,blockMem);
        //     CreateD(path,outputString,P,superBlock,inodeListMem,blockMem);
        //     break;
        // }
        // case 2:
        //     DeleteD(path,outputString,superBlock,inodeListMem,blockMem);
        //     break;

        // case 3:
        //     listFiles(path,outputString,superBlock,inodeListMem,blockMem);
        //     break;

        // case 4:
        //     CreateF(path,outputString,superBlock,inodeListMem,blockMem);
        //     break;

        // case 5:
        //     DeleteF(path,outputString,superBlock,inodeListMem,blockMem);
        //     break;

        // case 6:
        //     readFile(path,outputString,superBlock,inodeListMem,blockMem);
        //     break;

        // case 7:
        //     std::cout<<"Input the file:"<<std::endl;
           
        //     std::getline(std::cin, inputLine, '$');
        //     WriteF(path,outputString,inputLine,superBlock,inodeListMem,blockMem);
        //     break;

        // case 0:
        //     exit(0);
        
        // default:
        //     printf("Please input a valid command number\n");
        //     break;
        // }
        // std::cout<<outputString;
    } 
    return 0;

}
void CourseTest(MYSQL mysql){

    //query
    string sql = "select * from course where ID = 'CS101'";
    MYSQL_RES * result = NULL;
    basic_search(&mysql, &result, sql);
    MYSQL_ROW row = mysql_fetch_row(result);
    cout<<FORMAT(row[0],20)<<FORMAT(row[1],20)<<FORMAT(row[2],20)<<FORMAT(row[3],20)<<endl;

    //Course Creation
    string info[3];
    for(int i=0;i<3;i++){
        info[i]=row[i];
    }
    int teacherID = 700325;
    int studentID = 700318;
    {   
        cout<<"create CS101"<<endl;
        Course course_CS101(info,superBlock,inodeListMem,blockMem);
        //teacher 
        cout<<"create CS101/202164700325"<<endl;
        course_CS101.setTeacher(teacherID,superBlock,inodeListMem,blockMem);
        //student 
        course_CS101.chooseCourse(superBlock,inodeListMem,blockMem);
        cout<<"create CS101/202164700318"<<endl;
        string res = course_CS101.AddStudent(studentID,superBlock,inodeListMem,blockMem);
        cout<<res<<endl;
        cout<<"students:";
        int student_num = course_CS101.getStudentNum();
        vector<int> students = course_CS101.getStudents();
        cout<<student_num<<endl;
        for(int i=0;i<student_num;i++){
            cout<<students[i]<<endl;
        }
        
        //HW
        cout<<"assign HW_first:what is 1+1"<<endl;
        string HW_name ="fisrt";
        string HW_conent ="what is 1+1?";
        course_CS101.assignHW(HW_name,HW_conent,superBlock,inodeListMem,blockMem);
        cout<<"getHW\n";
        cout<<course_CS101.getHW(HW_name,superBlock,inodeListMem,blockMem)<<endl;
        //
        cout<<"saving\n";
        course_CS101.Save_config(superBlock,inodeListMem,blockMem);
    }
    
    
    Course course_CS101(info[0],superBlock,inodeListMem,blockMem);  

    int student_num = course_CS101.getStudentNum();
    cout<<student_num<<endl;
    

    course_CS101.RemoveStudent(studentID,superBlock,inodeListMem,blockMem);
    student_num = course_CS101.getStudentNum();
    cout<< course_CS101.getStudentNum()<<endl;
    cout<<"saving\n";
    course_CS101.Save_config(superBlock,inodeListMem,blockMem);
}

void StudentTest(MYSQL mysql){

    string sql = "select * from users where ID = '700141'";
    MYSQL_RES * result = NULL;
    basic_search(&mysql, &result, sql);
    MYSQL_ROW row = mysql_fetch_row(result);
    // cout<<FORMAT(row[0],20)<<FORMAT(row[1],20)<<FORMAT(row[2],20)<<FORMAT(row[3],20)<<endl;
    string info[4];
    for (int i=0;i<4;i++){
        info[i]=row[i];
    }
    int id = atoi(info[0].c_str());
    string name = info[1];
    int identity = atoi(info[3].c_str());
    cout<<id<<"\t"<<name<<"\t"<<identity<<endl;
    User* user=NULL;
    //init from DB
    cout<<"init from DB\n";
    user = new Student(id,name,identity,superBlock,inodeListMem,blockMem);
    // Student s(id,name,identity,superBlock,inodeListMem,blockMem);
    //load
    cout<<"init from FS\n";
    delete user;
    user = nullptr;
    user = new Student(id,superBlock,inodeListMem,blockMem);
    cout<<endl<<endl<<endl;
    //show
    cout<<"---reload 1"<<endl;
    cout<<"show:"<<user->getUserID()<<"\t"<<user->getName()<<"\t"<<user->getIdentity()<<endl;
    cout<<"ChangeP:"<<to_string(user->getChangeOtherPrivilege()==0)<<endl;//0
    cout<<"Permission:"<<to_string(user->getOtherPrivilegeNum()==0)<<endl;//0
    // //set
    user->setChangeOtherPrivilege(true,superBlock,inodeListMem,blockMem);
    user->appendFilePrivilege("/Course","11111110",superBlock,inodeListMem,blockMem);
    user->appendFilePrivilege("/User","01111110",superBlock,inodeListMem,blockMem);
    //show
    cout<<"------after setting ChangeP and appending file privilege\n";
    cout<<"ChangeP:"<<(user->getChangeOtherPrivilege()==1)<<endl;//1
    cout<<"Permission:"<<(user->getOtherPrivilegeNum()==2)<<endl;//2
    cout<<"/Course:"<<(user->getFilePrivilege("/Course")=="11111110")<<endl;//11111110
    //change &show
    cout<<"-----after changing\n";
    user->changeFilePrivilege("/Course","01111110",superBlock,inodeListMem,blockMem);
    cout<<"/Course:"<<(user->getFilePrivilege("/Course")=="01111110")<<endl;//01111110
    user->changeFilePrivilege("/Course",1,false,superBlock,inodeListMem,blockMem);
    cout<<"/Course:"<<(user->getFilePrivilege("/Course")=="00111110")<<endl;//00111110

    //append&show course
    cout<<"-----after appending course\n";
    int res ;
    user->appendCourse("CS101",3,res,superBlock,inodeListMem,blockMem);
    user->appendCourse("AI132",3,res,superBlock,inodeListMem,blockMem);
    unordered_map<string,pair<int,int>> map ;
    // for(auto x:map){
    //     cout<<x.first<<"\t"<<x.second.first<<"\t"<<x.second.second<<endl;
    // }
    user->setGrade("CS101",100,res,superBlock,inodeListMem,blockMem);
    user->getGrade("CS101",res);
    cout<<"CS101 grade:"<<res<<endl;
    user->getInfo(map);
    for(auto x:map){
        cout<<x.first<<"\t"<<x.second.first<<"\t"<<x.second.second<<endl;
    }
    cout<<"---"<<endl;
    cout<<"Permission  2:"<<user->getOtherPrivilegeNum()<<endl<<endl;//2
    //load
    delete user;
    user = nullptr;
    user = new Student(id,superBlock,inodeListMem,blockMem);
     cout<<endl<<endl<<endl;
    //show
    cout<<"-----after reloading 2\n";
    cout<<user->getUserID()<<"\t"<<user->getName()<<"\t"<<user->getIdentity()<<endl;
    cout<<"ChangeP:"<<(user->getChangeOtherPrivilege()==1)<<endl;//1
    cout<<"Permission  2:"<<user->getOtherPrivilegeNum()<<endl;//2
    cout<<"/Course:"<<user->getFilePrivilege("/Course")<<endl;//00111110
    user->getInfo(map);
    for(auto x:map){
        cout<<x.first<<"\t"<<x.second.first<<"\t"<<x.second.second<<endl;
    }
    //remove&show
    cout<<"after removing file privilege and course\n";
    user->removeFilePrivilege("/User",superBlock,inodeListMem,blockMem);//
    user->removeCourse("AI132",res,superBlock,inodeListMem,blockMem);//wrong
    cout<<"remove AI132 res:"<<res<<endl;
    cout<<"Permission:"<<(user->getOtherPrivilegeNum()==1)<<endl;//1
    user->getInfo(map);
    for(auto x:map){
        cout<<x.first<<"\t"<<x.second.first<<"\t"<<x.second.second<<endl;
    }
    //load &show
    cout<<"-----after reloading 3\n";
    delete user;
    user = nullptr;
    user = new Student(id,superBlock,inodeListMem,blockMem);
     cout<<endl<<endl<<endl;
    //show
    cout<<"after reloading 3\n";
    cout<<user->getUserID()<<"\t"<<user->getName()<<"\t"<<user->getIdentity()<<endl;
    cout<<"ChangeP:"<<(user->getChangeOtherPrivilege()==1)<<endl;//1
    cout<<"Permission:"<<(user->getOtherPrivilegeNum()==1)<<endl;
    cout<<"/Course:"<<user->getFilePrivilege("/Course")<<endl;//00111110
    user->getInfo(map);
    for(auto x:map){
        cout<<x.first<<"\t"<<x.second.first<<"\t"<<x.second.second<<endl;
    }

    delete user;
    user = nullptr;
}

void TeacherTest(MYSQL mysql){
    string sql = "select * from users where ID = '700325'";
    MYSQL_RES * result = NULL;
    basic_search(&mysql, &result, sql);
    MYSQL_ROW row = mysql_fetch_row(result);
    // cout<<FORMAT(row[0],20)<<FORMAT(row[1],20)<<FORMAT(row[2],20)<<FORMAT(row[3],20)<<endl;
    string info[4];
    for (int i=0;i<4;i++){
        info[i]=row[i];
    }
    int id = atoi(info[0].c_str());
    string name = info[1];
    int identity = atoi(info[3].c_str());
    cout<<id<<"\t"<<name<<"\t"<<identity<<endl;
    User* user=NULL;
    //
    //init from DB
    cout<<"init from DB\n";
    user = new Teacher(id,name,identity,superBlock,inodeListMem,blockMem);
    // Student s(id,name,identity,superBlock,inodeListMem,blockMem);
    //load
    cout<<"init from FS\n";
    delete user;
    user = nullptr;
    user = new Teacher(id,superBlock,inodeListMem,blockMem);
    cout<<endl<<endl;
    //show
    cout<<"---reload 1"<<endl;
    cout<<"show:"<<user->getUserID()<<"\t"<<user->getName()<<"\t"<<user->getIdentity()<<endl;
    cout<<"ChangeP:"<<(user->getChangeOtherPrivilege()==0? "True" : "False")<<endl;//0
    cout<<"Permission:"<<(user->getOtherPrivilegeNum()==0? "True" : "False")<<endl;//0
    //setshow
    user->setChangeOtherPrivilege(true,superBlock,inodeListMem,blockMem);
    user->appendFilePrivilege("/Course","11111110",superBlock,inodeListMem,blockMem);
    user->appendFilePrivilege("/User","01111110",superBlock,inodeListMem,blockMem);
    
    cout<<"------after setting ChangeP and appending file privilege\n";
    cout<<"ChangeP:"<<(user->getChangeOtherPrivilege()==1? "True" : "False")<<endl;//1
    cout<<"Permission:"<<(user->getOtherPrivilegeNum()==2? "True" : "False")<<endl;//2
    cout<<"/Course:"<<(user->getFilePrivilege("/Course")=="11111110"? "True" : "False")<<endl;//11111110
    //change &show
    cout<<"-----after changing\n";
    user->changeFilePrivilege("/Course","01111110",superBlock,inodeListMem,blockMem);
    cout<<"/Course:"<<(user->getFilePrivilege("/Course")=="01111110"? "True" : "False")<<endl;//01111110
    user->changeFilePrivilege("/Course",1,false,superBlock,inodeListMem,blockMem);
    cout<<"/Course:"<<(user->getFilePrivilege("/Course")=="00111110"? "True" : "False")<<endl;//00111110

    //append&show course
    cout<<"-----after appending course\n";
    int res ;
    user->appendCourse("CS101",res,superBlock,inodeListMem,blockMem);
    user->appendCourse("AI132",res,superBlock,inodeListMem,blockMem);
    vector<string>courses;
    user->getCourses(courses);
    for(auto x:courses){
        cout<<x<<"\t";
    }
    cout<<"\n---"<<endl;
    cout<<"Permission  2:"<<user->getOtherPrivilegeNum()<<endl<<endl;//2
    //load
    delete user;
    user = nullptr;
    user = new Teacher(id,superBlock,inodeListMem,blockMem);
    cout<<endl<<endl;
    //show
    cout<<"-----after reloading 2\n";
    cout<<user->getUserID()<<"\t"<<user->getName()<<"\t"<<user->getIdentity()<<endl;
    cout<<"ChangeP:"<<(user->getChangeOtherPrivilege()==1? "True" : "False")<<endl;//1
    cout<<"Permission  2:"<<user->getOtherPrivilegeNum()<<endl;//2
    cout<<"/Course:"<<user->getFilePrivilege("/Course")<<endl;//00111110
    courses.clear();
    user->getCourses(courses);
    for(auto x:courses){
        cout<<x<<"\t";
    }
    cout<<"\n---"<<endl;
    //remove&show
    cout<<"after removing file privilege and course\n";
    user->removeFilePrivilege("/User",superBlock,inodeListMem,blockMem);//
    user->removeCourse("AI132",res,superBlock,inodeListMem,blockMem);//
    cout<<"remove AI132 res:"<<res<<endl;
    cout<<"Permission:"<<(user->getOtherPrivilegeNum()==1? "True" : "False")<<endl;//1
    courses.clear();
    user->getCourses(courses);
    for(auto x:courses){
        cout<<x<<"\t";
    }
    cout<<"\n---"<<endl;
    //load &show
    cout<<"-----after reloading 3\n";
    delete user;
    user = nullptr;
    user = new Teacher(id,superBlock,inodeListMem,blockMem);
     cout<<endl<<endl;
    //show
    cout<<"after reloading 3\n";
    cout<<user->getUserID()<<"\t"<<user->getName()<<"\t"<<user->getIdentity()<<endl;
    cout<<"ChangeP:"<<(user->getChangeOtherPrivilege()==1? "True" : "False")<<endl;//1
    cout<<"Permission:"<<(user->getOtherPrivilegeNum()==1? "True" : "False")<<endl;
    cout<<"/Course:"<<user->getFilePrivilege("/Course")<<endl;//00111110
    courses.clear();
    user->getCourses(courses);
    for(auto x:courses){
        cout<<x<<"\t";
    }
    cout<<"\n---"<<endl;

    delete user;
    user = nullptr;
}

void ManagerTest(MYSQL mysql){
    string sql = "select * from users where ID = '700326'";
    MYSQL_RES * result = NULL;
    basic_search(&mysql, &result, sql);
    MYSQL_ROW row = mysql_fetch_row(result);
    // cout<<FORMAT(row[0],20)<<FORMAT(row[1],20)<<FORMAT(row[2],20)<<FORMAT(row[3],20)<<endl;
    string info[4];
    for (int i=0;i<4;i++){
        info[i]=row[i];
    }
    int id = atoi(info[0].c_str());
    string name = info[1];
    int identity = atoi(info[3].c_str());
    cout<<id<<"\t"<<name<<"\t"<<identity<<endl;
    User* user=NULL;
    //
    //init from DB
    cout<<"init from DB\n";
    user = new Manager(id,name,identity,superBlock,inodeListMem,blockMem);
    // Student s(id,name,identity,superBlock,inodeListMem,blockMem);
    //load
    cout<<"init from FS\n";
    delete user;
    user = nullptr;
    user = new Manager(id,superBlock,inodeListMem,blockMem);
    cout<<endl<<endl;
    //show
    cout<<"---reload 1"<<endl;
    cout<<"show:"<<user->getUserID()<<"\t"<<user->getName()<<"\t"<<user->getIdentity()<<endl;
    cout<<"ChangeP:"<<(user->getChangeOtherPrivilege()==0? "True" : "False")<<endl;//0
    cout<<"Permission:"<<(user->getOtherPrivilegeNum()==0? "True" : "False")<<endl;//0
    //setshow
    user->setChangeOtherPrivilege(true,superBlock,inodeListMem,blockMem);
    user->appendFilePrivilege("/Course","11111110",superBlock,inodeListMem,blockMem);
    user->appendFilePrivilege("/User","01111110",superBlock,inodeListMem,blockMem);
    
    cout<<"------after setting ChangeP and appending file privilege\n";
    cout<<"ChangeP:"<<(user->getChangeOtherPrivilege()==1? "True" : "False")<<endl;//1
    cout<<"Permission:"<<(user->getOtherPrivilegeNum()==2? "True" : "False")<<endl;//2
    cout<<"/Course:"<<(user->getFilePrivilege("/Course")=="11111110"? "True" : "False")<<endl;//11111110
    //change &show
    cout<<"-----after changing\n";
    user->changeFilePrivilege("/Course","01111110",superBlock,inodeListMem,blockMem);
    cout<<"/Course:"<<(user->getFilePrivilege("/Course")=="01111110"? "True" : "False")<<endl;//01111110
    user->changeFilePrivilege("/Course",1,false,superBlock,inodeListMem,blockMem);
    cout<<"/Course:"<<(user->getFilePrivilege("/Course")=="00111110"? "True" : "False")<<endl;//00111110

    cout<<"Permission  2:"<<user->getOtherPrivilegeNum()<<endl<<endl;//2
    //load
    delete user;
    user = nullptr;
    user = new Manager(id,superBlock,inodeListMem,blockMem);
    cout<<endl<<endl;
    //show
    cout<<"-----after reloading 2\n";
    cout<<user->getUserID()<<"\t"<<user->getName()<<"\t"<<user->getIdentity()<<endl;
    cout<<"ChangeP:"<<(user->getChangeOtherPrivilege()==1? "True" : "False")<<endl;//1
    cout<<"Permission  2:"<<user->getOtherPrivilegeNum()<<endl;//2
    cout<<"/Course:"<<user->getFilePrivilege("/Course")<<endl;//00111110
   
    //remove&show
    cout<<"after removing file privilege and course\n";
    user->removeFilePrivilege("/User",superBlock,inodeListMem,blockMem);//
    cout<<"Permission:"<<(user->getOtherPrivilegeNum()==1? "True" : "False")<<endl;//1
   
    //load &show
    cout<<"-----after reloading 3\n";
    delete user;
    user = nullptr;
    user = new Manager(id,superBlock,inodeListMem,blockMem);
     cout<<endl<<endl;
    //show
    cout<<"after reloading 3\n";
    cout<<user->getUserID()<<"\t"<<user->getName()<<"\t"<<user->getIdentity()<<endl;
    cout<<"ChangeP:"<<(user->getChangeOtherPrivilege()==1? "True" : "False")<<endl;//1
    cout<<"Permission:"<<(user->getOtherPrivilegeNum()==1? "True" : "False")<<endl;
    cout<<"/Course:"<<user->getFilePrivilege("/Course")<<endl;//00111110
    

    delete user;
    user = nullptr;
}

void InitUser(MYSQL mysql){
    string sql;
    MYSQL_RES * result = NULL;
    MYSQL_ROW row;
    //init user
    // sql = "select * from users where ID = '700141'";
    sql = "select * from users";
    result = NULL;
    basic_search(&mysql, &result, sql);
    //MYSQL_ROW row;// = mysql_fetch_row(result);
    // while()
    // cout<<FORMAT(row[0],20)<<FORMAT(row[1],20)<<FORMAT(row[2],20)<<FORMAT(row[3],20)<<endl;
    User* user=NULL;
    string info[4];
    //init from DB
    while ((row = mysql_fetch_row(result))) {
        // Iterate through columns and print values
        // string info[4];//
        for (unsigned int i = 0; i < mysql_num_fields(result); i++) {
            // std::cout << row[i] << "\t";
            info[i]=row[i];
        }
        cout<<endl;
        int id = atoi(info[0].c_str());
        string name = info[1];
        int identity = atoi(info[3].c_str());
        cout<<id <<"\t"+name+"\t"<<identity<<endl;
        if(identity==1){Student s(id,name,identity,superBlock,inodeListMem,blockMem);}
        else if(identity==2){Teacher t(id,name,identity,superBlock,inodeListMem,blockMem);}
        else if(identity==3 && name!="root"){
            Manager m(id,name,identity,superBlock,inodeListMem,blockMem);
        }
        
    }
    
    delete user;
    user=nullptr;
}

void initCourse(MYSQL mysql){
    string sql;
    MYSQL_RES * result = NULL;
    MYSQL_ROW row;
    // init courses
    // query
    sql = "select * from course;";
    result = NULL;
    basic_search(&mysql, &result, sql);
    
    // while()
    // cout<<FORMAT(row[0],20)<<FORMAT(row[1],20)<<FORMAT(row[2],20)<<FORMAT(row[3],20)<<endl;
    while ((row = mysql_fetch_row(result))) {
        // Iterate through columns and print values
        string info[3];
        for (unsigned int i = 0; i < mysql_num_fields(result); i++) {
            // std::cout << row[i] << "\t";
            info[i]=row[i];
        }
        Course course(info,superBlock,inodeListMem,blockMem);
        // std::cout << std::endl;
    }
}
