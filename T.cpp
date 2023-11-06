#include<sys/types.h>
#include<sys/socket.h>   
#include<sys/stat.h>
#include<dirent.h>  
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>
#include<stdio.h>
#include<iostream>
#include<thread>
#include<mutex>
#include<vector>
#include<sstream>
#include<fstream>
#include<mysql/mysql.h>
#include<unordered_set>
#include"PRIVILEDGE.h"
#include"CODES.h"
#define SERVER_PORT 8888  // 定义服务器端口为8888
#define BUFF_LEN 1024  // 定义消息缓冲区长度为1024字节
#define IP_MAXSIZE 9999  // 定义IP最大数量为9999

using namespace std;

//g++ -o A2 T.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
//-----------------------------------------------------------


int FD;  // 文件描述符
int NUM;  // 用户数
bool IP_STATE[IP_MAXSIZE];  // 存储IP状态的布尔数组
int pointer = 0;  // 指向IP状态数组的指针
mutex LOCK;  // 互斥锁，用于多线程同步
unordered_set<string> ID_search;  // 存储用户ID的无序集合
MYSQL mysql;  // MySQL数据库连接句柄

//都可以替换，但Trecv_msg会接收不到目前。但1106的提交还是没有替换版的，将对应函数放在对应位置上打了注释
// void Tsend_msg(char* buf,char operate_code,string user_id,struct sockaddr_in clent_addr,socklen_t len){
//     memset(buf,0,BUFF_LEN);
//     sprintf(buf,"%c%s%s",operate_code,user_id.c_str(),TERMINAL_ID);
//     sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
// }

// void Trecv_msg(char* buf,string user_id,struct sockaddr_in clent_addr,socklen_t len){
//     memset(buf,0,BUFF_LEN);
//     recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
// }

