// #include "CODES.h"
#include <iostream>
// #include  "FileSystem/inode.h"
// #include  "inode.h"
// #include  "PRIVILEDGE.h"
#include <sstream>
#include <unordered_map>
using namespace std;
// SuperBlock superBlock;
// struct InodeList inodeListMem[MAX_INODELIST_NUMBER];//inodelist空间,一共有124个inodelist   //212*124  //有层次结构，不需要用bitmap
// struct FileBlock blockMem[BLOCK_NUMBER];//存block的空间，一共有128个block  //512*1120
//  unordered_map<string,vector<pair<int,int>>>groups;
   string Info[3];//ID,Name,Credit
       
int teacher;
bool open;
bool choose;
bool close;
// vector<int>students;
// vector<string>HWname;//老师布置作业情况
// unordered_map<string,vector<int>>HW_S;//已交作业信息：<HW_name,studentsID>
// unordered_map<string,vector<int>>feedback_S;//已批改信息：<HW_name,studentsID>
 
 //
//  void read(SuperBlock &superBlock, struct InodeList inodeListMem[MAX_INODELIST_NUMBER], struct FileBlock blockMem[BLOCK_NUMBER]);
// void readGPT(const std::string& input, std::unordered_map<std::string, std::vector<std::pair<int, int>>>& result);
// void save( std::string& output,const std::unordered_map<std::string, std::vector<std::pair<int, int>>>& result);
// void Save_config(std::string& output);
// void Read(std::string output);
// void Read_config(const std::string& input) ;
int main(){
    //初始化部分
    // init_inode(superBlock,inodeListMem,blockMem);

    // Group group(superBlock,inodeListMem,blockMem);
    // read(superBlock,inodeListMem,blockMem);
    // std::unordered_map<std::string, std::vector<std::pair<int, int>>> resultMap;
    // resultMap["CS101"] = {{700141, 1}, {700302, 2}, {700318, 1}};
    // resultMap["CS201"] = {{700111, 1}, {700322, 2}, {700313, 1}};
    
    //  std::string output;
    // save(output, resultMap);

    // std::cout << output << std::endl;

    // // std::string input = "CS101:700302,2;700141,1;CS201:700332,2;700111,1;";
    // // std::unordered_map<std::string, std::vector<std::pair<int, int>>> resultMap;
    // resultMap.clear();
    // cout<<"after clearing, resultMap.size():"<<resultMap.size()<<endl;
    // // readGPT(input, resultMap);
    //  readGPT(output, resultMap);

    // // Print the result
    // for (const auto& entry : resultMap) {
    //     std::cout << entry.first << ": ";
    //     for (const auto& pair : entry.second) {
    //         std::cout << pair.first << "," << pair.second << ";";
    //     }
    //     std::cout << std::endl;
    // }
    // Info[0]="CS201";
    // Info[1]="DATA";
    // Info[2]="3";
    // teacher=700302;
    // open=true;
    // choose=false;
    // close = false;
    // students.push_back(700141);
    // HWname.push_back("first");
    // HW_S["first"]=vector<int>();
    // HW_S["first"].push_back(700141);
    // feedback_S["first"]=vector<int>();
    // feedback_S["first"].push_back(700141);
    // string fs ="";
    // Save_config(fs);
    // cout<<"Course->string: "+fs<<endl;
    // Info[0]="";
    // Info[1]="";
    // Info[2]="";
    // teacher=0;
    // open=false;
    // choose=true;
    // close = true;
    // students.clear();
    // HWname.clear();
    // Read_config(fs);
    // // Read(fs);
    // cout<<"string -> Course:\n";
    // cout<<teacher<<" "<<(open?"true ":"false ")<<(choose?"true ":"false ")\
    // <<(close?"true ":"false ")<<students[0]<<" "<<HWname[0]<<endl;
    // cout<<HW_S["first"][0]<<" "<<feedback_S["first"][0]<<endl;
    int grade = 100;
    int credit = 1;
     unordered_map<string,pair<int,int>>info;
    pair<int,int>creditGrade(credit,grade);
    pair<string,pair<int,int>>courseInfo=make_pair("CS101",creditGrade);
    info.insert(courseInfo);
    cout<<"before:"<<to_string(info["CS101"].second)<<endl;
    // info.clear();
    creditGrade.second=60;
    courseInfo=make_pair("CS101",creditGrade);
    info.emplace("CS101",creditGrade);
    // info.insert(courseInfo);
    cout<<"after:"<<to_string(info["CS101"].second)<<endl;
}
/*
void save( std::string& output,const std::unordered_map<std::string, std::vector<std::pair<int, int>>>& result){
        //save unordered_map<string,vector<pair<int,int>>>groups;//groupname:<id,identity> 
        // 将 unordered_map<string,vector<pair<int,int>>> groups 转换为字符串进行保存
        // groups["CS101"]=[<700141,1>,<700302,2>,<700318,1>]
        //groups["CS201"]=[<700111,1>,<700322,2>,<700313,1>]
        //output="CS101:700141,1&700302,2&700318&1;CS201:700111,1&700322,2&700313,1;"


        for (const auto& entry : result) {
        output += entry.first + ":"; // 添加课程ID

        // 遍历每个课程ID对应的用户ID和身份信息
        for (const auto& pair : entry.second) {
            output += std::to_string(pair.first) + "," + std::to_string(pair.second) + "&";
        }

        // 移除最后一个"&"
        if (!entry.second.empty()) {
            output.pop_back();
        }

        output += ";"; // 添加分隔符
    }
        // output=outputString;
        // char path_char[100];
        // string path_str = "/Group";
        // string output;
        // strcpy(path_char,path_str.c_str());
        // WriteF(path_char,output,outputString,superBlock,inodeListMem,blockMem);
}
void readGPT(const std::string& input, std::unordered_map<std::string, std::vector<std::pair<int, int>>>& result) {
     //save unordered_map<string,vector<pair<int,int>>>groups;//groupname:<id,identity> 
        // 将字符串  转换为unordered_map<string,vector<pair<int,int>>> groups
        //input="CS101:700141,1&700302,2&700318&1;CS201:700111,1&700322,2&700313,1;"
        // groups["CS101"]=[<700141,1>,<700302,2>,<700318,1>]
        //groups["CS201"]=[<700111,1>,<700322,2>,<700313,1>]
        
    std::istringstream iss(input);

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
                    userPairValues.first = std::atoi(userPair.c_str());
                } else {
                    userPairValues.second = std::atoi(userPair.c_str());
                }
            }

            userPairs.push_back(userPairValues);
        }

        result[courseID] = userPairs;
    }
}

void Save_config(string& output){
        //Course->string: CS201;DATA;3;700302;1;0;0;1;700141;1;first;1;first,1:700141&;1;first,1:700141&;
        // char Config_path[100];
        // string Config_path_string = "/Course/"+Info[0]+"/Config";
        // strcpy(Config_path, Config_path_string.c_str());
        // string outputString ;
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
        // WriteF(Config_path,outputString,inputLine, superBlock,inodeListMem, blockMem);
        output= inputLine;
    } 
void Read(string output){
        //Read and Spilt
        
        // cout<<output<<endl;
        vector<string>token;
        string temp_str="";
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
        //Init from token
        for(int i=0;i<3;i++){
            Info[i]=token[i];
        }
        //value
        teacher = atoi(token[3].c_str());
        open= atoi(token[4].c_str())==1?true:false;
        choose=atoi(token[5].c_str())==1?true:false;
        close=atoi(token[6].c_str())==1?true:false;
        //vector<int>students&vector<string>hw
        int students_num= atoi(token[7].c_str());
        for(int i =0;i<students_num;i++){
            students.push_back(atoi(token[i+8].c_str())) ;
        }
        int hw_num= atoi(token[8+students_num].c_str());
        for(int i =0;i<hw_num;i++){
            HWname.push_back(token[i+7+students_num]) ;
        }
        // HW_S: unordered_map<string, vector<int>>
        int HW_S_size = atoi(token[8 + students_num + hw_num].c_str());
        int current_index = 9 + students_num + hw_num;
        for (int i = 0; i < HW_S_size; i++) {
            string hw_key = token[current_index++];
            int hw_vector_size = atoi(token[current_index++].c_str());
            vector<int> hw_vector;
            for (int j = 0; j < hw_vector_size; j++) {
                hw_vector.push_back(atoi(token[current_index++].c_str()));
            }
            HW_S[hw_key] = hw_vector;
        }
        // feedback_S: unordered_map<string, vector<int>>
        int feedback_S_size = atoi(token[current_index++].c_str());
        for (int i = 0; i < feedback_S_size; i++) {
            string feedback_key = token[current_index++];
            int feedback_vector_size = atoi(token[current_index++].c_str());
            vector<int> feedback_vector;
            for (int j = 0; j < feedback_vector_size; j++) {
                feedback_vector.push_back(atoi(token[current_index++].c_str()));
            }
            feedback_S[feedback_key] = feedback_vector;
        }
}

void Read_config(const std::string& input) {
    std::istringstream iss(input);
    std::string token;
    size_t pos;

    // Info
    for (int i = 0; i < 3; ++i) {
        std::getline(iss, token, ';');
        Info[i] = token;
    }

    // teacher
    std::getline(iss, token, ';');
    teacher = std::stoi(token);

    // open, choose, close
    for (int i = 0; i < 3; ++i) {
        std::getline(iss, token, ';');
        int value = std::stoi(token);
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
    int num_students = std::stoi(token);
    students.clear();
    for (int i = 0; i < num_students; ++i) {
        std::getline(iss, token, ';');
        students.push_back(std::stoi(token));
    }

    // HWname
    std::getline(iss, token, ';');
    int num_HW = std::stoi(token);
    HWname.clear();
    for (int i = 0; i < num_HW; ++i) {
        std::getline(iss, token, ';');
        HWname.push_back(token);
    }

    // HW_S
    std::getline(iss, token, ';');
    int num_HW_S = std::stoi(token);
    HW_S.clear();
    for (int i = 0; i < num_HW_S; ++i) {
        std::getline(iss, token, ',');
        std::string key = token;

        std::getline(iss, token, ':');
        int len = std::stoi(token);

        std::getline(iss, token, ';');
        std::istringstream userIss(token);

        std::vector<int> values;
        for (int j = 0; j < len; ++j) {
            std::getline(userIss, token, '&');
            values.push_back(std::stoi(token));
        }

        HW_S[key] = values;
    }

    // feedback_S
    std::getline(iss, token, ';');
    int num_feedback_S = std::stoi(token);
    feedback_S.clear();
    for (int i = 0; i < num_feedback_S; ++i) {
        std::getline(iss, token, ',');
        std::string key = token;

        std::getline(iss, token, ':');
        int len = std::stoi(token);

        std::getline(iss, token, ';');
        std::istringstream userIss(token);

        std::vector<int> values;
        for (int j = 0; j < len; ++j) {
            std::getline(userIss, token, '&');
            values.push_back(std::stoi(token));
        }

        feedback_S[key] = values;
    }
}
*/