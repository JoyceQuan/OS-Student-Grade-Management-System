//operating code
#include<sys/types.h>
#include<sys/socket.h>   
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<iostream>
#include<thread>
#include<mutex>
#include<vector>
#include<string>
#include<unordered_set>
using namespace std;
#define LOGIN_CODE '0'
#define LOGIN_FEEDBACK_CODE_FAIL '1'
#define LOGIN_FEEDBACK_CODE_SUCCESS '2'
#define LOGIN_PERSON_CODE '3'
#define LOGOUT_CONFIRMATION '4'
#define LOGIN_ROOT_PASSWORD_WRONG '5'
#define LOGIN_ROOT_PASSWORD_RIGHT '6'
#define LOGIN_PERMIT '7'    
#define ROOT_OP '8'
#define ROOT_OP_FEEDBACK '9'
#define STUDENT_LOGIN 'a'


//terminal ID
#define TERMINAL_ID "10000"

// 自定义 sleep 函数，用于在指定时间内暂停程序执行,因为Linux没有Windows.h
void sleep(unsigned int time){
    struct timeval tv;
    tv.tv_sec = time;
    tv.tv_usec = 0;
    select(0,NULL,NULL,NULL,&tv);

}

// 检查收到的包是否符合预期的格式
bool isneed(char code, string target,string buf){
    if(buf[0]!=code){
        return false;
    }
    if(target!=buf.substr(1,5)){
        return false;
    }
    return true;
}
// 从包中提取特定位置的子串
string get_region(string buf){
    return buf.substr(6,5);
}
// 从包中提取特定位置的子串
string decode(string buf){
    return buf.substr(11);
}
// 重新构建一个包，交换位置信息
string RE_TO_SEND(string buf){
    return buf[0] + buf.substr(6,5) + buf.substr(1,5) + buf.substr(11);
}