void Mul_handle(string user_id, struct sockaddr* dst){
    priviledge user_P;  // 用于存储用户权限的对象
    string USER_ID, USER_IDENTITY, USER_NAME;  // 存储用户信息的变量
    string target_path = "./";  // 目标路径的字符串
    struct sockaddr_in clent_addr;  // 客户端地址信息结构体
    socklen_t len = sizeof(clent_addr);  // 客户端地址结构体的长度
    LOCK.lock();  // 加锁，确保线程安全
    NUM++;  // 用户数加1
    cout << "Current users:" << NUM << endl;
    LOCK.unlock();  // 解锁
    cout << user_id << ": start receiving!" << endl;

    // 等待用户登录选项
    WAIT1:
    target_path = "./";  // 重置目标路径
    sleep(1);  // 休眠1秒
    char buf[BUFF_LEN];  // 消息缓冲区
    memset(buf, 0, BUFF_LEN);  // 初始化消息缓冲区
    recvfrom(FD, buf, BUFF_LEN, 0, (struct sockaddr*)&clent_addr, &len);  // 接收消息
    cout << "receive package from " << user_id << ":" << buf << endl;
    string str_temp(buf);  // char*转string

    // 检查接收到的消息是否符合特定格式
    if (!isneed(LOGIN_PERSON_CODE, TERMINAL_ID, str_temp)) {
        cout << "receive package from " << user_id << ":" << buf << ", wrong package, throw it back!" << endl;
        sendto(FD, buf, BUFF_LEN, 0, (struct sockaddr*)&clent_addr, len);
        goto WAIT1;  // 重新等待
    }

    string str_user_id = get_region(str_temp);  // 从消息中提取用户ID
    str_temp = decode(str_temp);  // 解码消息
    cout << user_id << ":" << buf << endl;

    // 用户选择退出登录
    if (str_temp[0] == '5') {
        cout << "User login choice: " << decode(buf) << ", QUIT!" << endl;

        //Tsend_msg(buf,LOGOUT_CONFIRMATION,user_id,clent_addr,len);
        memset(buf, 0, BUFF_LEN);// 初始化消息缓冲区
        // 使用 sprintf 函数将需要发送的退出确认消息写入消息缓冲区 buf，消息格式为字符 '4' + 用户 ID + 终端 ID
        sprintf(buf, "%c%s%s", LOGOUT_CONFIRMATION, user_id.c_str(), TERMINAL_ID);
        // 使用 sendto 函数将退出确认消息发送给客户端，参数 目标地址信息结构体指针 (struct sockaddr*)&clent_addr 
        sendto(FD, buf, BUFF_LEN, 0, (struct sockaddr*)&clent_addr, len);

        LOCK.lock();  // 加锁
        NUM--;  // 用户数减1
        IP_STATE[atoi(str_user_id.c_str()) - 10001] = false;  // 将用户IP状态设置为不在线
        cout << "Current users:" << NUM << endl;
        LOCK.unlock();  // 解锁
        return;
    }


    // 登录请求
    else if(str_temp[0]=='1'||str_temp[0]=='2'||str_temp[0]=='3'){
        //Tsend_msg(buf,LOGIN_PERMIT,user_id,clent_addr,len);
        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s",LOGIN_PERMIT,user_id.c_str(),TERMINAL_ID);
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
        
        WAIT4://wait for user password and id
        sleep(1);
        //Trecv_msg(buf,user_id,clent_addr,len);
        memset(buf,0,BUFF_LEN);
        recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        cout<<"receive package from"<<user_id<<":"<<buf<<endl;
        string str_temp2(buf);
        //检查是否合法
        if(!isneed(LOGIN_PERSON_CODE,TERMINAL_ID,str_temp2)){
            cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
            // 将无效消息发送回客户端
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            goto WAIT4;
        }

         // 使用 stringstream 分割消息内容，存储在 temp_user_information 数组中
        str_temp2 = decode(str_temp2);
        stringstream ss(str_temp2);
        string temp_user_information[2];
        int pos = 0;
        string temp;
        while(getline(ss, temp, ',')){
            temp_user_information[pos] = temp;
            pos++;
        }
        cout<<"start query:"<<endl;

        //MYSQL查询
        string sql = "select * from users where ID = '"+temp_user_information[0]+"'";
        cout<<sql<<endl;
        mysql_query(&mysql,sql.c_str());
        MYSQL_RES * result = mysql_store_result(&mysql);
        MYSQL_ROW row;
        row = mysql_fetch_row(result);
        //检查用户密码和身份是否匹配
        if(strcmp(row[3],&str_temp[0])==0&&strcmp(row[2],temp_user_information[1].c_str())==0){
            USER_ID = row[0];
            USER_NAME = row[1];
            USER_IDENTITY = row[2];
            mysql_free_result(result); // 释放数据库查询结果
            //Tsend_msg(buf,LOGIN_FEEDBACK_CODE_SUCCESS,user_id,clent_addr,len);
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s%s",LOGIN_FEEDBACK_CODE_SUCCESS,user_id.c_str(),TERMINAL_ID,row[1]);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
        }
        else{
            mysql_free_result(result);
            //Tsend_msg(buf,LOGIN_FEEDBACK_CODE_FAIL,user_id,clent_addr,len);
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_FAIL,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            goto WAIT1;// 回到登录选项等待状态
        }

        //login successfully // 登录成功后的操作
        //在对应权限表 写入用户
        target_path = target_path + "data/information/"+temp_user_information[0]+"/priviledge.txt";
        user_P.set(row[3]);
        user_P.set2(target_path);
        cout<<"initialization finished"<<endl;
    }
    else if(str_temp[0]=='4'){
        //Tsend_msg(buf,LOGIN_PERMIT,user_id,clent_addr,len);
        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s",LOGIN_PERMIT,user_id.c_str(),TERMINAL_ID);
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);

        WAIT2://wait for root password
        sleep(1);
        //Trecv_msg(buf,user_id,clent_addr,len);
        memset(buf,0,BUFF_LEN);
        recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        cout<<"receive package from"<<user_id<<":"<<buf<<endl;
        string str_temp(buf);
        if(!isneed(LOGIN_PERSON_CODE,TERMINAL_ID,str_temp)){
            cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            goto WAIT2;
        }

        cout<<"start query:"<<endl;

        str_temp = decode(str_temp);//get password

        string sql = "select * from users where ID = '200000000000'";
        mysql_query(&mysql,sql.c_str());
        MYSQL_RES * result = mysql_store_result(&mysql);
        MYSQL_ROW row;
        row = mysql_fetch_row(result);

        if(strcmp(row[2],str_temp.c_str())==0){
            mysql_free_result(result);
            //Tsend_msg(buf,LOGIN_ROOT_PASSWORD_RIGHT,user_id,clent_addr,len);
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_ROOT_PASSWORD_RIGHT,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);

            WAIT3:
            sleep(1);
            //Trecv_msg(buf,user_id,clent_addr,len);
            memset(buf,0,BUFF_LEN);
            recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
            if(!isneed(ROOT_OP,TERMINAL_ID,buf)){
                cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                goto WAIT3;
            }
            cout<<"receive from "<<user_id<<":"<<buf<<endl;
            string str_temp_get(buf);
            str_temp_get = decode(str_temp_get);
            //cout<<"AAAreceive from "<<user_id<<":"<<str_temp_get<<endl;
            
            if(str_temp_get[0]=='1'){
                //create user
                cout<<"start creating"<<endl;
                string information[4];
                str_temp_get = str_temp_get.substr(2);//从第二位开始取
                stringstream ss(str_temp_get);
                int pos = 0;
                string temp;
                while(getline(ss, temp, ',')){
                    information[pos] = temp;
                    pos++;
                }
                for(int i = 0;i<4;i++){
                    cout<<information[i]<<endl;
                }

                //设置用户权限
                user_P.set(information[3]);
                //create file
                target_path = target_path + "data/information/" + information[0];
                cout<<target_path<<endl;
                // 检查目录是否存在，如果不存在，创建目录
                DIR * mydir =NULL;
	            mydir=opendir(target_path.c_str());
                if(mydir==NULL){
                    mkdir(target_path.c_str(),0777);
                    cout<<"create file"<<endl;
                }else{
                    cout<<"file exist"<<endl;
                }
                closedir(mydir);
                //create information txt// 创建用户信息文件
                ofstream of_for_priviledge, of_for_course, of_for_credit;
                of_for_priviledge.open(target_path.substr(2)+"/priviledge.txt", std::ios::out | std::ios::app); 
                cout<<target_path.substr(2)+"/priviledge.txt"<<endl;
                of_for_course.open(target_path.substr(2)+"/course.txt", std::ios::out | std::ios::app); 
                cout<<target_path.substr(2)+"/course.txt"<<endl;
                of_for_credit.open(target_path.substr(2)+"/credit.txt", std::ios::out | std::ios::app); 
                cout<<target_path.substr(2)+"/credit.txt"<<endl;
                 // 将用户权限信息写入文件
                of_for_priviledge<<information[3]<<"\n";
                for(int i = 0;i<PRI_LEN;i++){
                    of_for_priviledge<<user_P.flagset[i]<<"\n";
                }

                //MYSQL查询
                sql = "insert into users values('"+information[0]+"','"+information[1]+"','"+information[2]+"',"+information[3]+")";
                cout<<"query:"<<sql<<endl;
                mysql_query(&mysql,sql.c_str());
                //Tsend_msg(buf,ROOT_OP_FEEDBACK,user_id,clent_addr,len);
                memset(buf,0,BUFF_LEN);
                sprintf(buf,"%c%s%s",ROOT_OP_FEEDBACK,user_id.c_str(),TERMINAL_ID);
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                cout<<"done"<<endl;
                goto WAIT1;// 回到登录选项等待状态
            }
            else{
                // 如果接收到的消息的第一个字符不是 '1'，表示管理员操作无效
                mysql_free_result(result);
                //Tsend_msg(buf,ROOT_OP_FEEDBACK,user_id,clent_addr,len);
                memset(buf,0,BUFF_LEN);
                sprintf(buf,"%c%s%s",ROOT_OP_FEEDBACK,user_id.c_str(),TERMINAL_ID);
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                cout<<"back"<<endl;
                goto WAIT1;
            }

        }//right 
        else{
            // 如果管理员密码不正确
            //Tsend_msg(buf,LOGIN_ROOT_PASSWORD_WRONG,user_id,clent_addr,len);
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_ROOT_PASSWORD_WRONG,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            goto WAIT1;
        }//wrong

    }
    //----------


}

