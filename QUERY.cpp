#include"QUERY.h"
#include<iostream>
using namespace std;
string FORMAT2(string target, int len){
    string output(len,' ');
    for(int i = 0;i<len;i++){
        if(i<target.size()){
            output[i] = target[i];
        }
    }
    return output;
}
void basic_search(MYSQL* mysql,MYSQL_RES ** result, string target){
    mysql_query(mysql,target.c_str());
    *result =  mysql_store_result(mysql);
}
void query_all_courses(MYSQL* mysql, string& output){
    output+=FORMAT2("course ID",12)+FORMAT2("course name",20)+FORMAT2("credit",10)+'\n';
    string sql = "select * from course";
    MYSQL_RES * result = NULL;
    basic_search(mysql, &result, sql);
    MYSQL_ROW row;
    while((row = mysql_fetch_row(result))){
        output+=FORMAT2(row[0],12)+FORMAT2(row[1],20)+FORMAT2(row[2],10)+'\n';
    }
}
void query_basic_information(vector<string*>& basic_information, string& output){
    string identity_to_name[4] = {
    "",
    "student",
    "teacher",
    "administrator"
    };
    output+=FORMAT2("IP",10)+FORMAT2("ID",15)+FORMAT2("name",15)+FORMAT2("identity",15)+'\n';
    int id_for_identity_to_name = atoi(basic_information[3]->c_str());
    output+=FORMAT2(*basic_information[0],10)+FORMAT2(*basic_information[1],15)+FORMAT2(*basic_information[2],15)+FORMAT2(identity_to_name[id_for_identity_to_name],15)+'\n';
}


void delete_user(MYSQL* mysql,string user_ID){
    MYSQL_RES * result = NULL;
    string sql = "delete from users where id = '" + user_ID + "'";
    basic_search(mysql, &result, sql);
}


void change_password(MYSQL* mysql, string user_ID, string old_password){
    MYSQL_RES * result = NULL;
    string sql = "update users set Password = '" + old_password +"' where ID = '" + user_ID + "'";
    basic_search(mysql, &result, sql);
}
// void query_courseID(MYSQL* mysql, string& output,string courseID){

// }
