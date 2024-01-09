
#ifndef inode
#define inode

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <map>
// #include <string>
#include <iomanip>
#include <cmath>
#include <bitset>
#include "txt.h"
#include "QUERY.h"

#define BLOCK_SIZE 508//Block的大小
#define MAX_FILENAME_LENGTH 12//fileName的长度最长为12
#define ENTRY_NUMBER 32//dirBlock中的项数为32
#define BLOCK_NUMBER 1120//存block的空间，一共有1120个block //精确是1117，但为了被8整除，增长到1120
#define INODE_NUMBER 10 //inodeList中inode数量为10个
#define MAX_INODELIST_NUMBER 124 //最大的inodelist数量
//#define MAX_INODE_NUMBER 1240 //最大的inode数量为1240个
//#define INODE_NUMBER 128//inode空间,一共有128个inode

struct DirectInode //20 bytes
{
    int inodeNumber;
    int blockID;
    int fileType;  // 0 represents directory, 1 represents normal file
    char privilege[4];
    int inodeType; //1 directInode   0 indirectInode
    DirectInode(int inodeNumber,int blockID,int fileType,int inodeType);
};

struct IndirectInode //20 bytes
{
    int inodeNumber;
    int pointLevel;
    int inodeListID;
    int inodeType;//1 directInode   0 indirectInode
    char nothing[4];
    IndirectInode(int inodeNumber,int pointLevel,int inodeListID,int inodeType);

};

struct InodeList //212 bytes
{
    int inodeListID;
    int full;
    int level;
    struct DirectInode inodeMem[INODE_NUMBER]; //20*10

    InodeList(int inodeListID=-1,int full=0,int level=-1);
    
    void init_inodeMem();      
};

struct DirectoryBlock //516 bytes
{
    int blockType;
    char fileName[ENTRY_NUMBER][MAX_FILENAME_LENGTH];
    int inodeID[ENTRY_NUMBER];
    DirectoryBlock();
    DirectoryBlock(char fileName[ENTRY_NUMBER][MAX_FILENAME_LENGTH],int inodeID[ENTRY_NUMBER]);
};

struct FileBlock  //516 bytes
{
    int blockType;
    char content[BLOCK_SIZE];
    int pointBlockNum;
    FileBlock();
    FileBlock(char content[BLOCK_SIZE]);
};

struct SuperBlock  
{
    int allfileSize; //4
    //struct InodeList inodeListMem[MAX_INODELIST_NUMBER];//inodelist空间,一共有124个inodelist   //212*124  //有层次结构，不需要用bitmap
    //struct FileBlock blockMem[BLOCK_NUMBER];//存block的空间，一共有128个block  //512*1120
    int inodeListMemByte;//4
    int blockMemByte; //4
    int max_usedInodeListNum;//4
    int max_usedBlockNum; //4
    char blockBitmap[BLOCK_NUMBER/8];//快速查找哪里有剩余 一字节8bit   //140
    std::unordered_map<int, int> inodeListid_to_MemIndex;
    SuperBlock();
    SuperBlock(char blockBitmap[BLOCK_NUMBER/8]);
};

//-------------------权限操作的函数---------------------
char Eight_Bit_To_Char(std::string eightBit);//string——>char
std::string Char_to_Eight_Bit(char c) ;//char-->string（为展示）
void build_Inode_Privilige(std::string A, std::string B, std::string C, char* PRI);//对inode设置权限
int check_Privilige(int n, int type, char* PRI);//检查某个权限表的指定位权限

//根据路径，检查指定位(n)、指定类型(type)的权限
//type:S0/T1/M2;  n:最左边0，最右边7
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int check_Privilige_by_Path(int n, int type,char path[100],int inodetype,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//根据路径，取出权限P
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void get_Privilige_by_Path(char path[100],int inodetype,std::vector<std::string>&P,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//根据路径，更改用户类下的权限
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void change_user_Privilige_byPath(std::unordered_map<std::string, char> &user_P,std::string new_privilige,char path[100]);





//---------------------文件系统的初始化函数---------------------
//文件不存在时，做初始化创建
//已有文件架构时，实现向superBlock、inodeListMem、blockMem的赋值
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
bool init_inode(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);



//---------------------inode与inodeList间的操作------------------------
std::array<int, 2> inodeID_to_inodeListID(int inodeID);

int iListID_plus_offset_to_inodeID(int inodelistID,int offset);



//-------------------更改mem引发的文件写操作-----------------------------
void write_Block_To_File(SuperBlock superBlock,struct FileBlock blockMem[BLOCK_NUMBER],int blockID);

void write_Inode_To_File(SuperBlock superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],int Mem_index,int offset);

void write_WholeInode_To_File(SuperBlock superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],int Mem_index);

