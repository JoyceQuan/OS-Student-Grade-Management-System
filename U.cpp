#include<sys/types.h>
#include<sys/socket.h> 
#include<sys/stat.h>
#include<dirent.h>  
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<iostream>
#include"CODES.h"
using namespace std;
#define BUFF_LEN 512
#define SERVER_IP "172.0.1.1"
#define SERVER_PORT 8888

//g++ -o B2 U.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
//-----------------------------------------------------------
using namespace std;
string TRUE_IP;
string identity_to_name[4] = {
    "",
    "student",
    "teacher",
    "administrator"
};



void udp_msg_sender(int fd, struct sockaddr* dst)
{
    char pos_copy = '1';
    int user_id;
    socklen_t len;
    struct sockaddr_in src;
    char buf[BUFF_LEN];
    cout<<"Asking for your identity id(10001-19999)"<<endl;
        //error!!!!!   ok
    user_id = 10001;  // 初始化 user_id 为 10001，从终端获取正确的 IP
    sprintf(buf,"%c%s%d",LOGIN_CODE,TERMINAL_ID,user_id);
    len = sizeof(*dst);
    sendto(fd, buf, BUFF_LEN, 0, dst, len);
    cout<<"Start waiting for the response!"<<endl;

    WAITING_LOGINFEEDBACK:
    sleep(1);
    memset(buf, 0, BUFF_LEN);
    recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
    cout<<"receive:"<<buf<<endl;
    string str_temp(buf);
    // 如果接收到不是预期的反馈消息，重新发送请求
    if(!(isneed(LOGIN_FEEDBACK_CODE_FAIL,to_string(user_id),str_temp)||isneed(LOGIN_FEEDBACK_CODE_SUCCESS,to_string(user_id),str_temp))){
        cout<<"receive package:"<<buf<<",error"<<endl;
        sendto(fd, buf, BUFF_LEN, 0, dst, len);
        goto WAITING_LOGINFEEDBACK;
    }
    if(str_temp[0]==LOGIN_FEEDBACK_CODE_FAIL){
        cout<<"The terminal is full, please wait a minute!"<<endl;
        exit(0);
    }
    else{
        //成功，解码出真实ip
        TRUE_IP = decode(str_temp);
        cout<<"get ip: "<<TRUE_IP<<endl;
    }

    printf("Server: successfully created!\n");
    



    //first choice --- login options
    INPUT:
    cout<<"login options: 1.Student 2.Teacher 3.Administrator 4.Register 5.Quit"<<endl;
    int get;
    cin>>get;
    if(get==1||get==2||get==3||get==4||get==5){}else{
        cout<<"illegal input!"<<endl;
        goto INPUT;
    }
    if(get==5){
        memset(buf, 0, BUFF_LEN);
        sprintf(buf,"%c%s%s%d",LOGIN_PERSON_CODE,TERMINAL_ID,TRUE_IP.c_str(),get);
        sendto(fd, buf, BUFF_LEN, 0, dst, len);


        WAIT_FOR_LOGOUT_FEEDBACK:
        sleep(1);
        memset(buf, 0, BUFF_LEN);
        recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&src, &len);  //wait for a feedback before leave
        cout<<"receiving:"<<buf<<endl;
        if(!isneed(LOGOUT_CONFIRMATION,TRUE_IP,buf)){
            sendto(fd, buf, BUFF_LEN, 0, dst, len);
            goto WAIT_FOR_LOGOUT_FEEDBACK;
        }
        cout<<"Thank you for using!"<<endl;
        exit(0);
    }
    
    //登录，将get发向终端
    memset(buf, 0, BUFF_LEN);
    sprintf(buf,"%c%s%d%d",LOGIN_PERSON_CODE,TERMINAL_ID,user_id,get);
    sendto(fd, buf, BUFF_LEN, 0, dst, len);

    WAIT_FOR_LOGINPERMIT_FEEDBACK:
    sleep(1);
    memset(buf, 0, BUFF_LEN);
    recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&src, &len);  //接收来自server的信息

    if(!isneed(LOGIN_PERMIT,TRUE_IP,buf)){
        sendto(fd, buf, BUFF_LEN, 0, dst, len);
        goto WAIT_FOR_LOGINPERMIT_FEEDBACK;
    }
    string USER_INFORMATION[4];
    if(get==1 || get==2 || get==3){
        string USER_ID,USER_PASSWORD;
        cout<<"Enter your id:";
        cin>>USER_ID;
        cout<<"Enter your password:";
        cin>>USER_PASSWORD;
        //将登录信息发到终端
        memset(buf, 0, BUFF_LEN);
        sprintf(buf,"%c%s%d%s,%s",LOGIN_PERSON_CODE,TERMINAL_ID,user_id,USER_ID.c_str(),USER_PASSWORD.c_str());
        sendto(fd, buf, BUFF_LEN, 0, dst, len);

        // 等待服务器的登录反馈
        WAIT_FOR_LASTSTEP_FEEDBACK:
        sleep(1);
        memset(buf, 0, BUFF_LEN);
        recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
        if(!(isneed(LOGIN_FEEDBACK_CODE_SUCCESS,TRUE_IP,buf)||isneed(LOGIN_FEEDBACK_CODE_FAIL,TRUE_IP,buf))){
            sendto(fd, buf, BUFF_LEN, 0, dst, len);
            goto WAIT_FOR_LASTSTEP_FEEDBACK;// 重新等待反馈
        }

        if(buf[0]==LOGIN_FEEDBACK_CODE_FAIL){
            cout<<"information error!"<<endl;// 登录失败
            goto INPUT;

        }
        else{
            // 登录成功，记录用户信息
            USER_INFORMATION[0] = USER_ID;
            USER_INFORMATION[1] = decode(buf);
            USER_INFORMATION[2] = USER_PASSWORD;
            USER_INFORMATION[3] = to_string(get);
            cout<<"login success!"<<endl;
        }   
    }
    else if(get==4){
        //选择了register
        cout<<"Enter password for root:";
        string getpassword;
        cin>>getpassword;
        //向终端发密码
        memset(buf, 0, BUFF_LEN);
        sprintf(buf,"%c%s%d%s",LOGIN_PERSON_CODE,TERMINAL_ID,user_id,getpassword.c_str());
        sendto(fd, buf, BUFF_LEN, 0, dst, len);

        // 等待服务器的密码反馈
        WAIT_FOR_LOGOUTPASSWORD_FEEDBACK:
        sleep(1);
        memset(buf, 0, BUFF_LEN);
        recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
        if(!(isneed(LOGIN_ROOT_PASSWORD_RIGHT,TRUE_IP,buf)||isneed(LOGIN_ROOT_PASSWORD_WRONG,TRUE_IP,buf))){
            sendto(fd, buf, BUFF_LEN, 0, dst, len);
            goto WAIT_FOR_LOGOUTPASSWORD_FEEDBACK;// 重新等待反馈
        }
        string str_temp_get(buf);
        if(str_temp_get[0]==LOGIN_ROOT_PASSWORD_WRONG){
            cout<<"Password wrong!"<<endl; // 密码错误
            goto INPUT;
        }
        else{
            cout<<"Password right"<<endl;
        }


        INPUT2:
        cout<<"root options: 1.create user 2.back"<<endl;
        cin>>get;
        if(get==1||get==2){}else{
            cout<<"illegal input!"<<endl;
            goto INPUT2;
        }
        string USER_ID = "",USER_ACCOUNT = "",USER_PASSWORD = "",USER_IDENTITY = "";
        if(get==1){
            cout<<"enter id:";
            cin>>USER_ID;
            cout<<"enter account:";
            cin>>USER_ACCOUNT;
            cout<<"enter password:";
            cin>>USER_PASSWORD;
            cout<<"enter identity: 1.Student 2.Teacher 3.Administor:";
            cin>>USER_IDENTITY;

        }
        else{

        }
        // 向服务器发送创建用户请求
        memset(buf, 0, BUFF_LEN);
        sprintf(buf,"%c%s%d%d,%s,%s,%s,%s",ROOT_OP,TERMINAL_ID,user_id,get,USER_ID.c_str(),USER_ACCOUNT.c_str(),USER_PASSWORD.c_str(),USER_IDENTITY.c_str());
        sendto(fd, buf, BUFF_LEN, 0, dst, len);
        
        // 等待服务器的创建用户反馈
        WAIT_FOR_ROOT_FEEDBACK:
        sleep(1);
        memset(buf, 0, BUFF_LEN);
        recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
        if(!isneed(ROOT_OP_FEEDBACK,TRUE_IP,buf)){
            sendto(fd, buf, BUFF_LEN, 0, dst, len);
            goto WAIT_FOR_ROOT_FEEDBACK;// 重新等待反馈
        }
        
        cout<<"success"<<endl;
        goto INPUT;
    }
    cout<<"--------------------------------"<<endl;
    cout<<"Hi "+USER_INFORMATION[1]+",Welcome to the user system!"<<endl;

    while(pos_copy!='0'){
        cout<<"--------------------------------"<<endl;
        cout<<"Basic information:"<<endl;
        cout<<"ID:"<<USER_INFORMATION[0]<<endl;
        int id_for_identity_to_name = atoi(USER_INFORMATION[3].c_str());
        cout<<"Identity:"<<identity_to_name[id_for_identity_to_name]<<endl;
        cout<<"--------------------------------"<<endl;

        



    }



}

int main(int argc, char* argv[])
{
    int client_fd;
    struct sockaddr_in ser_addr;

    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    cout<<"User program is on"<<endl;
    if(client_fd < 0)
    {
        cout<<"Network initialization error!"<<endl;
        return -1;
    }
    else{
        cout<<"Network initialization success!"<<endl;
    }

    cout<<"-------------------------------------------"<<endl;
    cout<<"User's initialization begins:"<<endl;
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    //ser_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //注意网络序转换
    ser_addr.sin_port = htons(SERVER_PORT);  //注意网络序转换
    cout<<"Done!"<<endl;
    cout<<"-------------------------------------------"<<endl;
    cout<<"Start sending:"<<endl;
    
    udp_msg_sender(client_fd, (struct sockaddr*)&ser_addr);


    cout<<"User is off! Thanks for usage"<<endl;
    return 0;
}