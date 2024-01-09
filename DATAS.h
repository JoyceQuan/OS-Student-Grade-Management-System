#ifndef DATA_H
#define DATA_H
#include<iostream>
#include<sstream>
#include<vector>
#include<unordered_map>
#include "USER.h"

//user.h里加了一个函数int getIdentity() const；； int getIdentity() const {return identity;}
using namespace std;

string output_for_help = "help:\nyour system commands should begin with OSFS\nhere are some specifications:\n-------------------------------------\nOSFS <type> <path>\n-------------------------------------\ncreate directory:   OSFS -cd <path>\ndelete directory:   OSFS -dd <path>\nlist the files:     OSFS -ls <path>\ncreate file:        OSFS -cf <path>\ndelete file:        OSFS -df <path>\nread file:          OSFS -rf <path>\nwrite file:         OSFS -wf <path>\nchange file access permissions:\nrelated user needs to have been created!\nyour commands have several choices below:\naccurate operation is required:\n-------------------------------------\nOSFS -gr <target type> <flag bit> <path> <user ID>\n-------------------------------------\ncreate directory: OSFS -gr -cd -y/-n <path> <user ID>\ndelete directory: OSFS -gr -dd -y/-n <path> <user ID>\nlist the files:   OSFS -gr -ls -y/-n <path> <user ID>\ncreate file:      OSFS -gr -cf -y/-n <path> <user ID>\ndelete file:      OSFS -gr -df -y/-n <path> <user ID>\nread file:        OSFS -gr -rf -y/-n <path> <user ID>\nwrite file:       OSFS -gr -wf -y/-n <path> <user ID>\nAdministrator can allow user to operate objects:\n-------------------------------------\nOSFS -cr -gr <flag bit> / <user ID>\n-------------------------------------\nwish you an enjoyable use!\n";

//pair<{command type,director,secondary command type}, {path,user ID}> deal_command(identification check,command)
//eight types are shown below:
//index in array target_type_temp represents the identity
//
//
//
//
//
//
bool CheckPriviledge(User& currentUser, const std::string& path, int index,int FT, SuperBlock& superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER]){
    int uID = currentUser.getUserID();
    int identity = currentUser.getIdentity(); //获取用户身份
    //刷新身份信息
    cout<<"user Read():\n";
    currentUser.Read(uID,superBlock, inodeListMem, blockMem);
    cout<<(currentUser.getChangeOtherPrivilege()?"YEs":"FALSE")<<endl;
    cout<<identity<<endl;
    //优先判断 管理员就直接成功
    if(identity==3){
        return true;
    }
    else{
        cout<<"step1 failed"<<endl;
    }

    //获取用户在该路径的指定位置权限
    int priviledge = currentUser.getFilePrivilege(path,index);
    if(priviledge==1){
        return true;
    }
    //0或-1 不能或没有
    else{
        cout<<"step2 failed"<<endl;
        //检查该位置i-node权限是否可用
        int permission = check_Privilige_by_Path(index, identity-1, const_cast<char*>(path.c_str()), FT, superBlock, inodeListMem, blockMem);
        if(permission==1){
            return true;
        }
        else{
            cout<<"step3 failed"<<endl;
        }
    }
    return false;
}

void handleCreateDirectory(bool& flag, User& currentUser, const std::string& path, const std::vector<std::string>& P, SuperBlock& superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER], string& output) {
    if(CheckPriviledge(currentUser,path,0,0,superBlock, inodeListMem, blockMem)){
        flag = true;
        std::string outputString;
        CreateD(const_cast<char*>(path.c_str()), outputString, P, superBlock, inodeListMem, blockMem);
        output += outputString + '\n';
        // return flag;
        return;
    }
    output += "errors occured, please check your priviledge!\n";
    flag = false;
}
// 处理删除目录的函数
void handleDeleteDirectory(bool& flag, User& currentUser, const std::string& path, SuperBlock& superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER], string& output) {
    if(CheckPriviledge(currentUser,path,1,0,superBlock, inodeListMem, blockMem)){
        flag = true;
        std::string outputString;
        DeleteD(const_cast<char*>(path.c_str()), outputString, superBlock, inodeListMem, blockMem);
        output += outputString + '\n';
        // return flag;
        return;
    }
    output += "errors occured, please check your priviledge!\n";
    flag = false;
}

