#ifndef DATA_H
#define DATA_H
#include<iostream>
#include<sstream>
#include<vector>
#include<unordered_map>
using namespace std;

string output_for_help = "help:\nyour system commands should begin with OSFS\nhere are some specifications:\n-------------------------------------\nOSFS <type> <path>\n-------------------------------------\ncreate directory:   OSFS -cd <path>\ndelete directory:   OSFS -dd <path>\nlist the files:     OSFS -ls <path>\ncreate file:        OSFS -cf <path>\ndelete file:        OSFS -df <path>\nread file:          OSFS -rf <path>\nwrite file:         OSFS -wf <path>\nchange file access permissions:\nrelated user needs to have been created!\nyour commands have several choices below:\n1.accurate operation is required:\n-------------------------------------\nOSFS -gr <target type> <flag bit> <path> <user ID>\n-------------------------------------\ndelete directory: OSFS -gr -dd -y/-n <path> <user ID>\nlist the files:   OSFS -gr -ls -y/-n <path> <user ID>\nread file:        OSFS -gr -rf -y/-n <path> <user ID>\nwrite file:       OSFS -gr -wf -y/-n <path> <user ID>\n2.accurate operation is not required:\n-------------------------------------\nOSFS -gr <target type> <flag bit> <user ID>\n-------------------------------------\ncreate directory:   OSFS -gr -cd -y/-n <user ID>\ncreate file:        OSFS -gr -cf -y/-n <user ID>\ndelete file:        OSFS -gr -df -y/-n <user ID>\nchange permissions: OSFS -gr -gr -y/-n <user ID>\n";

//pair<{command type,director,secondary command type}, {path,user ID}> deal_command(identification check,command)
//eight types are shown below:
//index in array target_type_temp represents the identity
//
//
//
//
//
//
pair<vector<int>,vector<string>> deal_command(bool& flag, string command){
    //init data
    vector<int>data = {0,0,0};
    vector<string>output = {"",""};
    string target_type_temp[8] = {"-cd","-dd","-ls","-cf","-df","-rf","-wf","-gr"};
    unordered_map<string,int>target_type;
    for(int i = 0;i<8;i++){
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
    if(result.size()!=3&&result.size()!=5&&result.size()!=6){
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
        if(result[1]=="-gr"){
            flag = false;
            return make_pair(data,output);
        }
        flag = true;
        data[0] = target_type[result[1]];
        output[0] = result[2];
        return make_pair(data,output);
    }
    //check 5 and 6 together
    else if(result.size()==5||result.size()==6){
        if(result[1]!="-gr"){
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
        if(result.size()==5){
            if(result[2]!="-cd"&&result[2]!="-cf"&&result[2]!="-df"&&result[2]!="-gr"){
                flag = false;
                return make_pair(data,output);
            }
            flag = true;
            data[0] = target_type[result[1]];
            data[1] = result[3]=="-y"?1:-1;
            data[2] = target_type[result[2]];
            output[1] = result[4];
            return make_pair(data,output);
        }
        else{
            if(result[2]!="-dd"&&result[2]!="-ls"&&result[2]!="-rf"&&result[2]!="-wf"){
                flag = false;
                return make_pair(data,output);
            }
            flag = true;
            data[0] = target_type[result[1]];
            data[1] = result[3]=="-y"?1:-1;
            data[2] = target_type[result[2]];
            output[0] = result[4];
            output[1] = result[5];
            return make_pair(data,output);
        }
    }
    return make_pair(data,output);
}

#endif


