#ifndef COURSE
#define COURSE

#include <string>
#include "FileSystem/inode.h"
// #include "USER.h"

#define Max_STUDENTS 150;
using namespace std;
class Course{
    public:
    Course(string info[3]){
        //Init from database
        for(int i =0;i<3;i++){
            Info[i]=info[i];
        }
        open=false;
        choose=false;
        HomeworkTimes=-1;
        students_num=0;
        
        //创建课程文件夹和config文件，写入课程config
        
    }
    Course(){
        //Read from File system
    }
    ~Course(){}
    void Save(){
        string basePath ="/Course/"+Info[0];
        
    } 
    void Read(){

    }
    void AddStudent(int studentID){
        students[students_num]=studentID;
        students_num++;
        //mark dirty
        
        //创建学生文件夹
    }
    
    bool RemoveStudent(int studentID){
        for(int i =0;i<students_num;i++){
            if(students[i]==studentID){
                students[i]=students[students_num-1];
                students_num--;
                return true;
            }
        }
        return false;
    }
    int[] getStudents(){
        return students;
    }
    int getStudentNum(){
        return students_num;
    }
    bool getOpen(){
        return open;
    }
    void openCourse(){
        open = true;
        HomeworkTime=0;
    }
    void closeCourse(){
        open = false;
        HomeworkTimes=-1;
    }
    void assignHW(string HW_name){
        HomeworkTimes++;
        //创建作业文件
        
    }
    void setTeacher(int teacherID){
        teacher = teacherID;
        //创建老师文件夹
    }
    
    
    private:
        void markDirty(){
            dirty=1;
        }
        string Info[3];//ID,Name,Credit
        int students[Max_STUDENTS];//学生信息
        int students_num;//当前学生数量，附带充当students数组的指针作用
        int teacher;
        bool open;
        bool choose;
        int HomeworkTimes;
        int dirty;//信息是否修改
}
#endif