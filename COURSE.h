#ifndef COURSE
#define COURSE

#include <string>
#include "FileSystem/inode.h"
// #include "USER.h"

#define Max_STUDENTS 150;
using namespace std;

const string defaultPriviledge_Student= "00100000";
const string defaultPriviledge_Teacher= "00100000";
const string defaultPriviledge_Manager= "11111110";
class Course{
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
    public:
    Course(string info[3],SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        //Init from database
        for(int i =0;i<3;i++){
            Info[i]=info[i];
        }
        open=false;
        choose=false;
        HomeworkTimes=-1;
        students_num=0;
        dirty = 1;
        //创建课程文件夹和config文件，写入课程config
        char CourseDir_path[100] = "/Course/Course_"+Info[0]; 
        vector<string>P(3);
        P[0]=defaultPriviledge_Student;
        P[1]=defaultPriviledge_Teacher;
        P[2]=defaultPriviledge_Manager;
        string outputString;
        CreateD(CourseDir_path,outputString,P, superBlock,inodeListMem, blockMem);//创建课程文件夹
        char Config_path[100]= CourseDir_path+"/Config";
        CreateF(Config_path,outputString, superBlock,inodeListMem, blockMem); //创建config文件
        Save_config()
    }
    Course(string CourseID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        //Read from File system
        Read(CourseID,superBlock,inodeListMem, blockMem)
    }
    ~Course(){}
    void Save_config(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        char Config_path[100]=  "/Course/Course_"+Info[0]+"/Config";
        string outputString ;
        if (dirty){
            //信息修改，则保存
            char num_str[15];          
            string inputLine=Info[0]+";"+Info[1]+";"+Info+";";
            sprintf_s(num_str, "%d", teacher);
            inputLine+=num_str+";";
            int open_int= open?1:0;
            sprintf_s(num_str, "%d", open_int);
            inputLine+=num_str+";";
            int choose_int = choose?1:0;
            sprintf_s(num_str, "%d", choose_int);
            inputLine+=num_str+";";
            sprintf_s(num_str, "%d", HomeworkTimes);
            inputLine+=num_str+";";
            sprintf_s(num_str, "%d", students_num);
            inputLine+=num_str+";";
            for(int i=0;i<students;i++){
                sprintf_s(num_str, "%d", students[i]);
                inputLine+=num_str+";";
            }
            WriteF(Config_path,outputString,inputLine, superBlock,inodeListMem, blockMem);
        }
    } 
    void Read(string CourseID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        dirty = 0;
        char Config_path[100]=  "/Course/Course_"+CourseID+"/Config";
        string output="";
        //Read and Spilt
        readFile(Config_path,output,superBlock,inodeListMem, blockMem);
        vector<string>token;
        string temp_str="";
        for(int i=0;i<output.length();i++){
            if(output[i]!=";"){
                temp_str+=output[i];
            }
            else{
                token.append(temp_str);
                temp_str="";
            }
        }
        //Init from token
        for(int i=0;i<3;i++){
            Info[i]=token[i];
        }
        teacher = stoi(token[3]);
        open= stoi(token[4])==1?true:false;
        choose=stoi(token[5])==1?true:false;
        HomeworkTimes= stoi(token[6]);
        students_num= stoi(token[7]);
        for(int i =0;i<students_num;i++){
            students[i]=stoi(token[i+8]);
        }

    }
    void AddStudent(int studentID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
        students[students_num]=studentID;
        students_num++;
        //mark dirty
        markDirty();
        //创建学生文件夹
        char StidentDir_path[100]="/Course/"+Info[0]+
        CreateD(StidentDir_path,outputString,P, superBlock,inodeListMem, blockMem);//创建课程文件夹
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
    
    
   
}
#endif