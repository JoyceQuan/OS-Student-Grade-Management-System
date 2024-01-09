#include "CODES.h"
#include <iostream>
// #include  "FileSystem/inode.h"
#include  "inode.h"
#include  "PRIVILEDGE.h"
#include <unordered_map>
#include "USER.h"

using namespace std;

/*
从数据库初始化:userID,identity
*/

/*创建用户目录，及其permission和basicInfo文件*/
User::User(int userID,string name,int identity,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER])
:userID(userID),identity(identity),userName(name)
{
    if (inodeListMem == nullptr || blockMem == nullptr) {
        cout<<"FS fault"<<endl;
    }
    cout<<this->userID<<"\t"<<this->userName<<"\t"<<this->identity<<endl;       
    ChangeOtherPrivilege=identity==MANAGER?true:false;
    //create directory
    char path_char[100];
    string path_str="/User/"+to_string(userID);
    vector<string>P(3);
    string output;
    if(identity==STUDENT){
        P[0]="00100110";
        P[1]="00000000";
    }else if (identity==TEACHER)
    {
        P[1]="00100110";
        P[0]="00000000";
    }
    P[2]="00111110";
    strcpy(path_char,path_str.c_str());
    CreateD(path_char,output,P,superBlock,inodeListMem,blockMem);//创建User/<userID>目录
    cout<<"create directory "<<path_str<<"\t"<<output<<endl;
    //create Files:permission and BasicInformation
    string Permission_str = path_str+"/permission";
    strcpy(path_char,Permission_str.c_str());
    CreateF(path_char,output,superBlock,inodeListMem,blockMem);//创建User/<userID>/permission文件
    cout<<"create permission:"<<Permission_str<<"\t"<<output<<endl;
    string basicInfoPath_str = path_str+"/basicInfo";
    strcpy(path_char,basicInfoPath_str.c_str());
    CreateF(path_char,output,superBlock,inodeListMem,blockMem);//创建User/<userID>/basicInfo
    cout<<"create basicInfo:"<<basicInfoPath_str<<"\t"<<output<<endl;
    //save
    Save(superBlock,inodeListMem,blockMem);
};

