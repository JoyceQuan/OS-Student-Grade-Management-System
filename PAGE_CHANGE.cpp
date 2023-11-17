#include"PAGE_CHANGE.h"
using namespace std;
void NEXT(priviledge* p, string code){
    p->set_pos(true,code[code.size()-1]);
}
void PREVIOUS(priviledge* p){
    p->set_pos(false,'0');
}