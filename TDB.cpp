#include<iostream>
#include<cstring>
#include<vector>
#include<mysql/mysql.h>
using namespace std;
//g++ -o C1 TDB.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
 //sudo cat /etc/mysql/debian.cnf
 //mysql -udebian-sys-maint -p

int main(){
    MYSQL mysql;
    mysql_init(&mysql); 
    if(mysql_real_connect(&mysql,"localhost","root","123456","Project",0,NULL,0) == NULL){
	    cout << "connect fail\n";
	    return-1;
    }
    else{
        cout<<"connect success\n";
    }
    string sql = "drop from users where ID = '202164700325'";
    mysql_query(&mysql,sql.c_str());
    MYSQL_RES * result = mysql_store_result(&mysql);
}