void User::init(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    /*
    Read from file system
    */
    Read(userID,superBlock,inodeListMem,blockMem);
}
User::User(){}
/*保存permission和basicInfo文件*/
void User::Save(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    //save permission and basicInfo
    char Path[100];
    string basePath_str = "/User/"+to_string(userID);
    string outputString ;
    cout<<"----User::Save\nUser Save:"+basePath_str<<endl;
    if(1){
        //permission    
        int pri_num =getOtherPrivilegeNum();
        string inputLine=to_string(pri_num)+";";
        for(pair<string,char> filePrivilege:OtherFilePrivilege){
            string second = Char_to_Eight_Bit(filePrivilege.second);
            inputLine+=filePrivilege.first+";"+second+";";
        }
        string path_str = basePath_str+"/permission";
        strcpy(Path,path_str.c_str());
        WriteF(Path,outputString,inputLine,superBlock,inodeListMem,blockMem);
        cout<<"User Save permission:"+path_str+"\t"+outputString<<endl;
        //Base Info
        inputLine="";
        inputLine+=to_string(userID)+";";
        inputLine+=this->userName+";";
        inputLine+=to_string(identity)+";";
        int changeP = ChangeOtherPrivilege?1:0;
        inputLine+=to_string(changeP)+";";
        cout<<inputLine<<endl;
        path_str = basePath_str+"/basicInfo";
        strcpy(Path,path_str.c_str());
        WriteF(Path,outputString,inputLine,superBlock,inodeListMem,blockMem);
        cout<<"basicInfo:"+path_str+"\t"+outputString<<endl;
        cout<<"---User:Save"<<endl;
    }
}
void User::Read(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    cout<<"User::Read()\n";
    //read permission and basicInfo
    char Path[100];
    string basePath_str = "/User/"+to_string(userID);
    string output="";

    //permission
    string path_str = basePath_str+"/permission";
    strcpy(Path,path_str.c_str());
    readFile(Path,output,superBlock,inodeListMem, blockMem);//read
    cout<<"----User::Read\nreading "+path_str<<": "+output<<endl;
    vector<string>token;
    string temp_str="";
    //spilt
    for(int i=0; i<output.length(); i++){
        string cur_char(1, output[i]);
        if(cur_char != ";"){
            temp_str += output[i];
        }
        else{
        token.push_back(temp_str);
        temp_str = "";
        }
    }
    if (!temp_str.empty()) {
        token.push_back(temp_str);
    }
    OtherFilePrivilege.clear();
    //init
    int pri_num=atoi(token[0].c_str())*2+1;
    for(int i=1;i<pri_num;i+=2){
        char second = Eight_Bit_To_Char(token[i+1]);
        pair<string,char>filePrivilege=make_pair(token[i],second);
        OtherFilePrivilege.insert(filePrivilege);
    }
    //basic info
    token.clear();
    path_str = basePath_str+"/basicInfo";
    output ="";
    strcpy(Path,path_str.c_str());
    readFile(Path,output,superBlock,inodeListMem, blockMem);//read
    cout<<"reading "+path_str<<": "+output<<endl;
    //spilt
    for(int i=0; i<output.length(); i++){
        string cur_char(1, output[i]);
        if(cur_char != ";"){
            temp_str += output[i];
        }
        else{
        token.push_back(temp_str);
        temp_str = "";
        }
    }
    if (!temp_str.empty()) {
        token.push_back(temp_str);
    }
    //init 
    this->userID=atoi(token[0].c_str());
    userName = token[1];
    identity = atoi(token[2].c_str());
    int changeP=atoi(token[3].c_str());
    ChangeOtherPrivilege=changeP==1?true:false;
    cout<<"----User::Read\n";
}
//set privilege
/*
设置ChangP:on=true,ChangP=1;on =false,changP=0
*/
void User::setChangeOtherPrivilege(bool on,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    if (on){
        ChangeOtherPrivilege = true;
    }
    else{
        ChangeOtherPrivilege = false;
    }
    Save(superBlock,inodeListMem,blockMem);
}
/*
append new file privilege.成功append,return 1;path已在其中，return -2
*/
int User::appendFilePrivilege(string path,string privilege,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    //check path in FilePrivilege
    auto it = OtherFilePrivilege.find(path);
    if (it != OtherFilePrivilege.end()) {
        return In;
    }
    //no in and append
    char p = Eight_Bit_To_Char(privilege);
    OtherFilePrivilege.insert({path, p});
    //Save
    Save(superBlock,inodeListMem,blockMem);
    return Success;
}
/*
改变 file privilege的特定位.成功append,return 1;path不在其中，return -1
*/
int User::changeFilePrivilege(string path,int index,bool on,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    //check path in FilePrivilege , index is [0,6]
    auto it = OtherFilePrivilege.find(path);
    if (it == OtherFilePrivilege.end()||(index<0 || index>7)) {
        return Failure;
    }
    // if(index<0 || index>7){return Failure;}
    //change
    char p = OtherFilePrivilege[path];
    string p_str = Char_to_Eight_Bit(p);
    if(on){
        p_str[index]='1';
    }
    else{
        p_str[index]='0';
    }
    p = Eight_Bit_To_Char(p_str);
    OtherFilePrivilege[path]=p;
    //Save
    // markDirty();
    Save(superBlock,inodeListMem,blockMem);
    return Success;
}
/*
改变 file privilege的8位.成功append,return 1;path不在其中，return -1
*/
int User::changeFilePrivilege(string path,string privilege,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    //check path in FilePrivilege , index is [0,6]
    auto it = OtherFilePrivilege.find(path);
    if (it == OtherFilePrivilege.end()) {
        return Failure;
    }

    //change
    OtherFilePrivilege[path] = Eight_Bit_To_Char(privilege);
    //Save
    // markDirty();
    Save(superBlock,inodeListMem,blockMem);
    return Success;
}
/*
移除 file privilege.成功append,return 1;path不在其中，return -1
*/
int User::removeFilePrivilege(string path,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){     
    //check path in FilePrivilege 
    auto it = OtherFilePrivilege.find(path);
    if (it == OtherFilePrivilege.end()) {
        return Failure;
    }
    //change
    OtherFilePrivilege.erase(path);
    //Save
    // markDirty();
    Save(superBlock,inodeListMem,blockMem);
    return Success;
}
//get
/*获得特定文件的8位权限。失败，则返回空字符串*/
string User::getFilePrivilege(string path){
    if(OtherFilePrivilege.find(path)==OtherFilePrivilege.end()){
        return "";
    }
    return Char_to_Eight_Bit(OtherFilePrivilege[path]);
}
/*获得文件的特定位权限,失败，返回-1*/
int User::getFilePrivilege(string path,int index){
    if(OtherFilePrivilege.find(path)==OtherFilePrivilege.end()){
        return Failure;
    }
    string res = Char_to_Eight_Bit(OtherFilePrivilege[path]);
    // cout<<res<<endl;
    if(res[index]=='1'){return 1;}
    return 0;
}
/*get ChangP(0/1）*/
bool User::getChangeOtherPrivilege(){return ChangeOtherPrivilege;}
/*获得permission的长度*/
int User::getOtherPrivilegeNum(){return OtherFilePrivilege.size();}
//获得permission的全部
unordered_map<string,char> User::getOtherFilePrivilege(){return OtherFilePrivilege;}
/*获得userID*/
int User::getUserID(){ return userID; }
/*获得identity*/
int User::getIdentity(){ return identity;}
/*获得userName*/
string User::getName(){return userName;}    