void write_SuperBlock_To_File(SuperBlock superBlock);




//-------------------不涉及备份操作的文件系统操作----------------------

//创建文件夹  path:文件夹路径  outputString:输出内容(用户端可以直接cout)   P:权限(3个8位string)
void createDirectory(char path[100],std::string &outputString ,std::vector<std::string>P,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//列举文件夹下内容%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void listFiles(char path[100],std::string &outputString ,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//只列举文件夹下的文件名（没用到）（但其实没用，因为要限制文件类型，如果需要，可以将文件类型一起输出，然后分割字符判断）
void ls_only_name(char path[100],std::string& outputString,int f_type,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//创建文件
void createFile(char path[100],std::string &outputString ,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//删除文件
void deleteFile(char path[100],std::string &outputString ,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//读文件%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void readFile(char path[100],std::string &outputString ,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]) ;

//写文件  inputLine:写入的文件内容
void writeFile(char path[100],std::string &outputString ,std::string inputLine,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//子操作：删无子文件的文件夹
struct DirectoryBlock * deleteSingleDir(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],struct DirectoryBlock *block,char path[100]);

//子操作：删文件
void deleteSingleFile(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],char path[100]);

//子操作：深度优先搜索
struct DirectoryBlock * DFS(std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],struct DirectoryBlock * block,char parent[100]);

//删除文件夹
void deleteDirectory(char path[100],std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);





//-----------------------inode拓展的相关操作--------------------

//拓展到新的inodeList后，对其初始化
void init_inodelist(struct InodeList inodeListMem[MAX_INODELIST_NUMBER],int startPos,int pointLevel,int inodelistID);

//拓展到新的indirect的inodeList后，对其初始化
void init_Indirect_inodelist(struct InodeList inodeListMem[MAX_INODELIST_NUMBER],int startPos,int pointLevel,int inodelistID);

//将inodeA信息拷贝到inodeB
void A_copyto_B(struct InodeList inodeListMem[MAX_INODELIST_NUMBER],int A_Pos,int B_memPos);

//改变inode的类型
void change_inode_type(struct InodeList inodeListMem[MAX_INODELIST_NUMBER],int memPos,int offset,int pointLevel,int inodeID);

//初始化inode的相关内容
void init_inodeList_and_inode(SuperBlock superblock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER], int tobe_level);

//inode拓展的主函数
void inodeExtend(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);





//---------------------------备份操作-----------------------------
void hash_to_file(std::map<std::string, int> hash,char path[100],SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

void file_to_hash(std::map<std::string, int> &hash,char path[100],SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

void string_to_char100(std::string s,char c[100]);

void copy_file(char source[100],char target[100],SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

void copy_folder(char source[100],char target[100],std::vector<std::string>P,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

bool detect_if_copy(char path[100]);

bool path_exist(char path[100],int f_type,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

void writeLog(char path[100],int operation,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

void writeLog_for_DB(int operation,int ID,std::string password,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

void Copy_Operation(char path[100],int f_type,int operation,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

void Copy_Operation_For_Create(char path[100],int f_type,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//开启备份的函数；备份内容从此清零，并重新记录
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void start_copy(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

void readLog(std::vector<std::vector<std::string>>&log,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

std::string getCopyFile(char path[100],int f_type,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

void deleteUser_in_DB();
void recoverPassword_in_DB();

//备份恢复函数
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void recover_From_Log(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],MYSQL* mysql);





//---------------------考虑备份后的文件系统操作--------------------------------
//创建文件夹%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void CreateD(char path[100],std::string &outputString ,std::vector<std::string>P,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//创建文件%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void CreateF(char path[100],std::string &outputString ,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//删除文件%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void DeleteF(char path[100],std::string &outputString ,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//写文件%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void WriteF(char path[100],std::string &outputString,std::string inputLine ,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

//删除无子文件的文件夹
struct DirectoryBlock * DeleteSingleD(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],struct DirectoryBlock *block,char path[100]);
//删除文件
void DeleteSingleF(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],char path[100]);
//深度优先搜索
struct DirectoryBlock * DFS_notCopy(std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],struct DirectoryBlock * block,char parent[100]);
//删除文件夹(不涉及备份)
void DeleteDir(char path[100],std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]) ;

//删除文件夹%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void DeleteD(char path[100],std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]);

#endif