// 处理列出文件的函数
void handleListFiles(bool& flag, User& currentUser, const std::string& path, SuperBlock& superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER], string& output) {
    if(CheckPriviledge(currentUser,path,2,0,superBlock, inodeListMem, blockMem)){
        flag = true;
        std::string outputString;
        listFiles(const_cast<char*>(path.c_str()), outputString, superBlock, inodeListMem, blockMem);
        output += outputString + '\n';
        // return flag;
        return;
    }
    output += "errors occured, please check your priviledge!\n";
    flag = false;
}

// 处理创建文件的函数
void handleCreateFile(bool& flag, User& currentUser, const std::string& path, SuperBlock& superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER], string& output) {
    if(CheckPriviledge(currentUser,path,3,1,superBlock, inodeListMem, blockMem)){
        flag = true;
        std::string outputString;
        CreateF(const_cast<char*>(path.c_str()), outputString, superBlock, inodeListMem, blockMem);
        output += outputString + '\n';
        // return flag;
        return;
    }
    output += "errors occured, please check your priviledge!\n";
    flag = false;
    //CreateF
}

// 处理删除文件的函数
void handleDeleteFile(bool& flag, User& currentUser, const std::string& path, SuperBlock& superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER], string& output) {
    if(CheckPriviledge(currentUser,path,4,1,superBlock, inodeListMem, blockMem)){
        flag = true;
        std::string outputString;
        DeleteF(const_cast<char*>(path.c_str()), outputString, superBlock, inodeListMem, blockMem);
        output += outputString + '\n';
        // return flag;
        return;
    }
    output += "errors occured, please check your priviledge!\n";
    flag = false;
    // DeleteF
}

// 处理读取文件的函数
void handleReadFile(bool& flag, User& currentUser, const std::string& path, SuperBlock& superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER], string& output) {
    if(CheckPriviledge(currentUser,path,5,1,superBlock, inodeListMem, blockMem)){
        flag = true;
        std::string outputString;
        readFile(const_cast<char*>(path.c_str()), outputString, superBlock, inodeListMem, blockMem);
        output += outputString + '\n';
        // return flag;
        return;
    }
    output += "errors occured, please check your priviledge!\n";
    flag = false;
    //readFile
}

// 处理写入文件的函数
void handleWriteFile(bool& flag, User& currentUser, const std::string& path, const std::string& inputLine, SuperBlock& superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER], string& output, int& is_extra_input) {
    if(CheckPriviledge(currentUser,path,5,1,superBlock, inodeListMem, blockMem)&&is_extra_input==0){
        flag = true;
        is_extra_input = 1;
        output += "please input for your file\n";
        // return flag;
        return;
    }
    else if(is_extra_input==1){
        flag = true;
        std::string outputString;
        cout<<"handleWriteFile\n";
        WriteF(const_cast<char*>(path.c_str()), outputString,inputLine, superBlock, inodeListMem, blockMem);
        output += outputString + '\n';
        is_extra_input = 0;
        // return flag;
        return;
    }
    output += "errors occured, please check your priviledge!\n";
    flag = false;
    //WriteF
}

