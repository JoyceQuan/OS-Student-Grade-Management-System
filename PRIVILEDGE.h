#ifndef PRIVILEDGE_H
#define PRIVILEDGE_H
//priviledge list
#include<fstream>
#include<iostream>
#include<sstream>
#include<algorithm>
#include<vector>
#define PRI_LEN 40
using namespace std;

class priviledge{
public:
    int flagset[PRI_LEN];
    int identity;//1-3:1:student,2:teacher,3:Administrator
    vector<string> true_options;
    pair<char,string>pos = make_pair('1',"");
    vector<char> index_to_option;
    static vector<string> priviledges;
    static vector<vector<int>> default_flagset;

    priviledge();
    priviledge(int);
    priviledge(bool,string);//read from a txt or set identity by a string

    string show_options(string);
    void select();
    void print_selections();
    void setdefault();
    bool ismeet(string,string);
    void set_pos(bool,char);
    void set(string);//set identity by a string 
    void set2(string);//set identity by a txt
    char search_option(string);
};

#endif