// 处理用户消息的函数
void handle_msg(int fd,struct sockaddr* dst){
    char buf[BUFF_LEN];  // 消息缓冲区
    socklen_t len;  // 客户端地址结构体的长度
    int count;  // 接收到的字节数
    int user = 0;  // 用户标识
    struct sockaddr_in clent_addr;  // 客户端地址信息结构体
    while (true) {
        BEGINS:
        sleep(1);  // 休眠1秒
        char buf_temp[BUFF_LEN];  // 临时消息缓冲区
        memset(buf_temp, 0, BUFF_LEN);  // 初始化临时消息缓冲区
        memset(buf, 0, BUFF_LEN);  // 初始化消息缓冲区
        len = sizeof(clent_addr);  // 获取客户端地址结构体的长度
        count = recvfrom(fd, buf_temp, BUFF_LEN, 0, (struct sockaddr*)&clent_addr, &len);  // 接收消息
        if(count == -1){
            cout<<"Receive data error!"<<endl;
            return;
        }
        string str_temp(buf_temp);  // 将接收到的消息转换为字符串

        // 检查接收到的消息是否为登录格式
        if (!isneed(LOGIN_CODE, TERMINAL_ID, str_temp)) {
            sendto(fd, buf_temp, BUFF_LEN, 0, (struct sockaddr*)&clent_addr, len);
            goto BEGINS;  // 重新等待消息
        }

        string str_temp2 = decode(str_temp);  // 解码消息
        string str_user_code = get_region(str_temp);  // 从消息中提取用户ID

        //contribute ip to user
        int true_ip = 0;  // 用户的真实IP地址
        int temp_pointer = pointer;  // 保存当前指针位置的临时变量
        int flag = 0;  // 标志变量

        while(flag!=2&&IP_STATE[pointer]){
            if(pointer==temp_pointer){
                flag++;
            }
            if(pointer==IP_MAXSIZE-1){
                pointer = 0;
            }
            pointer++;
        }



        //full// 如果IP地址已满
        if(flag==2){
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_FAIL,str_user_code.c_str(),TERMINAL_ID);
            sendto(fd,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            goto BEGINS;
        }

        //success  // IP地址分配成功
        true_ip = 10000+pointer+1;//真实ip
        LOCK.lock();
        IP_STATE[pointer] = true;
        LOCK.unlock();
        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_SUCCESS,str_user_code.c_str(),TERMINAL_ID);
        

        cout<<"Receive:"<<true_ip<<" is on!"<<endl;
        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s%d",LOGIN_FEEDBACK_CODE_SUCCESS,str_user_code.c_str(),TERMINAL_ID,true_ip);
        sendto(fd,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len); 
        //login option receive
        thread(Mul_handle,to_string(true_ip),dst).detach();
        goto BEGINS;



    }

}


