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
#define SERVER_PORT 8888
#define BUFF_LEN 1024
#define IP_MAXSIZE 9999

//g++ -o A1 T.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
//-----------------------------------------------------------
using namespace std;

int FD;
int NUM;
bool IP_STATE[IP_MAXSIZE];
int pointer = 0;
mutex LOCK;
unordered_set<string>ID_search;
MYSQL mysql;




void Mul_handle(string user_id, struct sockaddr* dst){
    priviledge user_P;
    string USER_ID,USER_IDENTITY,USER_NAME;
    string target_path = "./";
    struct sockaddr_in clent_addr;
    socklen_t len = sizeof(clent_addr);
    LOCK.lock();
    NUM++;
    cout<<"Current users:"<<NUM<<endl;
    LOCK.unlock();
    cout<<user_id<<": start receiving!"<<endl;
    WAIT1://wait for login options
    target_path = "./";
    sleep(1);
    char buf[BUFF_LEN];
    memset(buf,0,BUFF_LEN);
    recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
    cout<<"receive package from"<<user_id<<":"<<buf<<endl;
    string str_temp(buf);
    if(!isneed(LOGIN_PERSON_CODE,TERMINAL_ID,str_temp)){
        cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
        goto WAIT1;
    }
    string str_user_id = get_region(str_temp);
    str_temp = decode(str_temp);
    cout<<user_id<<":"<<buf<<endl;


    //quit option
    if(str_temp[0]=='5'){
        cout<<"User login choice:"<<decode(buf)<<", QUIT!"<<endl;

        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s",LOGOUT_CONFIRMATION,user_id.c_str(),TERMINAL_ID);
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);

        LOCK.lock();
        NUM--;
        IP_STATE[atoi(str_user_id.c_str())-10001] = false;
        cout<<"Current users:"<<NUM<<endl;
        LOCK.unlock();
        return;
    }



    //login code
    else if(str_temp[0]=='1'||str_temp[0]=='2'||str_temp[0]=='3'){
        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s",LOGIN_PERMIT,user_id.c_str(),TERMINAL_ID);
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
        WAIT4://wait for user password and id
        sleep(1);
        memset(buf,0,BUFF_LEN);
        recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        cout<<"receive package from"<<user_id<<":"<<buf<<endl;
        string str_temp2(buf);
        if(!isneed(LOGIN_PERSON_CODE,TERMINAL_ID,str_temp2)){
            cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            goto WAIT4;
        }
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
        string sql = "select * from users where ID = '"+temp_user_information[0]+"'";
        cout<<sql<<endl;
        mysql_query(&mysql,sql.c_str());
        MYSQL_RES * result = mysql_store_result(&mysql);
        MYSQL_ROW row;
        row = mysql_fetch_row(result);
        if(strcmp(row[3],&str_temp[0])==0&&strcmp(row[2],temp_user_information[1].c_str())==0){
            USER_ID = row[0];
            USER_NAME = row[1];
            USER_IDENTITY = row[2];
            mysql_free_result(result);
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s%s",LOGIN_FEEDBACK_CODE_SUCCESS,user_id.c_str(),TERMINAL_ID,row[1]);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
        }
        else{
            mysql_free_result(result);
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_FAIL,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            goto WAIT1;
        }

        //login successfully

        target_path = target_path + "data/information/"+temp_user_information[0]+"/priviledge.txt";
        user_P.set(row[3]);
        user_P.set2(target_path);
        cout<<"initialization finished"<<endl;
    }
    else if(str_temp[0]=='4'){
        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s",LOGIN_PERMIT,user_id.c_str(),TERMINAL_ID);
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);

        WAIT2://wait for root password
        sleep(1);
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
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_ROOT_PASSWORD_RIGHT,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);

            WAIT3:
            sleep(1);
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
            if(str_temp_get[0]=='1'){
                //create user
                cout<<"start creating"<<endl;
                string information[4];
                str_temp_get = str_temp_get.substr(2);
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
                
                user_P.set(information[3]);
                //create file
                target_path = target_path + "data/information/" + information[0];
                cout<<target_path<<endl;
                DIR * mydir =NULL;
	            mydir=opendir(target_path.c_str());
                if(mydir==NULL){
                    mkdir(target_path.c_str(),0777);
                    cout<<"create file"<<endl;
                }else{
                    cout<<"file exist"<<endl;
                }
                closedir(mydir);
                //create information txt
                ofstream of_for_priviledge, of_for_course, of_for_credit;
                of_for_priviledge.open(target_path.substr(2)+"/priviledge.txt", std::ios::out | std::ios::app); 
                cout<<target_path.substr(2)+"/priviledge.txt"<<endl;
                of_for_course.open(target_path.substr(2)+"/course.txt", std::ios::out | std::ios::app); 
                cout<<target_path.substr(2)+"/course.txt"<<endl;
                of_for_credit.open(target_path.substr(2)+"/credit.txt", std::ios::out | std::ios::app); 
                cout<<target_path.substr(2)+"/credit.txt"<<endl;
                of_for_priviledge<<information[3]<<"\n";
                for(int i = 0;i<PRI_LEN;i++){
                    of_for_priviledge<<user_P.flagset[i]<<"\n";
                }




                sql = "insert into users values('"+information[0]+"','"+information[1]+"','"+information[2]+"',"+information[3]+")";
                cout<<"query:"<<sql<<endl;
                mysql_query(&mysql,sql.c_str());
                memset(buf,0,BUFF_LEN);
                sprintf(buf,"%c%s%s",ROOT_OP_FEEDBACK,user_id.c_str(),TERMINAL_ID);
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                cout<<"done"<<endl;
                goto WAIT1;
            }
            else{
                mysql_free_result(result);
                memset(buf,0,BUFF_LEN);
                sprintf(buf,"%c%s%s",ROOT_OP_FEEDBACK,user_id.c_str(),TERMINAL_ID);
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                cout<<"back"<<endl;
                goto WAIT1;
            }

        }//right 
        else{
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_ROOT_PASSWORD_WRONG,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            goto WAIT1;
        }//wrong

    }
    //----------


}
void handle_msg(int fd,struct sockaddr* dst){

    char buf[BUFF_LEN];
    socklen_t len;
    int count;
    int user = 0;
    struct sockaddr_in clent_addr;
    while(true){
        BEGINS:
        sleep(1);
        char buf_temp[BUFF_LEN];
        memset(buf_temp,0,BUFF_LEN);
        memset(buf,0,BUFF_LEN);
        len = sizeof(clent_addr);
        count = recvfrom(fd,buf_temp,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        if(count == -1){
            cout<<"Receive data error!"<<endl;
            return;
        }


        string str_temp(buf_temp);
        if(!isneed(LOGIN_CODE,TERMINAL_ID,str_temp)){
            sendto(fd,buf_temp,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            goto BEGINS;
        }


        string str_temp2 = decode(str_temp);
        string str_user_code = get_region(str_temp);

        //contribute ip to user



        int true_ip = 0;
        int temp_pointer = pointer;
        int flag = 0;
        while(flag!=2&&IP_STATE[pointer]){
            if(pointer==temp_pointer){
                flag++;
            }
            if(pointer==IP_MAXSIZE-1){
                pointer = 0;
            }
            pointer++;
        }



        //full
        if(flag==2){
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_FAIL,str_user_code.c_str(),TERMINAL_ID);
            sendto(fd,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            goto BEGINS;
        }

        //success
        true_ip = 10000+pointer+1;
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
    if(UDP_fd == -1){
        cout<<"Network initialization error!"<<endl;
        exit(0);
    }
    else{
        cout<<"Network initialization success!"<<endl;
    }

    cout<<"-------------------------------------------"<<endl;

    cout<<"Server initialization begins:"<<endl;
    memset(&server_address,0,sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(SERVER_PORT);
    cout<<"Done!"<<endl;

    cout<<"Bind server to the socket:"<<endl;
    RET = bind(UDP_fd,(struct sockaddr*)&server_address,sizeof(server_address));
    if(RET<0){
        cout<<"Failed!"<<endl;
        exit(0);
    }
    else{
        cout<<"Done!"<<endl;
    }
    cout<<"IP state initialization begins:"<<endl;
    for(int i = 0;i<IP_MAXSIZE;i++){
        IP_STATE[i] = false;
    }
    pointer = 0;
    cout<<"Done!"<<endl;

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
    FD = UDP_fd;
    handle_msg(UDP_fd, (struct sockaddr*)&server_address);


    cout<<"Terminal is off! Thanks for usage"<<endl;
}