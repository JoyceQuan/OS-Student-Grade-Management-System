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
#include"PAGE_CHANGE.h"
#include"QUERY.h"
#include"CODES.h"
#include"DATAS.h"
#define SERVER_PORT 8888
#define BUFF_LEN 2048
#define IP_MAXSIZE 9999

//g++ -o A1 T.cpp QUERY.cpp PAGE_CHANGE.cpp PRIVILEDGE.cpp CODES.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
//-----------------------------------------------------------
using namespace std;
string identity_to_name[4] = {
    "",
    "student",
    "teacher",
    "administrator"
};

int FD;//
int NUM;
bool IP_STATE[IP_MAXSIZE];
int pointer = 0;
mutex LOCK;
unordered_set<string>ID_search;
MYSQL mysql;
string DEAL_THE_CODE(string op_code, priviledge* user_information, int* if_extra_input,bool* error_detect,int* step, string extra, 
vector<string*>basic_information, string* target_path){
    cout<<"dealing option:"<<op_code<<endl;
    cout<<"if_extra_input:"<<*if_extra_input<<endl;
    string output = "";
    if(*if_extra_input==0){
        if(extra.size()==0){
            cout<<"enter part1"<<endl;
            //deal basic options 
            if(op_code=="11"||op_code=="12"||op_code=="13"||op_code=="14"||op_code=="15"||op_code=="16"||op_code=="17"||op_code=="18"){
                NEXT(user_information,op_code);
            }
            //deal code 19 291 3911 which related to 'back'
            if(op_code[op_code.size()-1] == 'b'){
                PREVIOUS(user_information);
            }
            //deal the second layer
            if(op_code=="211"){
                //search all the course in the table course
                query_all_courses(&mysql,output);
            }
            if(op_code=="281"){

            }
            if(op_code=="282"){
                query_basic_information(basic_information,output);
            }
            if(op_code=="283"){
                *if_extra_input = 1;
                output+="Please input your old password and your new password(2 inputs required, don't input redundant spaces), splitting by blank space\n";
            }
        }
        else{
            //deal system code here
            cout<<"system command"<<endl;
            if(extra=="help"){
                output+=output_for_help;
            }
            else{
                bool flag_bit = true;
                pair<vector<int>,vector<string>> get_result = deal_command(flag_bit,extra);
                cout<<"command type:"<<get_result.first[0]<<endl;
                cout<<"director:"<<get_result.first[1]<<endl;
                cout<<"secondary command type:"<<get_result.first[2]<<endl;
                cout<<"path:"<<get_result.second[0]<<endl;
                cout<<"user_id:"<<get_result.second[1]<<endl;
                if(!flag_bit){
                    output+="Inputs that are not recognized!\n";
                }
                else{
                    //deal the code here
                }
            }
        }
    }
    else{
        cout<<"enter part2"<<endl;
        if(op_code=="283"){
            stringstream ss(extra);
            vector<string>result;
            string str;
            while(getline(ss, str, ' ')){
                result.push_back(str);
            }
            if(result.size()!=2){
                *if_extra_input = 0;
                output+="Input error! Operation fails.\n";
            }
            else{
                if(*basic_information[4]!=result[0]){
                    *if_extra_input = 0;
                    output+="Password is wrong!\n";
                }
                else{
                    *if_extra_input = 0;
                    output+="Operation success\n";
                    *basic_information[4] = result[1];
                    string sql = "update users set Password = '"+result[1]+"' where ID = '"+*basic_information[1]+"'";
                    mysql_query(&mysql,sql.c_str());
                }
            }
        }
    }
    if(*if_extra_input==1){
        *step++;
    }
    else{
        *step = 0;
    }
    return output;
}