string User::getCurCourse(){return this->curCourseID;} 
void User::setCurCourse(string courseID){this->curCourseID=courseID;}
string User::getCurHWname(){return this->curHWname;} 
void User::setCurHWname(string HWname){this->curHWname=HWname;}
string User::getCurGroup(){return this->curGroup;} 
void User::setCurGroup(string group){this->curGroup=group;}

/*创建学生目录和Course_info目录*/
Student::Student(int userID,string name,int identity,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER])
    :User(userID,name,identity,superBlock,inodeListMem,blockMem)
    {
        char path_char[100];
        string output;
        string basePath_str="/User/"+to_string(userID);
        //create Files:permission and BasicInformation
        string CourseInfo_str = basePath_str+"/CourseInfo";
        strcpy(path_char,CourseInfo_str.c_str());
        CreateF(path_char,output,superBlock,inodeListMem,blockMem);
        //save
        Save(superBlock,inodeListMem,blockMem);
    }

Student::Student(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    Student::init(userID,superBlock,inodeListMem,blockMem);
}

/*保存info*/
void Student::Save(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    cout<<"----Student Save\n";

    //Save
    // string inputLine="";  
    int info_num =0;
    getInfoNum(info_num);
    string inputLine=to_string(info_num)+";";
    for(pair<string,pair<int,int>> courseInfo:info){
        inputLine+=courseInfo.first+";"+to_string(courseInfo.second.first)+";"+to_string(courseInfo.second.second)+";";
    }
    //
    char path_char[100];
    string output="";
    string basePath_str="/User/"+to_string(getUserID());
    string CourseInfo_str = basePath_str+"/CourseInfo";
    strcpy(path_char,CourseInfo_str.c_str());
    WriteF(path_char,output,inputLine,superBlock,inodeListMem,blockMem);
    cout<<"----student Info:"<<CourseInfo_str<<"\t"<<output<<endl;
}
/*读取info*/
void Student::Read(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    cout<<"Student::Read():\n";
    //read info
    char Path[100];
    string basePath_str = "/User/"+to_string(userID);
    string output="";

    string path_str = basePath_str+"/CourseInfo";
    strcpy(Path,path_str.c_str());
    readFile(Path,output,superBlock,inodeListMem, blockMem);//read
    vector<string>token;
    string temp_str="";
    //spilt
    for(int i=0; i<output.length(); i++){
        string cur_char(1, output[i]);
        if(cur_char != ";"){
            temp_str += output[i];
        }
        else{
        token.push_back(temp_str);
        temp_str = "";
        }
    }
    if (!temp_str.empty()) {
        token.push_back(temp_str);
    }
    //init:unordered_map<string,pair<int,int>>info
    info.clear();
    int info_num=atoi(token[0].c_str())*3+1;
    for(int i=1;i<info_num;i+=3){
        // pair<string,string>filePrivilege(token[i],token[i+1]);
        int credit = atoi(token[i+1].c_str());
        int grade = atoi(token[i+2].c_str());
        pair<int,int>creditGrade(credit,grade);
        pair<string,pair<int,int>>courseInfo=make_pair(token[i],creditGrade);
        info.insert(courseInfo);
        // info.emplace(token[i],creditGrade);
    }
}
/*Init from file system*/
// void init(int userID, SuperBlock & superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER]);
void Student::init(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    Read(userID,superBlock,inodeListMem,blockMem);
    User::init(userID,superBlock,inodeListMem,blockMem);
}
//  void Student::initStudent(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
//     Student::Read(this->getUserID(),superBlock,inodeListMem,blockMem);
//  }
/*增加课程。成功，返回1；已在其中，返回-2*/
void Student::appendCourse(string courseID,int credit,int& res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    // cout<<this->getUserID()<<" append course:"<<courseID<<endl;
    //check
    if(info.find(courseID)!=info.end()){
        res=In;
        return ;
    }
    //append
    pair<int,int>creditGrade(credit,-1);
    pair<string, pair<int,int>>course(courseID,creditGrade);
    info.insert(course);
    //Save
    Student::Save(superBlock,inodeListMem,blockMem);
    res = Success;
    cout<<this->getUserID()<<" append course:"<<courseID<<res<<endl;
    return ;
}
/*设置课程成绩。成功，返回1；课程不在其中，返回-1*/
void Student::setGrade(string courseID,int courseGrade,int& res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    //check
    if(info.find(courseID)==info.end()){
        res = Failure;
        return ;
    }
    //set
    int credit=info[courseID].first;
    pair<int,int>creditGrade(credit,courseGrade);
    info[courseID]=creditGrade;
    //Save
    Student::Save(superBlock,inodeListMem,blockMem);
    res = Success;
    return ;
}
/*移除课程。成功，返回1；课程不在其中，返回-1*/
void Student::removeCourse(string courseID,int& res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    //check
    if(info.count(courseID)==0){
        res =Failure;
        return ;
    }
    //remove
    info.erase(courseID);
    //Save
    Student::Save(superBlock,inodeListMem,blockMem);
    res = Success;
    return ;
}
/*查看所有课程*/
void Student::getInfo(unordered_map<string,pair<int,int>>& res){
    res = info;
    return ;
}
    /*查看特定课程成绩。成功返回成绩，失败返回-1*/
