//priviledge list
#include<fstream>
#include<iostream>
#include<sstream>
#include<algorithm>
#include<vector>
#define PRI_LEN 40
using namespace std;

class priviledge{
public:
    int flagset[PRI_LEN];  // 存储特权标志的数组
    int identity;  // 用户身份标识
    vector<string> true_options;  // 存储可用的特权选项
    pair<char, string> pos = make_pair('1', "");  // 当前位置
    vector<char> index_to_option;

    static vector<string> priviledges;  // 预定义的特权列表
    static vector<vector<int>> default_flagset;  // 默认特权标志数组
    priviledge();  // 默认构造函数
    priviledge(int);  // 带身份参数的构造函数
    priviledge(bool, string);  // 根据文件初始化的构造函数
    string show_options(string);  // 从特权字符串中提取选项
    void select();  // 选择可用的特权选项
    void print_selections();  // 打印可用的特权选项
    void setdefault();  // 设置为默认特权标志
    bool ismeet(string, string);  // 检查两个字符串是否匹配
    void set_pos(bool, char);  // 设置当前位置
    void set(string);  // 设置特权身份
    void set2(string);  // 从文件初始化特权标志
};

vector<string> priviledge::priviledges = {
    "11:Choose courses","211:Optional courses","212:Selection",
    "12:Selected courses","221:Choose course","3211:Homework","3212:Hand in homework","3213:Ratings","222:Credits",
    "13:Charge your course","231:Assign homework","232:Received homework","233:Homework feedback",
    "14:Rights management","241:Information encyclopedia","242:Right assignment","3421:Single user assignment","3422:Group assignment",
    "15:Charge groups","251:Free grouping","252:Assignment according to courses","3521:Unassigned courses","3522:Assign according to course ID",
    "16:Charge courses","261:Offer courses","3611:All courses' information","3612:Offer courses according to course id","262:End courses","3621:Unfinished courses","3622:End courses according to course ID",
    "17:File snapshot","271:Excute file snapshot","272:Recover file",
    "18:Information","281:Group information","282:Personal information","283:Change password",
    "19:quit","291:back","3911:back"
};
vector<vector<int>> priviledge::default_flagset = {
    {1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1}
 };
void priviledge::setdefault(){
    for(int i = 0;i<default_flagset[identity-1].size();i++){
        flagset[i] = default_flagset[identity-1][i];
    }
}
priviledge::priviledge(){
    identity = 1;
    setdefault();
}
priviledge::priviledge(int NUM){
    identity = NUM;
    setdefault();
}
priviledge::priviledge(bool flag,string target){
    if(flag==false){
        identity = atoi(target.c_str());
        setdefault();
    }
    else{
        ifstream target_file;
        target_file.open(target,ios::in);
        if (!target_file.is_open())
        {
            cout << "fail to open file from:" <<target<< endl;
            return;
        }
        vector<string>get_file;
        string buf;
	    while (getline(target_file,buf))
	    {
		    get_file.push_back(buf);
	    }
        identity = atoi(get_file[0].c_str());
        for(int i = 1;i<get_file.size();i++){
            flagset[i-1] = atoi(get_file[i].c_str());
        }
    }
}
// 从特权字符串中提取文字选项
string priviledge::show_options(string temp){
    int index = 0;
    for(int i = 0;i<temp.size();i++){
        if(temp[i]==':'){
            index = i+1;
            break;
        }
    }
    return temp.substr(index);
}
// 选择可用的特权选项
void priviledge::select(){
    true_options.clear();
    index_to_option.clear();
    for(int i = 0;i<PRI_LEN;i++){
        if(priviledges[i][0]==pos.first && flagset[i]==1 && ismeet(pos.second,priviledges[i].substr(1))){
            true_options.push_back(show_options(priviledges[i]));
            // index_to_option.push_back(priviledges[priviledges[0]-'0']);
        }
    }
}
//ab字符串是否相同// a 是较短的字符串
bool priviledge::ismeet(string a,string b){
    bool output = true;
    for(int i = 0;i<a.size();i++){
        if(a[i]!=b[i]){
            return false;
        }
    }
    return output;
}//a is shorter

void priviledge::print_selections(){
    for(int i = 0;i<true_options.size();i++){
        cout<<i+1<<" : "<<true_options[i]<<endl;
    }
}

void priviledge::set_pos(bool flag,char target){
    if(flag){
        pos.first = char(int(pos.first)+1);
        pos.second+=target;
    }
    else{
        pos.first = char(int(pos.first)-1);
        pos.second = pos.second.substr(0,pos.second.size()-1);
    }
}
// 权限设置 target是数
void priviledge::set(string target){
    identity = atoi(target.c_str());
    setdefault();
}
//权限设置 target是路径
void priviledge::set2(string target){
    ifstream target_file;
        target_file.open(target,ios::in);
        if (!target_file.is_open())
        {
            cout << "fail to open file from:" <<target<< endl;
            return;
        }
        vector<string>get_file;
        string buf;
	    while (getline(target_file,buf))
	    {
		    get_file.push_back(buf);
	    }
        identity = atoi(get_file[0].c_str());
        for(int i = 1;i<get_file.size();i++){
            flagset[i-1] = atoi(get_file[i].c_str());
        }
}