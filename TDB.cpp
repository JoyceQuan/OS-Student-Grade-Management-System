#include<iostream>
#include<iomanip>
#include<cstring>
#include<vector>
#include<mysql/mysql.h>
#include"QUERY.h"
using namespace std;
//g++ -o C1 TDB.cpp QUERY.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
 //sudo cat /etc/mysql/debian.cnf
 //mysql -udebian-sys-maint -p
string FORMAT(string target, int len){
    string output(len,' ');

    for(int i = 0;i<len;i++){
        if(i<target.size()){
            output[i] = target[i];
        }
    }
    return output;
}


int main(){
    MYSQL mysql;
    mysql_init(&mysql); 
    //vicky
    // if(mysql_real_connect(&mysql,"localhost","root","123456","Project",0,NULL,0) == NULL){
    if(mysql_real_connect(&mysql,"localhost","Manager","123456","OS",0,NULL,0) == NULL){
	    cout << "connect fail\n";
	    return-1;
    }
    else{
        cout<<"connect success\n";
    }
    string sql = "select * from users where ID = '202164700325'";
    MYSQL_RES * result = NULL;
    basic_search(&mysql, &result, sql);
    MYSQL_ROW row = mysql_fetch_row(result);
    cout<<FORMAT(row[0],20)<<FORMAT(row[1],20)<<FORMAT(row[2],20)<<FORMAT(row[3],20);
}