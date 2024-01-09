#ifndef COURSE
#define COURSE

#include <string>
#include<iostream>
#include<cstdio>
// #include "FileSystem/inode.h"
#include "inode.h"
#include"CODES.h"
#include <sstream>
#include <unordered_map>
// #include "USER.h"
using namespace std;

class Course{
     private:
        string Info[3];//ID,Name,Credit
       
        int teacher;
        bool open;
        bool choose;
        bool close;
        vector<int>students;
        vector<string>HWname;//老师布置作业情况
        unordered_map<string,vector<int>>HW_S;//已交作业信息：<HW_name,studentsID>
        unordered_map<string,vector<int>>feedback_S;//已批改信息：<HW_name,studentsID>
    string FORMAT(string target, int len){
        string output(len,' ');
        for(int i = 0;i<len;i++){
            if(i<target.size()){
                output[i] = target[i];
            }
        }
        return output;
    }

    public:
    /*
    Init from info[3]:ID,,Name,Credit
    */
    Course(string info[3],SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        //Init from database
        for(int i =0;i<3;i++){
            Info[i]=info[i];
        }
        teacher=0;
        open=false;
        choose=false;
        close = false;
        //创建课程文件夹和config文件，写入课程config
        char CourseDir_path[100] ;
        string path_str ="/Course/"+Info[0];
        strcpy(CourseDir_path, path_str.c_str());
        vector<string>P(3);
        P[0]="00100000";
        P[1]="00100000";
        P[2]="11111110";
        string outputString;
        CreateD(CourseDir_path,outputString,P, superBlock,inodeListMem, blockMem);//创建课程文件夹
        char Config_path[100];
        path_str =path_str+"/Config";
        strcpy(Config_path, path_str.c_str());
        CreateF(Config_path,outputString, superBlock,inodeListMem, blockMem); //创建config文件
        Save_config(superBlock,inodeListMem, blockMem);
    }
    Course(string CourseID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        //Read from File system
        Read(CourseID,superBlock,inodeListMem, blockMem);
    }
    ~Course(){}
    /*
    
    */
    void Save_config(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        char Config_path[100];
        string Config_path_string = "/Course/"+Info[0]+"/Config";
        strcpy(Config_path, Config_path_string.c_str());
        string outputString ;
        if (1){
            //信息修改，则保存:          
            string inputLine=Info[0]+";"+Info[1]+";"+Info[2]+";";
            string num_str ;
            num_str = to_string(teacher);
            inputLine+=num_str+";";
            int open_int= open?1:0;
            num_str = to_string(open_int);
            inputLine+=num_str+";";
            int choose_int = choose?1:0;
            num_str = to_string(choose_int);
            inputLine+=num_str+";";
            int close_int = close?1:0;
            num_str = to_string(close_int);
            inputLine+=num_str+";";
            num_str = to_string(students.size());
            inputLine+=num_str+";";
            //students
            for(int i=0;i<students.size();i++){
              
                num_str = to_string(students[i]);
                inputLine+=num_str+";";
            }
            //HW
            num_str = to_string(HWname.size());
            inputLine+=num_str+";";
            for(int i=0;i<HWname.size();i++){
               
                num_str = HWname[i];
                inputLine+=num_str+";";
            }
            // //HW_S:unordered_map<string,vector<int>>
            // num_str = to_string(HW_S.size());
            // inputLine+=num_str+";";
            // for(pair<string,vector<int>>x:HW_S){
            //     inputLine+=x.first+";";
            //     int len=x.second.size();
            //     inputLine+=to_string(len);
            //     for(int i=0;i<len;i++){
            //         inputLine+=to_string(x.second[i])+";";
            //     }
            // }
            // //feedback_S:unordered_map<string,vector<int>>
            // num_str = to_string(feedback_S.size());
            // inputLine+=num_str+";";
            // for(pair<string,vector<int>>x:feedback_S){
            //     inputLine+=x.first+";";
            //     int len=x.second.size();
            //     inputLine+=to_string(len);
            //     for(int i=0;i<len;i++){
            //         inputLine+=to_string(x.second[i])+";";
            //     }
            // }
            //HW_S:unordered_map<string,vector<int>>
            num_str = to_string(HW_S.size());
            inputLine+=num_str+";";
            for(pair<string,vector<int>>x:HW_S){
                inputLine+=x.first+",";
                int len=x.second.size();
                inputLine+=to_string(len)+":";
                for(int i=0;i<len;i++){
                    inputLine+=to_string(x.second[i])+"&";
                }
            }
            inputLine+=";";
            //feedback_S:unordered_map<string,vector<int>>
            num_str = to_string(feedback_S.size());
            inputLine+=num_str+";";
            for(pair<string,vector<int>>x:feedback_S){
                inputLine+=x.first+",";
                int len=x.second.size();
                inputLine+=to_string(len)+":";
                for(int i=0;i<len;i++){
                    inputLine+=to_string(x.second[i])+"&";
                }
            }
            inputLine+=";";
            WriteF(Config_path,outputString,inputLine, superBlock,inodeListMem, blockMem);
        }
    } 
    void Read(string CourseID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        char Config_path[100];
        string path = "/Course/"+CourseID+"/Config";
        cout<<"Reading "+path<<endl;
        strcpy(Config_path,path.c_str());
        string output="";
        //Read and Spilt
        readFile(Config_path,output,superBlock,inodeListMem, blockMem);
        // cout<<output<<endl;
        // vector<string>token;
        // string temp_str="";
        // for(int i=0; i<output.length(); i++){
        //      string cur_char(1, output[i]);
        //     if(cur_char != ";"){
        //          temp_str += output[i];
        //      }
        //     else{
        //      token.push_back(temp_str);
        //      temp_str = "";
        //      }
        // }
        // if (!temp_str.empty()) {
        //     token.push_back(temp_str);
        // }
        // //Init from token
        // for(int i=0;i<3;i++){
        //     Info[i]=token[i];
        // }
        // //value
        // teacher = atoi(token[3].c_str());
        // open= atoi(token[4].c_str())==1?true:false;
        // choose=atoi(token[5].c_str())==1?true:false;
        // close=atoi(token[6].c_str())==1?true:false;
        // //vector<int>students&vector<string>hw
        // int students_num= atoi(token[7].c_str());
        // for(int i =0;i<students_num;i++){
        //     students.push_back(atoi(token[i+8].c_str())) ;
        // }
        // int hw_num= atoi(token[8+students_num].c_str());
        // for(int i =0;i<hw_num;i++){
        //     HWname.push_back(token[i+7+students_num].c_str()) ;
        // }
        // // HW_S: unordered_map<string, vector<int>>
        // int HW_S_size = atoi(token[8 + students_num + hw_num].c_str());
        // int current_index = 9 + students_num + hw_num;
        // for (int i = 0; i < HW_S_size; i++) {
        //     string hw_key = token[current_index++];
        //     int hw_vector_size = atoi(token[current_index++].c_str());
        //     vector<int> hw_vector;
        //     for (int j = 0; j < hw_vector_size; j++) {
        //         hw_vector.push_back(atoi(token[current_index++].c_str()));
        //     }
        //     HW_S[hw_key] = hw_vector;
        // }
        // // feedback_S: unordered_map<string, vector<int>>
        // int feedback_S_size = atoi(token[current_index++].c_str());
        // for (int i = 0; i < feedback_S_size; i++) {
        //     string feedback_key = token[current_index++];
        //     int feedback_vector_size = atoi(token[current_index++].c_str());
        //     vector<int> feedback_vector;
        //     for (int j = 0; j < feedback_vector_size; j++) {
        //         feedback_vector.push_back(atoi(token[current_index++].c_str()));
        //     }
        //     feedback_S[feedback_key] = feedback_vector;
        // }
        std::istringstream iss(output);
        std::string token;
        size_t pos;

        // Info
        for (int i = 0; i < 3; ++i) {
            std::getline(iss, token, ';');
            Info[i] = token;
        }

        // teacher
        std::getline(iss, token, ';');
        teacher = std::atoi(token.c_str());

        // open, choose, close
        for (int i = 0; i < 3; ++i) {
            std::getline(iss, token, ';');
            int value = std::atoi(token.c_str());
            switch (i) {
                case 0:
                    open = (value == 1);
                    break;
                case 1:
                    choose = (value == 1);
                    break;
                case 2:
                    close = (value == 1);
                    break;
            }
        }

        // students
        std::getline(iss, token, ';');
        int num_students = std::atoi(token.c_str());
        students.clear();
        for (int i = 0; i < num_students; ++i) {
            std::getline(iss, token, ';');
            students.push_back(std::atoi(token.c_str()));
        }

        // HWname
        std::getline(iss, token, ';');
        int num_HW = std::atoi(token.c_str());
        HWname.clear();
        for (int i = 0; i < num_HW; ++i) {
            std::getline(iss, token, ';');
            HWname.push_back(token);
        }

        // HW_S
        std::getline(iss, token, ';');
        int num_HW_S = std::atoi(token.c_str());
        HW_S.clear();
        for (int i = 0; i < num_HW_S; ++i) {
            std::getline(iss, token, ',');
            std::string key = token;

            std::getline(iss, token, ':');
            int len = std::atoi(token.c_str());

            std::getline(iss, token, ';');
            std::istringstream userIss(token);

            std::vector<int> values;
            for (int j = 0; j < len; ++j) {
                std::getline(userIss, token, '&');
                values.push_back(std::atoi(token.c_str()));
            }

            HW_S[key] = values;
        }

        // feedback_S
        std::getline(iss, token, ';');
        int num_feedback_S = std::atoi(token.c_str());
        feedback_S.clear();
        for (int i = 0; i < num_feedback_S; ++i) {
            std::getline(iss, token, ',');
            std::string key = token;

            std::getline(iss, token, ':');
            int len = std::atoi(token.c_str());

            std::getline(iss, token, ';');
            std::istringstream userIss(token);

            std::vector<int> values;
            for (int j = 0; j < len; ++j) {
                std::getline(userIss, token, '&');
                values.push_back(std::atoi(token.c_str()));
            }

            feedback_S[key] = values;
        }
    }
    /*
    Add student when choose course,success return 1;failture return 0;
    */
    string AddStudent(int studentID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        for(int i =0;i<students.size();i++){
            if(studentID==students[i]){
                return "already in.";
            }
        }
        students.push_back(studentID);
        //mark dirty
        Save_config(superBlock,inodeListMem,blockMem);
        //创建学生文件夹
        char StidentDir_path[100];
        string path ="/Course/"+Info[0]+"/" +to_string(studentID);
        strcpy(StidentDir_path,path.c_str());
        string outputString;
        vector<string>P(3);
        P[0]="00010000";
        P[1]="00100100";
        P[2]="00111110";
        CreateD(StidentDir_path,outputString,P, superBlock,inodeListMem, blockMem);//创建学生文件夹
        return outputString;
    }  
    bool RemoveStudent(int studentID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        int index=-1;
        for(int i =0;i<students.size();i++){
            if(studentID==students[i]){
                index=i;
                break;
            }
        }
        if(index==-1){
            return false;
        }
        //
        students.erase(students.begin()+index);
        Save_config(superBlock,inodeListMem,blockMem);
        //delete directory
        char StudentDir_path[100];
        string path="/Course/"+Info[0]+"/"+to_string(studentID);
        strcpy(StudentDir_path,path.c_str());
        string outputString;
        DeleteF(StudentDir_path,outputString, superBlock,inodeListMem,blockMem);
        return true;
    }
    vector<int> getStudents(){
        return students;
    }
    int getStudentNum(){
        return students.size();
    }
    