int main(){
    cout<<"Terminal program is on"<<endl;
    
    int UDP_fd = socket(AF_INET,SOCK_DGRAM,0);
    int RET;
    struct sockaddr_in server_address;
    // 检查套接字创建是否成功
    if(UDP_fd == -1){
        cout<<"Network initialization error!"<<endl;
        exit(0);
    }
    else{
        cout<<"Network initialization success!"<<endl;
    }

    cout<<"-------------------------------------------"<<endl;

    cout<<"Server initialization begins:"<<endl;
    // 初始化服务器地址信息
    memset(&server_address,0,sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(SERVER_PORT);
    cout<<"Done!"<<endl;

     // 绑定服务器地址到套接字
    cout<<"Bind server to the socket:"<<endl;
    RET = bind(UDP_fd,(struct sockaddr*)&server_address,sizeof(server_address));
    if(RET<0){
        cout<<"Failed!"<<endl;
        exit(0);
    }
    else{
        cout<<"Done!"<<endl;
    }
    // 初始化 IP 状态数组，所有元素标记为 false
    cout<<"IP state initialization begins:"<<endl;
    for(int i = 0;i<IP_MAXSIZE;i++){
        IP_STATE[i] = false;
    }
    pointer = 0;
    cout<<"Done!"<<endl;
    // 初始化 MySQL 连接
    cout<<"Mysql initialization begins:"<<endl;
    mysql_init(&mysql); 
    if(mysql_real_connect(&mysql,"localhost","root","123456","Project",0,NULL,0) == NULL){
	    cout << "connect fail!\n";
	    exit(0);
    }
    else{
        cout<<"connect success!\n";
    }


    cout<<"-------------------------------------------"<<endl;
    cout<<"Start listening:"<<endl;
    // 设置 FD 为 UDP_fd，用于处理消息
    FD = UDP_fd;
    handle_msg(UDP_fd, (struct sockaddr*)&server_address);


    cout<<"Terminal is off! Thanks for usage"<<endl;
}