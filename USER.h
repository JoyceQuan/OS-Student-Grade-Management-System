#ifndef USER
#define USER
#include<iostream>
#include "inode.h"
#include "PRIVILEDGE.h"
#include <unordered_map>
#
// #include "utilize.h"
using namespace std;


#define Success 1
#define Failure -1
#define In -2

#define STUDENT 1
#define TEACHER 2
#define MANAGER 3
class User{
    private:
        //basicInfo
        int userID;//id,
        int identity;
        string userName;
        bool ChangeOtherPrivilege;//修改别人权限的权限
        //permission
        unordered_map<string,char>OtherFilePrivilege;//permission
        //cur，不用存到文件系统中
        string curCourseID;
        string curHWname;
        string curGroup;
        int changeUerID;
        int changeUserIdentity;
    public:
        /*
        从数据库初始化:userID,identity
        */

        /*创建用户目录，及其permission和basicInfo文件*/
        User(int userID,string name,int identity,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
        User();
        void init(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
        /*保存permission和basicInfo文件*/
        void Save(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
        void Read(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    
        //set privilege
        /*
        set ChangP:on=true,ChangP=1;on =false,changP=0
        */
        void setChangeOtherPrivilege(bool on,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
        /*
        append new file privilege. append successfully,return 1; path is already there, return-2
        */
        int appendFilePrivilege(string path,string privilege,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
        /*
        Change the specific bits of file privilege. append successfully,return 1; path is not one of them. return-1
        */
        int changeFilePrivilege(string path,int index,bool on,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
        /*
        Change the 8 bits of the file privilege. append successfully,return 1; path is not one of them. return-1
        */
        int changeFilePrivilege(string path,string privilege,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
        /*
        The file privilege is removed successfully. append,return 1; path is not one of them. return-1
        */
        int removeFilePrivilege(string path,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
        //get
        /*Get 8-bit permissions on specific files. If it fails, an empty string is returned*/
        string getFilePrivilege(string path);
        /*Gets bit-specific permissions for a file. If fails, return -1*/
        int getFilePrivilege(string path,int index);
        /*get ChangP(0/1）*/
        bool getChangeOtherPrivilege();
        /*get permission length*/
        int getOtherPrivilegeNum();
        //get permission
         unordered_map<string,char> getOtherFilePrivilege();

        /*获得userID*/
        int getUserID();
        /*获得identity*/
        int getIdentity();
        /*获得userName*/
        string getName();

        string getCurCourse();
        void setCurCourse(string courseID);
        string getCurHWname();
        void setCurHWname(string HWname);
        string getCurGroup();
        void setCurGroup(string group);

        //virtual

        //增加课程。成功，返回1；已在其中，返回-2
        //student
        virtual void initStudent(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
            cout<<"User::initStudent\n";
        };
        virtual void appendCourse(string courseID,int credit,int& res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){};
        virtual void setGrade(string courseID,int courseGrade,int& res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){};
        virtual void removeCourse(string courseID,int& res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){};
        virtual void getInfo(unordered_map<string,pair<int,int>>&res){};
        virtual void getGrade(string courseID,int& res){};
        virtual void getInfoNum(int& res){};
        //teacher
        virtual void appendCourse(string CourseID,int& res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){};
        // virtual int removeCourse(string CourseID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
        virtual void getCourse(string CourseID,bool& res){};
        //返回老师教授课程
        virtual void getCourses(vector<string>&res){};
        // virtual void getInfoNum(int& res){};

        //Manager
        // virtual void getChangeUserID(int& res){};
        // virtual void setChangeUserID(int id){};
        // virtual void setChangeUserIdentity(int& res){};
        // virtual void getChangeUserIdentity(int identity){};
        void getChangeUserID(int& res);
        void setChangeUserID(int id);
        void setChangeUserIdentity(int& res);
        void getChangeUserIdentity(int identity);
};
class Student:public User{
    public:
    /*创建学生目录和Course_info目录*/
    Student(int userID,string name,int identity,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

    Student(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    
    // void initStudent(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    
    /*保存info*/
    void Save(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    /*读取info*/
    void Read(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    void init(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    /*增加课程。成功，返回1；已在其中，返回-2*/
    void appendCourse(string courseID,int credit,int& res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    /*设置课程成绩。成功，返回1；课程不在其中，返回-1*/
    void setGrade(string courseID,int courseGrade,int& res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    /*移除课程。成功，返回1；课程不在其中，返回-1*/
    void removeCourse(string courseID,int& res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    /*查看所有课程*/
    void getInfo(unordered_map<string,pair<int,int>>& res);
     /*查看特定课程成绩。成功返回成绩，失败返回-1*/
    void getGrade(string courseID,int& res);
    void getInfoNum(int& res);

    //刷新用户
    void initStudent(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    private:
    unordered_map<string,pair<int,int>>info;//<CourseID,<学分，成绩>>
    
};
class Teacher:public User{
    public:
    /*Init from Database*/
     Teacher(int userID,string name,int identity,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    
    Teacher(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
 
    /*保存Course*/
    void Save(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    /*读取Course*/
    void Read(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    /*Init from file system*/
    void init(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);


    /*新增课程，成功返回1；已在其中返回-2*/
    void appendCourse(string CourseID,int& res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
     /*删除课程，成功返回1；不在其中返回-1*/
    void removeCourse(string CourseID,int & res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    /*是否选择courseID.已选择，返回true*/
    void getCourse(string CourseID,bool& res);
    void getCourses(vector<string>& res);
    void getInfoNum(int& res);
    private:
    vector<string>Info;//CourseID
};
class Manager:public User{
    public:
    /*Init from DB*/
    Manager(int userID,string name,int identity,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

    /*Init from file system*/
    Manager(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

    void init(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);
    //  void getChangeUserID(int& res);
    //  void setChangeUserID(int id);
    //  void setChangeUserIdentity(int& res);
    //  void getChangeUserIdentity(int identity);
    private:
    // int changeUerID;
    // int changeUserIdentity;
};

class Group{
    public:
    Group(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        read(superBlock,inodeListMem,blockMem);
    }
    void save(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        //save unordered_map<string,vector<pair<int,int>>>groups;//groupname:<id,identity> 
        // 将 unordered_map<string,vector<pair<int,int>>> groups 转换为字符串进行保存
       
        string outputString;
        for (const auto& entry : this->groups) {
            outputString += entry.first + ":"; // 添加课程ID

            // 遍历每个课程ID对应的用户ID和身份信息
            for (const auto& pair : entry.second) {
                outputString += std::to_string(pair.first) + "," + std::to_string(pair.second) + "&";
            }

            // 移除最后一个"&"
            if (!entry.second.empty()) {
                outputString.pop_back();
            }

            outputString += ";"; // 添加分隔符
        }
        char path_char[100];
        string path_str = "/Group";
        string output;
        strcpy(path_char,path_str.c_str());
        WriteF(path_char,output,outputString,superBlock,inodeListMem,blockMem);
    }
    void read(SuperBlock &superBlock, struct InodeList inodeListMem[MAX_INODELIST_NUMBER], struct FileBlock blockMem[BLOCK_NUMBER]) {  
        char path_char[100];
        string path_str = "/Group";
        string inputString;
        strcpy(path_char,path_str.c_str());
        readFile(path_char,inputString,superBlock,inodeListMem, blockMem);//read
        // 清空原有的 groups
        groups.clear();
        
        std::istringstream iss(inputString);
        // 通过分号分割每个课程信息
        std::string courseInfo;
        while (std::getline(iss, courseInfo, ';')) {
            std::istringstream courseIss(courseInfo);
            std::string courseID;
            std::getline(courseIss, courseID, ':'); // 提取课程ID

            std::vector<std::pair<int, int>> userPairs;
            std::string userPairInfo;
            while (std::getline(courseIss, userPairInfo, '&')) {
                std::istringstream userPairIss(userPairInfo);
                std::string userPair;
                std::pair<int, int> userPairValues;

                while (std::getline(userPairIss, userPair, ',')) {
                    if (userPairValues.first == 0) {
                        // userPairValues.first = std::stoi(userPair);
                        userPairValues.first = std::atoi(userPair.c_str());
                    } else {
                        // userPairValues.second = std::stoi(userPair);
                        userPairValues.second = std::atoi(userPair.c_str());

                    }
                }

                userPairs.push_back(userPairValues);
            }

            groups[courseID] = userPairs;
        }

        // 输出读取后的 groups，你可以在实际代码中去掉
        cout << "Read groups:" << endl;
        for (const auto &groupEntry : groups) {
            cout << groupEntry.first << ": ";
            for (const auto &member : groupEntry.second) {
                cout << "(" << member.first << "," << member.second << ") ";
            }
            cout << endl;
        }
    }
    
    unordered_map<string,vector<pair<int,int>>> getGroups(){return groups;}
    bool getGroups(string group){
        bool in=false;
        for(pair<string,vector<pair<int,int>>>x:groups){
            if(x.first==group){
                in=true;
            }
        }
        return in;
    }
    string createGroup(string group,SuperBlock &superBlock, struct InodeList inodeListMem[MAX_INODELIST_NUMBER], struct FileBlock blockMem[BLOCK_NUMBER]){
        string output="";
        if (groups.find(group) == groups.end()) {
            // If not, create a new entry with an empty vector
            groups[group] = vector<pair<int, int>>();
            output += "Group '" + group + "' created.\n"  ;
        } else {
            output +=  "Group '"+ group + "' already exists.\n";
        }
        save(superBlock,inodeListMem,blockMem);
        return output;
    }
    string addMember(int id ,int identity,string group,SuperBlock &superBlock, struct InodeList inodeListMem[MAX_INODELIST_NUMBER], struct FileBlock blockMem[BLOCK_NUMBER]){
        // vector<pair<int,int>>curGroup=groups[group];
        
       // Check if the group exists
        if (groups.find(group) != groups.end()) {
            // If the group exists, add the new member to its vector
            groups[group].push_back(make_pair(id, identity));
            save(superBlock,inodeListMem,blockMem);
            return "Member"+ to_string(id) +" added to group '" + group + "'.\n";
        } else {
            // If the group does not exist, return an error message
            return "Group '" + group + "' does not exist.";
        }
    }
    private:
    unordered_map<string,vector<pair<int,int>>>groups;//groupname:<id,identity>                                                                                                                                                                                                                                                 
};
#endif