void Student::getGrade(string courseID,int& res){
    if(info.find(courseID)==info.end()){
        res = Failure;
        return ;
    }
    res = info[courseID].second;
    return ;
}
void Student::getInfoNum(int& res){
    res = info.size();
    return ;
}

void Student::initStudent(SuperBlock &superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER])
{
    cout<<"Studetne::initStudent:init student\n";
    Student::Read(this->getUserID(),superBlock,inodeListMem,blockMem);
}
/*Init from Database*/
Teacher::Teacher(int userID,string name,int identity,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]):
    User(userID,name,identity,superBlock,inodeListMem,blockMem)
    {
    //Course_info
    //create directory
    char path_char[100];
    string output;
    string basePath_str="/User/"+to_string(userID);
    //create Files:permission and BasicInformation
    string CourseInfo_str = basePath_str+"/CourseInfo";
    strcpy(path_char,CourseInfo_str.c_str());
    CreateF(path_char,output,superBlock,inodeListMem,blockMem);
    //save
    Save(superBlock,inodeListMem,blockMem);
}

Teacher::Teacher(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    Info.reserve(10);
    cout<<"after reserve :"<<Info.size();
    init(userID,superBlock,inodeListMem,blockMem);
}

/*保存Course*/
void Teacher::Save(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    cout<<"teacher Save\n";

    //Save
    // string inputLine="";  
    int info_num =0;
    getInfoNum(info_num);
    string inputLine=to_string(info_num)+";";
    for(string course:Info){
        inputLine+=course+";";
    }
    //
    char path_char[100];
    string output;
    string basePath_str="/User/"+to_string(getUserID());
    string CourseInfo_str = basePath_str+"/CourseInfo";
    strcpy(path_char,CourseInfo_str.c_str());
    WriteF(path_char,output,inputLine,superBlock,inodeListMem,blockMem);
}
/*读取Course*/
void Teacher::Read(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){

    //read info
    char Path[100];
    string basePath_str = "/User/"+to_string(userID);
    string output="";

    string path_str = basePath_str+"/CourseInfo";
    strcpy(Path,path_str.c_str());
    readFile(Path,output,superBlock,inodeListMem, blockMem);//read
    vector<string>token;
    // token.reserve(n);
    
    string temp_str="";
    //spilt
    for(int i=0; i<output.length(); i++){
        string cur_char(1, output[i]);
        if(cur_char != ";"){
            temp_str += output[i];
        }
        else{
        token.push_back(temp_str);
        temp_str = "";
        }
    }
    if (!temp_str.empty()) {
        token.push_back(temp_str);
    }
    cout<<"token is fine.\n";
    //init
    int info_num=atoi(token[0].c_str())+1;
    cout<<info_num<<endl;
    for(int i=1;i<info_num;i+=1){
        Info.push_back(token[i]);
        cout<<i<<"\t";
    }
    cout<<"Info is init:"<<Info.size();
}
/*Init from file system*/
void Teacher::init(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    Read(userID,superBlock,inodeListMem,blockMem);
    User::init(userID,superBlock,inodeListMem,blockMem);
}


