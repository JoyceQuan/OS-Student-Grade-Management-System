//operating code
#ifndef CODE_H
#define CODE_H
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
#define LOGIN_PERSON_CODE2 '4'
#define LOGOUT_CONFIRMATION '5'
#define LOGIN_ROOT_PASSWORD_WRONG '6'
#define LOGIN_ROOT_PASSWORD_RIGHT '7'
#define LOGIN_PERMIT '8'    
#define ROOT_OP '9'
#define ROOT_OP_FEEDBACK 'a'
#define STUDENT_LOGIN 'b'
#define POS_UPDATE_ASK 'c'
#define POS_UPDATE_FEEDBACK 'd'
#define ASK_FOR_OPTION_SHOW 'e'
// split with ,  the first bit represents the call of input function in users program
#define ASK_FOR_OPTION_SHOW_FEEDBACK 'f'



//terminal ID
#define TERMINAL_ID "10000"


void sleep(unsigned int time);

bool isneed(char code, string target,string buf);

bool isneed_accurate(char code, string target, string origin,string buf);

string get_region(string buf);

string decode(string buf);

string RE_TO_SEND(string buf);

pair<string,pair<string,string>> decode_options(string buf);

string FORMAT(string target, int len);


#endif