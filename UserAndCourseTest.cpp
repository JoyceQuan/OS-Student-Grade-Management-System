// #include "USER.h"
// #include "COURSE.h"
#include "FileSystem/inode.h"
#include "FileSystem/txt.h"
#include "QUERY.h"
#include<mysql/mysql.h>
#include "CODES.h"
using namespace std;

int main() 
{
    //初始化部分
    SuperBlock superBlock;
    struct InodeList inodeListMem[MAX_INODELIST_NUMBER];//inodelist空间,一共有124个inodelist   //212*124  //有层次结构，不需要用bitmap
    struct FileBlock blockMem[BLOCK_NUMBER];//存block的空间，一共有128个block  //512*1120
    init_inode(superBlock,inodeListMem,blockMem);

    int running = 1;
    int choice;
    string info[3];
    //mysql init and connect
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
    //query
    string sql = "select * from Course where ID = 'CS101'";
    MYSQL_RES * result = NULL;
    basic_search(&mysql, &result, sql);
    MYSQL_ROW row = mysql_fetch_row(result);
    cout<<FORMAT(row[0],20)<<FORMAT(row[1],20)<<FORMAT(row[2],20)<<FORMAT(row[3],20);

    
    return 0;

}