#ifndef COURSE.h
#define COURSE.h
#include "USER.h"
#define Max_STUDENTS 150;
class Course{
    public:
    Course(string info[],User T){
        for(int i =0;i<3;i++){
            Info[i]=info[i];
        }
        teacher = User(T);
    }
    Course()
    void AddStudent(int studentID){
        students[students_num]=studentID;
        students_num++;
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
    void OpenCourse(){
        open = true;
    }
    void CloseCourse(){
        open = false;
    }
    void c
    private:
        string Info[3];//ID,Name,Credit
        int students[Max_STUDENTS];//学生信息
        int students_num;//当前学生数量，附带充当students数组的指针作用
        int teacher;
        bool open;
}