#ifndef QUERY
#define QUERY
#include<iostream>
#include<vector>
#include<mysql/mysql.h>
using namespace std;
void basic_search(MYSQL* mysql, MYSQL_RES** result, string target);
void query_all_courses(MYSQL* mysql,string& output);
void query_basic_information(vector<string*>& basic_information, string& output);

//mysql 对象已经初始化完毕了，在T.cpp里面，备份恢复的时候直接执行这两个函数就可以了，mysql对象记得传入 &mysql

void delete_user(MYSQL* mysql,string user_ID);

void change_password(MYSQL* mysql, string user_ID, string old_password);


string FORMAT2(string target, int len);
#endif