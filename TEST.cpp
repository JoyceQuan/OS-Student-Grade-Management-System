#include<iostream>
#include<cstring>
#include<vector>
#include"PRIVILEDGE.h"
using namespace std;
int main(){
    cout<<"Please enter a relative path:"<<endl;
    string buf;
    getline(cin,buf);
    priviledge P = priviledge(true,buf);
    cout<<"IDENTITY:"<<P.identity<<endl;
    cout<<"FLAG:"<<endl;
    for(int i = 0;i<PRI_LEN;i++){
        cout<<P.flagset[i];
    }
    cout<<endl<<"start receiving"<<endl;
    cout<<"first:"<<P.pos.first<<" second:"<<P.pos.second<<" end"<<endl;

    GO:
    cout<<"flag"<<endl;
    P.select();
    P.print_selections();
    int get1;
    char get2;
    cin>>get1>>get2;
    if(get1==1){
        P.set_pos(true,get2);
    }
    else{
        P.set_pos(false,get2);
    }
    goto GO;
   

    /* while(P.pos.first!='0'){
        cout<<"1";
        P.select();
        cout<<"1";
        P.print_selections();
        cout<<"1";
        int get1;
        char get2;
        cin>>get1>>get2;
        if(get1==1){
            P.set_pos(true,get2);
        }
        else{
            P.set_pos(false,get2);
        }
    } */
}