void Mul_handle(string user_id, struct sockaddr* dst){
    priviledge user_P;
    string USER_ID,USER_IDENTITY,USER_NAME,USER_PASSWORD;
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
    char buf[BUFF_LEN];
    memset(buf,0,BUFF_LEN);
    recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
    cout<<"receive package from"<<user_id<<":"<<buf<<endl;
    string str_temp(buf);
    if(!isneed(LOGIN_PERSON_CODE,TERMINAL_ID,str_temp)){
        cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
        sleep(1);
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
        memset(buf,0,BUFF_LEN);
        recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        cout<<"receive package from"<<user_id<<":"<<buf<<endl;
        string str_temp2(buf);
        if(!isneed(LOGIN_PERSON_CODE2,TERMINAL_ID,str_temp2)){
            cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
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


        if(ID_search.find(temp_user_information[0])!=ID_search.end()){
            mysql_free_result(result);
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_FAIL,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
            goto WAIT1;
        }


        if(strcmp(row[3],&str_temp[0])==0&&strcmp(row[2],temp_user_information[1].c_str())==0){
            ID_search.emplace(row[0]);
            USER_ID = row[0];
            USER_NAME = row[1];
            USER_PASSWORD = row[2];
            USER_IDENTITY = row[3];
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
            sleep(1);
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
        memset(buf,0,BUFF_LEN);
        recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        cout<<"receive package from"<<user_id<<":"<<buf<<endl;
        string str_temp(buf);
        if(!isneed(LOGIN_PERSON_CODE2,TERMINAL_ID,str_temp)){
            cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
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
            memset(buf,0,BUFF_LEN);
            recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
            if(!isneed(ROOT_OP,TERMINAL_ID,buf)){
                cout<<"receive package from"<<user_id<<":"<<buf<<", wrong package, throw it back!"<<endl;
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                sleep(1);
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
                //students
                if(user_P.identity == 1){
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
                }
                else if(user_P.identity==2){
                    string path_temp = target_path;
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
                    ofstream of_for_priviledge, of_for_group;
                    of_for_priviledge.open(target_path.substr(2)+"/priviledge.txt", std::ios::out | std::ios::app); 
                    cout<<target_path.substr(2)+"/priviledge.txt"<<endl;
                    of_for_group.open(target_path.substr(2)+"/group.txt", std::ios::out | std::ios::app); 
                    cout<<target_path.substr(2)+"/group.txt"<<endl;
                    of_for_priviledge<<information[3]<<"\n";
                    for(int i = 0;i<PRI_LEN;i++){
                        of_for_priviledge<<user_P.flagset[i]<<"\n";
                    }
                    target_path = path_temp + "data/teacher/" + information[0];
                    cout<<target_path<<endl;
                    mydir=opendir(target_path.c_str());
                    if(mydir==NULL){
                        mkdir(target_path.c_str(),0777);
                        cout<<"create file"<<endl;
                    }else{
                        cout<<"file exist"<<endl;
                    }
                    closedir(mydir);
                }
                else{
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
                    ofstream of_for_priviledge, of_for_group;
                    of_for_priviledge.open(target_path.substr(2)+"/priviledge.txt", std::ios::out | std::ios::app); 
                    cout<<target_path.substr(2)+"/priviledge.txt"<<endl;
                    of_for_group.open(target_path.substr(2)+"/group.txt", std::ios::out | std::ios::app); 
                    cout<<target_path.substr(2)+"/group.txt"<<endl;
                    of_for_priviledge<<information[3]<<"\n";
                    for(int i = 0;i<PRI_LEN;i++){
                        of_for_priviledge<<user_P.flagset[i]<<"\n";
                    }
                }
                sql = "insert into users values('"+information[0]+"','"+information[1]+"','"+information[2]+"',"+information[3]+")";
                cout<<"query:"<<sql<<endl;
                mysql_query(&mysql,sql.c_str());
                memset(buf,0,BUFF_LEN);
                sprintf(buf,"%c%s%s",ROOT_OP_FEEDBACK,user_id.c_str(),TERMINAL_ID);
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                cout<<"done"<<endl;
                sleep(1);
                goto WAIT1;
            }
            else{
                mysql_free_result(result);
                memset(buf,0,BUFF_LEN);
                sprintf(buf,"%c%s%s",ROOT_OP_FEEDBACK,user_id.c_str(),TERMINAL_ID);
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                cout<<"back"<<endl;
                sleep(1);
                goto WAIT1;
            }

        }//right 
        else{
            memset(buf,0,BUFF_LEN);
            sprintf(buf,"%c%s%s",LOGIN_ROOT_PASSWORD_WRONG,user_id.c_str(),TERMINAL_ID);
            sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
            goto WAIT1;
        }//wrong

    }


    //----------
    int extra_input_value = 0;  //0 represent user didnt be asked to input an extra information, while 1 does
    string extra_output = "Extra information(input 'help' if need):\n";
    string options = "";
    string temp_buf = "";
    target_path = "./";
    bool error_detect = true;
    int steps = 0;
    while(user_P.pos.first!='0'){
        WAIT5:
        memset(buf,0,BUFF_LEN);
        recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        cout<<"receive applyment"<<endl;
        if(!isneed_accurate(ASK_FOR_OPTION_SHOW,TERMINAL_ID,user_id,buf)){
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                sleep(1);
                goto WAIT5;
        }
        if(extra_input_value==0){
            user_P.select();
            for(int i = 0;i<user_P.true_options.size();i++){
                options+= to_string(i+1)+". "+user_P.true_options[i]+'\n';
            }
            options+= "Please enter your choice(or system command):\n";
        }
        else{
            options += "Input:\n";
        }
        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s%c%d%s%c%s",ASK_FOR_OPTION_SHOW_FEEDBACK,user_id.c_str(),TERMINAL_ID,char(user_P.true_options.size()+48),extra_input_value,extra_output.c_str(),';',options.c_str());
        cout<<buf<<endl;
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);

        WAIT6:
        memset(buf,0,BUFF_LEN);
        recvfrom(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        if(!isneed_accurate(POS_UPDATE_ASK,TERMINAL_ID,user_id,buf)){
                sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
                sleep(1);
                goto WAIT6;
        }
        extra_output = "Extra information(input 'help' if need):\n"; //initialize extra_output
        options = "";
        //deal the code here, decode(buf) to get the code
        //write a function to deal the code and send pos.first + pos.second + decode(buf) in to the function to get an extra output and change pos
        vector<string*> basic_information;
        basic_information.push_back(&user_id);
        basic_information.push_back(&USER_ID);
        basic_information.push_back(&USER_NAME);
        basic_information.push_back(&USER_IDENTITY);
        basic_information.push_back(&USER_PASSWORD);
        //code-----------------
        if(extra_input_value==0){
            string judge_temp = decode(buf);
            if(judge_temp.size()==1){
                steps = 0;
                error_detect = true;
                string buf_temp = decode(buf);
                string str_to_ch = buf_temp.substr(0,1);
                temp_buf = user_P.pos.first+user_P.pos.second+user_P.search_option(str_to_ch);
                extra_output += DEAL_THE_CODE(user_P.pos.first+user_P.pos.second+user_P.search_option(decode(buf)),&user_P,&extra_input_value,&error_detect,&steps,"",basic_information,&target_path);
            }
            else{
                extra_output += DEAL_THE_CODE(temp_buf,&user_P,&extra_input_value,&error_detect,&steps,decode(buf),basic_information,&target_path);
            }
        }
        else{
            extra_output += DEAL_THE_CODE(temp_buf,&user_P,&extra_input_value,&error_detect,&steps,decode(buf),basic_information,&target_path);
        }
        //---------------------

        memset(buf,0,BUFF_LEN);
        sprintf(buf,"%c%s%s%c",POS_UPDATE_FEEDBACK,user_id.c_str(),TERMINAL_ID,user_P.pos.first);
        sendto(FD,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
        cout<<"pos message sends!"<<buf<<endl;
    }
    //user quit, change current quality of users and release the ip
    LOCK.lock();
    NUM--;
    cout<<"Current users:"<<NUM<<endl;
    IP_STATE[atoi(user_id.c_str())-10001] = false;
    ID_search.erase(USER_ID);
    cout<<"Who is on:"<<endl;
    cout<<"----------------"<<endl;
    for(auto&n : ID_search){
        cout<<n<<endl;
    }
    cout<<"----------------"<<endl;
    LOCK.unlock();
}
void handle_msg(int fd,struct sockaddr* dst){

    char buf[BUFF_LEN];
    socklen_t len;
    int count;
    int user = 0;
    struct sockaddr_in clent_addr;
    while(true){
        BEGINS:
        char buf_temp[BUFF_LEN];
        //Init
        memset(buf_temp,0,BUFF_LEN);
        memset(buf,0,BUFF_LEN);
        //Listening
        len = sizeof(clent_addr);
        count = recvfrom(fd,buf_temp,BUFF_LEN,0,(struct sockaddr*)&clent_addr,&len);
        if(count == -1){
            cout<<"Receive data error!"<<endl;
            return;
        }


        string str_temp(buf_temp);
        if(!isneed(LOGIN_CODE,TERMINAL_ID,str_temp)){
            //Not Log in
            sendto(fd,buf_temp,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
            goto BEGINS;
        }

        string str_temp2 = decode(str_temp);//Command,
        string str_user_code = get_region(str_temp);//userID

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
            //fail
            sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_FAIL,str_user_code.c_str(),TERMINAL_ID);
            sendto(fd,buf,BUFF_LEN,0,(struct sockaddr*)&clent_addr,len);
            sleep(1);
            goto BEGINS;
        }

        //success and send back
        true_ip = 10000+pointer+1;
        LOCK.lock();
        IP_STATE[pointer] = true;
        LOCK.unlock();
        // memset(buf,0,BUFF_LEN);
        // sprintf(buf,"%c%s%s",LOGIN_FEEDBACK_CODE_SUCCESS,str_user_code.c_str(),TERMINAL_ID);
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
    //vicky
    // if(mysql_real_connect(&mysql,"localhost","root","123456","Project",0,NULL,0) == NULL){
    if(mysql_real_connect(&mysql,"localhost","Manager","123456","OS",0,NULL,0) == NULL){
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