//修改文件权限
void change_user_Privilige_byPath(bool& flag, User& currentUser, string& new_id, char path[100], vector<int>data, SuperBlock& superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER], string& output) {
    currentUser.Read(currentUser.getUserID(),superBlock, inodeListMem, blockMem);
    if(currentUser.getChangeOtherPrivilege()){
        User targetUser;
        targetUser.Read(atoi(new_id.c_str()),superBlock, inodeListMem, blockMem);      
        if (data[1] == 1) {
        // 将权限位设置为 1
        //若额外表中没有这个路径，就加入
            if(targetUser.getOtherFilePrivilege().find(path) == targetUser.getOtherFilePrivilege().end()){

                targetUser.appendFilePrivilege(path,"00000000", superBlock, inodeListMem, blockMem);
                targetUser.changeFilePrivilege(path,data[2],true,superBlock, inodeListMem, blockMem);
            }
        //若有这个路径就修改
            else{
                targetUser.changeFilePrivilege(path,data[2],true,superBlock, inodeListMem, blockMem);
            }
        }
        else if (data[1] == 0) {//
            // 将权限位设置为 0
            //若额外表中没有这个路径，不作为
            if(targetUser.getOtherFilePrivilege().find(path) == targetUser.getOtherFilePrivilege().end()){
                
            }
        //若有这个路径就修改，若全为0就删除
            else{
                targetUser.changeFilePrivilege(path,data[2],false,superBlock, inodeListMem, blockMem);
                //检查是否全0
                bool flag_bit = true;
                for(int i = 0; i<8;i++){
                    if(targetUser.getFilePrivilege(path,i)==1){
                        flag_bit = false;
                        break;
                    }
                }
                if(flag_bit){
                    targetUser.removeFilePrivilege(path,superBlock, inodeListMem, blockMem);
                }
            }
        }
        flag = true;
        output+="operation success!\n";
    }
    else{
        output += "errors occured, please check your priviledge!\n";
        flag = false;
    }
}

