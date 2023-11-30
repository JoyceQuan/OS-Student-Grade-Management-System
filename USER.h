#ifndef USER.h
#define USER.h
#include<iostream>
#include"PRIVILEDGE.h"
using namespace std;
#define MAX_CLASSNUM 30
#define CourseFull 0
class User{
    public:
        User(vector<string*>info){
            /*
            info=[ip,id,name,identity,password]
            */
            for(int i = 0;i<4;i++){
                Info[i] = *info[i+1];
            }
            user_priviledge = priviledge(false,Info[3]);
            ChangeOtherPrivilege=0;
            for(int i =0;i<MAX_CLASSNUM;i++){
                courses[i] ="";
            }
        }
        User(User u){
            /*
            Copy a user
            */
            for(int i = 0;i<4;i++){
                Info[i]=u.Info[i]
            }
            user_priviledge = priviledge(false,Info[3]);
            ChangeOtherPrivilege = u.ChangeOtherPrivilege;
            for(int i =0;i<MAX_CLASSNUM;i++){
                courses[i] = u.courses[i];
            }
            dirty = 0
        }
        User(string userID){
            /*
            Read from file system
            */
            
        }
        ~User(){
            /*
            Store into file system
            */
            if(dirty){
                //重新写入
                WriteUsertoFile()
            }
        }
        void WriteUsertoFile(){
            //学生ID，课程ID和成绩
        }
        
        string Info_user[5];//id,name,password,privilege
        vector<pair<int,int,int>>Info_courses;
 
        priviledge user_priviledge;
        string courses[MAX_CLASSNUM];//ID
        
        char group[BLOCK_SIZE-2*sizeof(int)];
        int ChangeOtherPrivilege;//修改别人权限的权限
        vector<pair<int,char[1]>>OtherFilePrivilege;//other privilege
        int dirty;
    private:
        void markDirty(){
            ditry=1;
        }
}
class Student:public User{
    public:
        Student()
        ~Student()
        int addCourse(string courseID){
            for(int i =0;i<MAX_CLASSNUM;i++){
                if(courses[i]==""){
                    courses[i]==courseID;
                    break
                }
            }
            if(i>=MAX_CLASSNUM){
                return CourseFull
            }
        }
        vector<pair<int,int,int>>Info_courses;
    private:
}
class Teacher:public User{
    public:
        Teacher(){

        }
        
    private:

}