    /*选课*/
    void chooseCourse(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        choose = true;
        open = false;
        close = false;
        Save_config(superBlock,inodeListMem,blockMem);
    }
    /*
    open course:reset open, choose and HW_times;mark dirty
    */
    void openCourse(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        open = true;
        choose = false;
        close = false;
        Save_config(superBlock,inodeListMem,blockMem);
    }
    /*
    close course:reset open,choose and HW_times;mark dirty
    */
    void closeCourse(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        open = false;
        choose = false;
        close = true;
        Save_config(superBlock,inodeListMem,blockMem);
    }
    /*
    布置作业： create and write hw file
    param: HW_name,HW_conent
    */
    string assignHW(string HW_name,string HW_conent,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        
        //创建作业文件
        char HW_path[100] ;
        string path ="/Course/"+Info[0]+ "/" + to_string(teacher)+"/"+ HW_name;
        strcpy(HW_path,path.c_str());
        string output;
        CreateF(HW_path,output,superBlock,inodeListMem, blockMem);
        WriteF(HW_path,output,HW_conent,superBlock,inodeListMem, blockMem);
        //update HWname and HW_S
        HWname.push_back(HW_name);
        HW_S[HW_name] = vector<int>();
        feedback_S[HW_name]=vector<int>();
        Save_config(superBlock,inodeListMem, blockMem);
        return output;
    }
    //学生读取老师布置的作业
    string getHW(string HW_name,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        //读取作业文件
        char HW_path[100] ;
        string path ="/Course/"+Info[0]+ "/" + to_string(teacher)+"/"+ HW_name;
        strcpy(HW_path,path.c_str());
        string output;
        readFile(HW_path,output,superBlock,inodeListMem, blockMem);
        return output;
    }
    //老师读取学生提交的作业
    string getStudent_HW(int studentID,string HWname,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        //读取学生作业
        char HW_path[100];
        string path ="/Course/"+Info[0]+ "/" + to_string(studentID)+"/"+ HWname;
        strcpy(HW_path,path.c_str());
        string output;
        readFile(HW_path,output,superBlock,inodeListMem, blockMem);
        return output;
    }
    //学生提交作业
    string handInHW(string HW_name,string HW,int studentID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        //创建作业文件，并写入
        char HW_path[100];
        string path ="/Course/"+Info[0]+ "/" + to_string(studentID)+"/"+ HW_name;
        strcpy(HW_path,path.c_str());
        string output;
        CreateF(HW_path,output,superBlock,inodeListMem, blockMem);
        WriteF(HW_path,output,HW,superBlock,inodeListMem, blockMem);
        //update HW_S
        HW_S[HW_name].push_back(studentID);
        Save_config(superBlock,inodeListMem, blockMem);
        return output;
    }
    /*
    set Teacher and create directories of teacher and feedback
    param:teacherID and FileSystem
    */
    void setTeacher(int teacherID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        teacher = teacherID;
        //创建老师文件夹
        char TeacherDir_path[100];
        string path ="/Course/"+Info[0]+"/"+to_string(teacherID) ;
        strcpy(TeacherDir_path,path.c_str());
        string outputString;
        vector<string>P(3);
        P[0]="00100100";
        P[1]="00111110";
        P[2]="11111110";
        CreateD(TeacherDir_path,outputString,P, superBlock,inodeListMem, blockMem);//创建老师文件夹
        char FeedbackDir[100] ;
        path = "/Course/"+Info[0]+"/"+to_string(teacherID)+"/Feedback";
        strcpy(FeedbackDir,path.c_str());
        CreateD(FeedbackDir,outputString,P, superBlock,inodeListMem, blockMem);//创建feedback文件夹
        Save_config(superBlock,inodeListMem,blockMem);
    }
    string getInfo(){
        return FORMAT(Info[0],10)+FORMAT(Info[1],20)+FORMAT(Info[2],10);
        // return FORMAT2(Info[0],10)+FORMAT2(Info[1],10)+FORMAT2(Info[2],10);
    }
    /*get close*/
    bool getClose(){return this->close;}
    /*get choose*/
    bool getChoose(){return choose; }
    /*
    get open
    */
    bool getOpen(){return open;}
    vector<string> getHWName(){return this->HWname;}
    unordered_map<string, vector<int>> getFeedback_S(){return this->feedback_S;}
     /*获得作业反馈内容。有，则返回反馈内容；否则返回“feedback is empty”*/
    string getFeedback(int ID,string hwname,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        bool in =false;
        for(int x:feedback_S[hwname]){
            if(x==ID){
                in = true;
            }
        }
        if(!in){
            return "feedback is empty";
        }
        char feedback_path[100] ;
        string path ="/Course/"+Info[0]+ "/" + to_string(teacher)+"/"+ "/Feedback/"+hwname+"_"+to_string(ID);
        strcpy(feedback_path,path.c_str());
        string output;
        readFile(feedback_path,output,superBlock,inodeListMem, blockMem);
        return output;
    }
    //设置作业反馈
    string setFeedback(int ID,string hwname,int score,string comment,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        //upadte feedback_S
        feedback_S[hwname].push_back(ID);
        Save_config(superBlock,inodeListMem,blockMem);
        string inputLine=to_string(score)+"&"+comment;
        //FS
        char feedback_path[100] ;
        string path ="/Course/"+Info[0]+ "/" + to_string(teacher)+ "/Feedback/"+hwname+"_"+to_string(ID);
        strcpy(feedback_path,path.c_str());
        string output;
        CreateF(feedback_path,output,superBlock,inodeListMem,blockMem);
        WriteF(feedback_path,output,inputLine,superBlock,inodeListMem, blockMem);
        return output;
    }
    /*获得已交作业的学生*/
    unordered_map<string,vector<int>> getHW_S(){
        return this->HW_S;
    }
    int getCredit(){
        return atoi(this->Info[2].c_str());
    }
    //获得授课老师ID
    int getTeacher(){
        return this->teacher;
    }
    //检查作业名重复:不重复返回true；
    bool HWisUnique(string hwname){
        for(int i =0;i<this->HWname.size();i++){
            if(hwname==HWname[i]){
                return false;
            }
        }
        return true;
    }
 };
#endif