//修改更改文件权限成员的
void change_user_ChangeP_byPath(bool& flag, User& currentUser, string& new_id,vector<int>data, SuperBlock& superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER], string& output){
    if(currentUser.getIdentity()==3){

        User targetUser;
        targetUser.Read(atoi(new_id.c_str()),superBlock, inodeListMem, blockMem);

        targetUser.setChangeOtherPrivilege(data[1]==1,superBlock, inodeListMem, blockMem);
        
        flag = true;
        output+="operation success!\n";
    }
    else{
        output += "errors occured, please check your priviledge!\n";
        flag = false;
    }
}
pair<vector<int>,vector<string>> deal_command(bool& flag, std::string command, User& currentUser, SuperBlock& superBlock, InodeList inodeListMem[MAX_INODELIST_NUMBER], FileBlock blockMem[BLOCK_NUMBER], string& output1, int& is_extra_input, string extra){
    vector<int>data = {0,0,0};
    vector<string>output = {"",""};
    string target_type_temp[9] = {"-cd","-dd","-ls","-cf","-df","-rf","-wf","-gr","-cr"};
        unordered_map<string,int>target_type;
        for(int i = 0;i<9;i++){
            target_type.emplace(target_type_temp[i],i);
        }

        //depart the command
        stringstream ss(command);
        vector<string>result;
        string str;
        while(getline(ss, str, ' ')){
            result.push_back(str);
        }


        //if the size is wrong, return
        if(result.size()!=3 && result.size()!=6){
            flag = false;
            return make_pair(data,output);
        }

        //check the first part
        if(result[0]!="OSFS"){
            flag = false;
            return make_pair(data,output);
        }

        //check the second part
        if(target_type.find(result[1])==target_type.end()){
            flag = false;
            return make_pair(data,output);
        }

        //means a simple style
        if(result.size()==3){
            //make sure different from grant priviledges
            if(result[1]=="-gr" || result[1]=="-cr"){
                flag = false;
                return make_pair(data,output);
            }
            flag = true;
            data[0] = target_type[result[1]];
            output[0] = result[2];
        }
        //check 6 together
        else if(result.size()==6){
            if(result[1]!="-gr" && result[1]!="-cr"){
                flag = false;
                return make_pair(data,output);
            }
            //check the second parameter
            if(target_type.find(result[2])==target_type.end()){
                flag = false;
                return make_pair(data,output);
            }
            //check the third parameter
            if(result[3]!="-y"&&result[3]!="-n"){
                flag = false;
                return make_pair(data,output);
            }
            //devide here
            if(result[1]=="-cr"&&result[2]!="-gr"){
                flag = false;
                return make_pair(data,output);
            }
            if(result[1]=="-gr"){
                if(result[2]=="-gr"||result[2]=="-cr"){
                    flag = false;
                    return make_pair(data,output);
                }
            }
            data[0] = target_type[result[1]];
            data[1] = (result[3]=="-y")?1:0;
            data[2] = target_type[result[2]];
            output[0] = result[4];
            output[1] = result[5];
        }
    if(is_extra_input==0){
        vector<string>P = {"00000000","00000000","11111110"};
        switch (data[0]) {
        case 0:  // 创建目录
            // vector<string>P = {"00000000","00000000","11111110"};
            handleCreateDirectory(flag, currentUser, output[0], P, superBlock, inodeListMem, blockMem,output1);
            //添加权限：
            currentUser.appendFilePrivilege(output[0],"11111110", superBlock, inodeListMem, blockMem);
            break;
        case 1:  // 删除目录
            handleDeleteDirectory(flag, currentUser, output[0], superBlock, inodeListMem, blockMem,output1);

            //删除权限：

            currentUser.removeFilePrivilege(output[0], superBlock, inodeListMem, blockMem);
            break;
        case 2:  // 列出文件
            handleListFiles(flag, currentUser, output[0], superBlock, inodeListMem, blockMem,output1);
            break;
        case 3:  // 创建文件
            handleCreateFile(flag, currentUser, output[0], superBlock, inodeListMem, blockMem,output1);
            break;
        case 4:  // 删除文件
            handleDeleteFile(flag, currentUser, output[0], superBlock, inodeListMem, blockMem,output1);
            break;
        case 5:  // 读取文件
            handleReadFile(flag, currentUser, output[0], superBlock, inodeListMem, blockMem,output1);
            break;
        case 6:  // 写入文件
            handleWriteFile(flag, currentUser, output[0], extra,superBlock, inodeListMem, blockMem,output1,is_extra_input);
            break;
        case 7:  // 更改文件权限
            // 在这里调用更改权限的函数
            //                          bool& flag, User& currentUser, string& new_id, char path[100], int data[3]
            char path[100];
            strcpy(path,output[0].c_str());
            change_user_Privilige_byPath(flag, currentUser, output[1],path , data,superBlock, inodeListMem, blockMem,output1);
            break;
        case 8:  // 更改修改文件权限的权限
            change_user_ChangeP_byPath(flag, currentUser,output[1],data,superBlock, inodeListMem, blockMem,output1);
            break;
        }
    }
    else{
        handleWriteFile(flag, currentUser, output[0], extra,superBlock, inodeListMem, blockMem,output1,is_extra_input);
    }
    return make_pair(data,output);
}


//get
/*获得特定文件的8位权限。失败，则返回空字符串*/
// string getFilePrivilege(string path, User& currentUser) {
//     if (currentUser.OtherFilePrivilege.find(path) == currentUser.OtherFilePrivilege.end()) {
//         return "";
//     }
//     return Char_to_Eight_Bit(currentUser.OtherFilePrivilege[path]);

// }
// /*获得文件的特定位权限,失败，返回-1*/
// int getFilePrivilege(string path, int index, User& currentUser) {
//     if (currentUser.OtherFilePrivilege.find(path) == currentUser.OtherFilePrivilege.end()) {
//         return Failure;
//     }
//     string res = Char_to_Eight_Bit(currentUser.OtherFilePrivilege[path]);
//     // cout<<res<<endl;
//     if (res[index] == '1') { return 1; }
//     return 0;
// }




#endif


