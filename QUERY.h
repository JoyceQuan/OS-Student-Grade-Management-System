#ifndef QUERY
#define QUERY
#include<iostream>
#include<vector>
#include<mysql/mysql.h>
using namespace std;
void basic_search(MYSQL* mysql, MYSQL_RES** result, string target);
void query_all_courses(MYSQL* mysql,string& output);
void query_basic_information(vector<string*>& basic_information, string& output);
string FORMAT2(string target, int len);
#endif