/*新增课程，成功返回1；已在其中返回-2*/
void Teacher::appendCourse(string CourseID,int& res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    bool CourseIn=false;
    getCourse(CourseID,CourseIn);
    if(CourseIn){
        res = In;
        return ;
    }
    Info.push_back(CourseID);
    Save(superBlock,inodeListMem,blockMem);
    res =Success;
    return ;
}
    /*删除课程，成功返回1；不在其中返回-1*/
void Teacher::removeCourse(string CourseID,int & res,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    bool CourseIn=false;
    getCourse(CourseID,CourseIn);
    if(!CourseIn){
        res = Failure;
        return ;
    }
    int index=0;
    for(int i=0;i<Info.size();i++){
        if(CourseID == Info[i]){
            index=i;
            break;
        }
    }
    Info.erase(Info.begin()+index);
    Save(superBlock,inodeListMem,blockMem);
    res = Success;
    return ;
}
/*是否选择courseID.已选择，返回true*/
void Teacher::getCourse(string CourseID,bool& res){
    for(int i=0;i<Info.size();i++){
        if(CourseID == Info[i]){
            res =true;
            return;
        }
    }
    res = false;
    return ;
}
void Teacher::getCourses(vector<string>& res){
    // vector<string>temp(Courses);
    // res = temp;
    res = Info;
    return ;
}
void Teacher::getInfoNum(int& res){res= Info.size();}

/*Init from DB*/
Manager::Manager(int userID,string name,int identity,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    User(userID,name,identity,superBlock,inodeListMem,blockMem);
}

/*Init from file system*/
Manager::Manager(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    init(userID,superBlock,inodeListMem,blockMem);
}

void Manager::init(int userID,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    User::init(userID,superBlock,inodeListMem,blockMem);
}
// void Manager::getChangeUserID(int& res){
//     res = changeUerID;
// };
// void Manager::setChangeUserID(int id){
//     changeUerID=id;
// };
// void Manager::setChangeUserIdentity(int& res){
//     res = changeUserIdentity;
// };
// void Manager::getChangeUserIdentity(int identity){
//     changeUserIdentity=identity;
// };
void User::getChangeUserID(int& res){
    res = changeUerID;
};
void User::setChangeUserID(int id){
    changeUerID=id;
};
void User::setChangeUserIdentity(int& res){
    res = changeUserIdentity;
};
void User::getChangeUserIdentity(int identity){
    changeUserIdentity=identity;
};