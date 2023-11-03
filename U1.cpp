#include<sys/types.h>
#include<sys/socket.h>   
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<iostream>
using namespace std;
#define BUFF_LEN 512
#define SERVER_IP "172.0.1.1"
#define SERVER_PORT 8888


void udp_msg_sender(int fd, struct sockaddr* dst,int user_id)
{

    socklen_t len;
    struct sockaddr_in src;
    while(1)
    {
        char buf[BUFF_LEN];
        sprintf(buf,"User %d is on!\n",user_id);
        len = sizeof(*dst);
        sendto(fd, buf, BUFF_LEN, 0, dst, len);
        memset(buf, 0, BUFF_LEN);
        recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
        printf("Server:%s\n",buf);


        //first choice --- login options
        cout<<"login options: 1.Student 2.Teacher 3.Administrator 4.Register 5.Quit"<<endl;
        INPUT:
        int get;
        cin>>get;
        if(get==1||get==2||get==3||get==4||get==5){}else{
            cout<<"illegal input!"<<endl;
            goto INPUT;
        }
        if(get==5){
            memset(buf, 0, BUFF_LEN);
            sprintf(buf,"User login choice:%d,QUIT\n",get);
            sendto(fd, buf, BUFF_LEN, 0, dst, len);
            exit(0);
        }
        //send the choice back to the terminal
        memset(buf, 0, BUFF_LEN);
        sprintf(buf,"User login choice:%d\n",get);
        sendto(fd, buf, BUFF_LEN, 0, dst, len);
        memset(buf, 0, BUFF_LEN);
        recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
        cout<<buf<<endl;
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
    
    udp_msg_sender(client_fd, (struct sockaddr*)&ser_addr,10002);


    cout<<"User is off! Thanks for usage"<<endl;
    return 0;
}