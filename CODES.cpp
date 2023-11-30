#include"CODES.h"
using namespace std;


void sleep(unsigned int time){
    struct timeval tv;
    tv.tv_sec = time;
    tv.tv_usec = 0;
    select(0,NULL,NULL,NULL,&tv);

}

bool isneed(char code, string target,string buf){
    if(buf[0]!=code){
        return false;
    }
    if(target!=buf.substr(1,5)){//接收端是否为target
        return false;
    }
    return true;
}

bool isneed_accurate(char code, string target, string origin,string buf){
    if(buf[0]!=code){
        return false;
    }
    if(target!=buf.substr(1,5)){
        return false;
    }
    if(origin!=buf.substr(6,5)){
        return false;
    }
    return true;
}
string get_region(string buf){
    return buf.substr(6,5);
}
string decode(string buf){
    return buf.substr(11);
}
string RE_TO_SEND(string buf){
    return buf[0] + buf.substr(6,5) + buf.substr(1,5) + buf.substr(11);
}

pair<string,pair<string,string>> decode_options(string buf){
    pair<string,pair<string,string>>output;
    output.first = buf.substr(0,2);
    buf = buf.substr(2);
    int index = 0;
    for(int i = 0;i<buf.size();i++){
        if(buf[i]==';'){
            index=i;
            break;
        }
    }
    //cout<<"debug---"<<endl;
    //cout<<buf.substr(0,index)<<endl;
    //cout<<buf.substr(index+1)<<endl;
    //cout<<"--------"<<endl;
    output.second = make_pair(buf.substr(0,index),buf.substr(index+1));
    return output;
}
string FORMAT(string target, int len){
    string output(len,' ');
    for(int i = 0;i<len;i++){
        if(i<target.size()){
            output[i] = target[i];
        }
    }
    return output;
}