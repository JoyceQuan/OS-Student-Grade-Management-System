#include"CODES.h"
using namespace std;


void sleep(unsigned int time){
    struct timeval tv;
    tv.tv_sec = time;
    tv.tv_usec = 0;
    select(0,NULL,NULL,NULL,&tv);

}

bool isneed(char code, string target,string buf){
    if(buf[0]!=code){
        return false;
    }
    if(target!=buf.substr(1,5)){//接收端是否为target
        return false;
    }
    return true;
}

bool isneed_accurate(char code, string target, string origin,string buf){
    if(buf[0]!=code){
        return false;
    }
    if(target!=buf.substr(1,5)){
        return false;
    }
    if(origin!=buf.substr(6,5)){
        return false;
    }
    return true;
}
string get_region(string buf){
    return buf.substr(6,5);
}
string decode(string buf){
    return buf.substr(11);
}
string RE_TO_SEND(string buf){
    return buf[0] + buf.substr(6,5) + buf.substr(1,5) + buf.substr(11);
}

pair<string,pair<string,string>> decode_options(string buf){
    pair<string,pair<string,string>>output;
    output.first = buf.substr(0,2);
    buf = buf.substr(2);
    int index = 0;
    for(int i = 0;i<buf.size();i++){
        if(buf[i]=='|'){
            index=i;
            break;
        }
    }
    //cout<<"debug---"<<endl;
    //cout<<buf.substr(0,index)<<endl;
    //cout<<buf.substr(index+1)<<endl;
    //cout<<"--------"<<endl;
    output.second = make_pair(buf.substr(0,index),buf.substr(index+1));
    return output;
}
string FORMAT(string target, int len){
    string output(len,' ');
    for(int i = 0;i<len;i++){
        if(i<target.size()){
            output[i] = target[i];
        }
    }
    return output;
}

void getAllChooseCourses(MYSQL mysql,string& output,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    string sql = "select * from course;";
    MYSQL_RES *  result = NULL;
    MYSQL_ROW row;
    basic_search(&mysql, &result, sql);
    vector<Course>courses;
    while ((row = mysql_fetch_row(result))) {
        // Iterate through columns and print values
        string info[3];
        for (unsigned int i = 0; i < mysql_num_fields(result); i++) {
            // std::cout << row[i] << "\t";
            info[i]=row[i];
        }
        Course tempCourse(info[0],superBlock,inodeListMem,blockMem);
        courses.push_back(tempCourse);
        // std::cout << std::endl;
    }
    if(courses.size()==0){output+="no courses can be chosen";}
    for(int i=0;i<courses.size();i++){
        if(courses[i].getChoose()){
            output+=courses[i].getInfo()+"\n";
        }
    }
    return;
}

vector<string> getAllCourse(MYSQL mysql,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    string sql = "select * from course;";
    MYSQL_RES *  result = NULL;
    MYSQL_ROW row;
    basic_search(&mysql, &result, sql);
    vector<string>courses;
    while ((row = mysql_fetch_row(result))) {
        // Iterate through columns and print values
        string info[3];
        for (unsigned int i = 0; i < mysql_num_fields(result); i++) {
            // std::cout << row[i] << "\t";
            info[i]=row[i];
        }
        // Course tempCourse(info[0],superBlock,inodeListMem,blockMem);
        courses.push_back(info[0]);
        // std::cout << std::endl;
    }
    return courses;
};
vector<pair<int,int>> getAllUser(MYSQL mysql){
    string sql = "select * from users;";
    MYSQL_RES *  result = NULL;
    MYSQL_ROW row;
    basic_search(&mysql, &result, sql);
    vector<pair<int,int>>users;
    while ((row = mysql_fetch_row(result))) {
        // Iterate through columns and print values
        string info[4];//id,name,password,identity
        for (unsigned int i = 0; i < mysql_num_fields(result); i++) {
            // std::cout << row[i] << "\t";
            info[i]=row[i];
        }
        // Course tempCourse(info,superBlock,inodeListMem,blockMem);
        pair<int,int>user(atoi(info[0].c_str()),atoi(info[3].c_str()));
        users.push_back(user);
        // std::cout << std::endl;
    }
    return users;
};

vector<string> spiltInput(string intput,string spiltCode){
    vector<string>token;
    string temp_str="";
        for(int i=0; i<intput.length(); i++){
        string cur_char(1, intput[i]);
        if(cur_char != spiltCode){
            temp_str += intput[i];
        }
        else{
        token.push_back(temp_str);
        temp_str = "";
        }
    }
    if (!temp_str.empty()) {
        token.push_back(temp_str);
    }
    return token;
};

User* createUser(int id,int identity,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    User* user=nullptr;
    if(identity==1){
        user=new Student(id,superBlock,inodeListMem,blockMem);
    }
    else if(identity==2){
        user=new Teacher(id,superBlock,inodeListMem,blockMem);
    }
    else if(identity==3){
        user=new Manager(id,superBlock,inodeListMem,blockMem);
    }
    return user;
};