// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unordered_map>
// #include <vector>
// #include "txt.h"

// #define BLOCK_SIZE 512//Block的大小
// #define MAX_FILENAME_LENGTH 12//fileName的长度最长为12
// #define ENTRY_NUMBER 32//dirBlock中的项数为32
// #define BLOCK_NUMBER 1120//存block的空间，一共有1120个block //精确是1117，但为了被8整除，增长到1120
// #define INODE_NUMBER 10 //初始的inode数量为10个
// #define MAX_INODELIST_NUMBER 124 //最大的inodelist数量
// //#define MAX_INODE_NUMBER 1240 //最大的inode数量为1240个
// //#define INODE_NUMBER 128//inode空间,一共有128个inode
#include "inode.h"


DirectInode::DirectInode(int inodeNumber=-1,int blockID=-1,int fileType=-1,int inodeType=1):
    inodeNumber(inodeNumber), blockID(blockID), fileType(fileType),inodeType(inodeType){
        std::fill(std::begin(privilege), std::end(privilege), 0);
    }


IndirectInode::IndirectInode(int inodeNumber=-1,int pointLevel=-1,int inodeListID=-1,int inodeType=0)
    :inodeNumber(inodeNumber),pointLevel(pointLevel),inodeListID(inodeListID),inodeType(inodeType){
        std::fill(std::begin(nothing), std::end(nothing), 0);
    }



// InodeList::InodeList(int inodeListID=-1,int full=0,int level=-1)
InodeList::InodeList(int inodeListID,int full,int level)
    :inodeListID(inodeListID),full(full),level(level){
        for (int i = 0; i < INODE_NUMBER; ++i) {
            this->inodeMem[i] = DirectInode(i+inodeListID*10);
        }
    }

void InodeList::init_inodeMem(){
    for (int i = 0; i < INODE_NUMBER; ++i) {
        this->inodeMem[i] = DirectInode(i+inodeListID*10);
    }
}



DirectoryBlock::DirectoryBlock(){
    for (int i = 0; i < ENTRY_NUMBER; ++i) {
        std::fill(std::begin(fileName[i]), std::end(fileName[i]), 0);
        this->inodeID[i] = -1;
    }
    blockType=0;
}

DirectoryBlock::DirectoryBlock(char fileName[ENTRY_NUMBER][MAX_FILENAME_LENGTH],int inodeID[ENTRY_NUMBER]){
    for (int i = 0; i < ENTRY_NUMBER; ++i) {
        std::strcpy(this->fileName[i], fileName[i]);
        this->inodeID[i] = inodeID[i];
    }
    blockType=0;
}



FileBlock::FileBlock(){
    this->pointBlockNum=-1;
    std::fill(std::begin(content), std::end(content), 0);
    blockType=1;
}
FileBlock::FileBlock(char content[BLOCK_SIZE]){
    this->pointBlockNum=-1;
    std::strcpy(this->content, content);
    blockType=1;
}


SuperBlock::SuperBlock()
{   
    blockBitmap[0] |= 1;
    inodeListMemByte=sizeof(struct InodeList)*MAX_INODELIST_NUMBER;
    blockMemByte=sizeof(struct DirectoryBlock)*BLOCK_NUMBER; 
    allfileSize=sizeof(struct SuperBlock)+this->inodeListMemByte+this->blockMemByte;
    max_usedInodeListNum=1;
    max_usedBlockNum=1;
    std::strcpy(this->blockBitmap, blockBitmap);

    std::vector<int> keys = {0, 1, 2};
    std::vector<int> values;
    size_t i=0;
    for (i = 20; i <= 29; ++i) {
        keys.push_back(i);
    }
    keys.push_back(3);
    for ( i = 30; i <= 39; ++i) {
        keys.push_back(i);
    }
    for ( i = 300; i <= 399; ++i) {
        keys.push_back(i);
    }
    for ( i = 0; i <= 123; ++i) {
        values.push_back(i);
    }

    for ( i = 0; i < keys.size(); ++i) {
        inodeListid_to_MemIndex[keys[i]] = values[i];
    }
}
SuperBlock::SuperBlock(char blockBitmap[BLOCK_NUMBER/8])
{   
    inodeListMemByte=sizeof(struct InodeList)*MAX_INODELIST_NUMBER;
    blockMemByte=sizeof(struct DirectoryBlock)*BLOCK_NUMBER; 
    allfileSize=sizeof(struct SuperBlock)+this->inodeListMemByte+this->blockMemByte;
    max_usedInodeListNum=1;
    max_usedBlockNum=1;
    std::strcpy(this->blockBitmap, blockBitmap);

    std::vector<int> keys = {0, 1, 2};
    std::vector<int> values;
    size_t i=0;
    for (i = 20; i <= 29; ++i) {
        keys.push_back(i);
    }
    keys.push_back(3);
    for ( i = 30; i <= 39; ++i) {
       keys.push_back(i);
    }
    for ( i = 300; i <= 399; ++i) {
        keys.push_back(i);
    }
    for ( i = 0; i <= 123; ++i) {
        values.push_back(i);
    }

    for ( i = 0; i < keys.size(); ++i) {
        inodeListid_to_MemIndex[keys[i]] = values[i];
    }
}



char Eight_Bit_To_Char(std::string eightBit) {
	int SUM = 0;
	for (int i = 0; i < eightBit.size(); i++) {
		SUM += ((eightBit[i] == '0') ? 0 : pow(2, eightBit.size() - i - 1));
	}
	return SUM;
}
std::string Char_to_Eight_Bit(char c) {
    return std::bitset<8>(c).to_string();
}
void build_Inode_Privilige(std::string A, std::string B, std::string C, char* PRI) {
	PRI[0] = Eight_Bit_To_Char(A);
	PRI[1] = Eight_Bit_To_Char(B);
	PRI[2] = Eight_Bit_To_Char(C);
}
int check_Privilige(int n, int type, char* PRI) {
	int temp = int(PRI[type]);
	return (temp >> (7 - n)) & 1;
}


int check_Privilige_by_Path(int n, int type,char path[100],int inodetype,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    char path_temp[100]="";
    strcpy(path_temp,path);
    int i, j, flag;
    //char path[100];
    char *directory, *parent, *target;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针
    char *blockBitmap=&superBlock.blockBitmap[0];

    std::unordered_map<int, int> inodeListid_to_MemIndex=superBlock.inodeListid_to_MemIndex;

    // Access the pointer of parent directory recursively
    parent = NULL;
    directory = strtok(path, delimiter);
    // printf("AAAdirectory%s\n",directory);

    //找到上一级目录的位置
    while (directory != NULL) {
        if (parent != NULL) //上一级不是根目录
        {
            block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
            flag = 0;
            for (i = 0; i < ENTRY_NUMBER; i++) { 
                if (strcmp(block->fileName[i], parent) == 0) //找到父目录的位置
                {   
                    int inode_temp=block->inodeID[i];
                    int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                    int offset=inodeID_to_inodeListID(inode_temp)[0];
                    int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
                    if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                        flag=1;
                        pointer=&inodeListMem[Mem_index].inodeMem[offset];
                        break;
                    }
                                    
                }
            }
            if (flag == 0) {
                // If the parent directory does not exist or is not a directory file
                //printf("The path does not exist!\n");

                strcpy(path,path_temp);
                return 1;//先允许进入，到下一层再处理，输出错误信息
            }
        }
        parent = directory;
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }
    //最后出来的时候，要创建的目录就是parent；point指向父目录

    // Create the target directory
    int entry = -1;//块内第几项
    int n_block = -1;//第几块放置
    int n_inode = -1;//inode#
    int bool_int=1;
    target = parent;
    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//父block
    //找到对应n_inode
    for (i = 0; i < ENTRY_NUMBER; i++) {
        if(strcmp(block->fileName[i], target) == 0){
            n_inode=block->inodeID[i];
            int inodeListID=inodeID_to_inodeListID(n_inode)[1];
            int offset=inodeID_to_inodeListID(n_inode)[0];
            int Mem_index=inodeListid_to_MemIndex[inodeListID];
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==inodetype){
                bool_int=check_Privilige(n,type,inodeListMem[Mem_index].inodeMem[offset].privilege);
                std::cout<<"bool_int:"<<bool_int<<std::endl;
            }else{
                std::cout<<"Filetype don't match!"<<std::endl;
                bool_int=0;//找错文件类型这种情况会置为0
            }

        }
    }

    strcpy(path,path_temp);
    return bool_int;//如果没找到，还是置为1，把他放进下一个函数里，输出错误信息。
}

void change_user_Privilige_byPath(std::unordered_map<std::string, char> &user_P,std::string new_privilige,char path[100]){
    std::string pathStr(path);
    user_P[pathStr]=Eight_Bit_To_Char(new_privilige);
}

void get_Privilige_by_Path(char path[100],int inodetype,std::vector<std::string>&P,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    char path_temp[100]="";
    strcpy(path_temp,path);
    int i, j, flag;
    //char path[100];
    char *directory, *parent, *target;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针
    char *blockBitmap=&superBlock.blockBitmap[0];

    std::unordered_map<int, int> inodeListid_to_MemIndex=superBlock.inodeListid_to_MemIndex;

    // Access the pointer of parent directory recursively
    parent = NULL;
    directory = strtok(path, delimiter);
    // printf("AAAdirectory%s\n",directory);

    //找到上一级目录的位置
    while (directory != NULL) {
        if (parent != NULL) //上一级不是根目录
        {
            block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
            flag = 0;
            for (i = 0; i < ENTRY_NUMBER; i++) { 
                if (strcmp(block->fileName[i], parent) == 0) //找到父目录的位置
                {   
                    int inode_temp=block->inodeID[i];
                    int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                    int offset=inodeID_to_inodeListID(inode_temp)[0];
                    int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
                    if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                        flag=1;
                        pointer=&inodeListMem[Mem_index].inodeMem[offset];
                        break;
                    }
                                    
                }
            }
            if (flag == 0) {
                // If the parent directory does not exist or is not a directory file
                //printf("The path does not exist!\n");

                strcpy(path,path_temp);
                return ;//先允许进入，到下一层再处理，输出错误信息
            }
        }
        parent = directory;
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }
    //最后出来的时候，要创建的目录就是parent；point指向父目录

    // Create the target directory
    int entry = -1;//块内第几项
    int n_block = -1;//第几块放置
    int n_inode = -1;//inode#
    int bool_int=1;
    target = parent;
    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//父block
    //找到对应n_inode
    for (i = 0; i < ENTRY_NUMBER; i++) {
        if(strcmp(block->fileName[i], target) == 0){
            n_inode=block->inodeID[i];
            int inodeListID=inodeID_to_inodeListID(n_inode)[1];
            int offset=inodeID_to_inodeListID(n_inode)[0];
            int Mem_index=inodeListid_to_MemIndex[inodeListID];
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==inodetype){
                P[0]=Char_to_Eight_Bit(inodeListMem[Mem_index].inodeMem[offset].privilege[0]);
                P[1]=Char_to_Eight_Bit(inodeListMem[Mem_index].inodeMem[offset].privilege[1]);
                P[2]=Char_to_Eight_Bit(inodeListMem[Mem_index].inodeMem[offset].privilege[2]);

                strcpy(path,path_temp);
                return;
            }else{
                std::cout<<"Filetype don't match!"<<std::endl;
            }

        }
    }
    strcpy(path,path_temp);
    return ;//如果没找到，还是置为1，把他放进下一个函数里，输出错误信息。
}

bool init_inode(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    // struct InodeList inodeListMem[MAX_INODELIST_NUMBER];
    std::string filename="disk.txt";
    std::streampos FileSize =superBlock.allfileSize;
    int pos=0;
    if(!init_file(filename,FileSize)) return false;
    if(isSuperBlockEmpty(filename,sizeof(struct SuperBlock)))//super为空，写入文件,要实现根目录的初始化
    {
        writeIntAtPosition(filename, superBlock.allfileSize,pos);
        pos+=sizeof(superBlock.allfileSize);
        writeIntAtPosition(filename, superBlock.inodeListMemByte,pos);
        pos+=sizeof(superBlock.inodeListMemByte);
        writeIntAtPosition(filename, superBlock.blockMemByte, pos);
        pos+=sizeof(superBlock.blockMemByte);
        writeIntAtPosition(filename, superBlock.max_usedInodeListNum, pos);
        pos+=sizeof(superBlock.max_usedInodeListNum);
        writeIntAtPosition(filename, superBlock.max_usedBlockNum,pos);
        pos+=sizeof(superBlock.max_usedBlockNum);
        writeByteAtPosition(filename, pos, superBlock.blockBitmap,sizeof(superBlock.blockBitmap));

        InodeList I_list_init(0,0,0);
        int inodeListMem_start_pos=sizeof(struct SuperBlock);
        writeIntAtPosition(filename, I_list_init.inodeListID,inodeListMem_start_pos);
        writeIntAtPosition(filename, I_list_init.full,inodeListMem_start_pos+sizeof(I_list_init.inodeListID));
        writeIntAtPosition(filename, I_list_init.level,inodeListMem_start_pos+sizeof(I_list_init.inodeListID)+sizeof(I_list_init.full) );
        pos=inodeListMem_start_pos+sizeof(I_list_init.inodeListID)+sizeof(I_list_init.full)+sizeof(I_list_init.level);
        for(int i=0;i<INODE_NUMBER;++i){
            if(i==0){
                writeIntAtPosition(filename,0,pos);
                writeIntAtPosition(filename,0,pos+sizeof(int)*1);
                writeIntAtPosition(filename,0,pos+sizeof(int)*2);
                char p[3];
                build_Inode_Privilige("00000000","00000000","11111110",p);
                writeByteAtPosition(filename, pos+sizeof(int)*3, p,sizeof(p));
                // writeIntAtPosition(filename,I_list_init.inodeMem[i].privilege,pos+sizeof(int)*3);
                writeIntAtPosition(filename,0,pos+sizeof(int)*3+sizeof(p));
            }
            else{
                writeIntAtPosition(filename,I_list_init.inodeMem[i].inodeNumber,pos);
                writeIntAtPosition(filename,I_list_init.inodeMem[i].blockID,pos+sizeof(int)*1);
                writeIntAtPosition(filename,I_list_init.inodeMem[i].fileType,pos+sizeof(int)*2);
                writeByteAtPosition(filename, pos+sizeof(int)*3, I_list_init.inodeMem[i].privilege,sizeof(I_list_init.inodeMem[i].privilege));
                // writeIntAtPosition(filename,I_list_init.inodeMem[i].privilege,pos+sizeof(int)*3);
                writeIntAtPosition(filename,I_list_init.inodeMem[i].inodeType,pos+sizeof(int)*3+sizeof(I_list_init.inodeMem[i].privilege));
            }
            pos=pos+sizeof(int)*4+sizeof(I_list_init.inodeMem[i].privilege);
        }    

        int blockMem_start_pos=inodeListMem_start_pos+superBlock.inodeListMemByte;
        writeIntAtPosition(filename,0,blockMem_start_pos);
        pos=blockMem_start_pos+sizeof(int);
        for(int i=0;i<ENTRY_NUMBER;++i){
            char null[MAX_FILENAME_LENGTH]={'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
            writeByteAtPosition(filename,pos,null,sizeof(char[MAX_FILENAME_LENGTH]));
            writeIntAtPosition(filename,-1,pos+sizeof(char[MAX_FILENAME_LENGTH]));
            pos=pos+sizeof(char[MAX_FILENAME_LENGTH])+sizeof(int);
        }

    }   
    //从文件写入superBlock,inodeListMem,blockMem
    {
        pos=0;
        //读superBlock
        readIntFromPosition(filename, superBlock.allfileSize,pos);
        pos+=sizeof(superBlock.allfileSize);
        readIntFromPosition(filename, superBlock.inodeListMemByte,pos);
        pos+=sizeof(superBlock.inodeListMemByte);
        readIntFromPosition(filename, superBlock.blockMemByte, pos);
        pos+=sizeof(superBlock.blockMemByte);
        readIntFromPosition(filename, superBlock.max_usedInodeListNum, pos);
        pos+=sizeof(superBlock.max_usedInodeListNum);
        readIntFromPosition(filename, superBlock.max_usedBlockNum,pos);
        pos+=sizeof(superBlock.max_usedBlockNum);
        readBytesFromPosition(filename, pos, superBlock.blockBitmap,sizeof(superBlock.blockBitmap));
        // readIntFromPosition(filename, superBlock.allfileSize,0);
        // readIntFromPosition(filename, superBlock.inodeListMemByte,sizeof(superBlock.allfileSize));
        // readIntFromPosition(filename, superBlock.blockMemByte, sizeof(superBlock.inodeListMemByte)+sizeof(superBlock.allfileSize));
        // readIntFromPosition(filename, superBlock.max_usedInodeListNum, sizeof(superBlock.blockMemByte)+sizeof(superBlock.inodeListMemByte)+sizeof(superBlock.allfileSize));
        // readIntFromPosition(filename, superBlock.max_usedBlockNum,sizeof(superBlock.max_usedInodeListNum)+sizeof(superBlock.blockMemByte)+sizeof(superBlock.inodeListMemByte)+sizeof(superBlock.allfileSize));
        // readBytesFromPosition(filename,  sizeof(superBlock.max_usedInodeListNum)+sizeof(superBlock.blockMemByte)+sizeof(superBlock.inodeListMemByte)+sizeof(superBlock.allfileSize)+sizeof(superBlock.max_usedBlockNum),(superBlock.blockBitmap), sizeof(superBlock.blockBitmap));


        //读inodeListMem
        int inodeListMem_start_pos=sizeof(struct SuperBlock);
        pos=inodeListMem_start_pos;
        for(int i_l=0;i_l<superBlock.max_usedInodeListNum;++i_l){
            InodeList I_list_temp;
            readIntFromPosition(filename, I_list_temp.inodeListID,pos);
            readIntFromPosition(filename, I_list_temp.full,pos+sizeof(I_list_temp.inodeListID));
            readIntFromPosition(filename, I_list_temp.level,pos+sizeof(I_list_temp.inodeListID)+sizeof(I_list_temp.full) );
            pos=pos+sizeof(I_list_temp.inodeListID)+sizeof(I_list_temp.full)+sizeof(I_list_temp.level);
            for(int i=0;i<INODE_NUMBER;++i){
                readIntFromPosition(filename,I_list_temp.inodeMem[i].inodeNumber,pos);
                readIntFromPosition(filename,I_list_temp.inodeMem[i].blockID,pos+sizeof(int)*1);
                readIntFromPosition(filename,I_list_temp.inodeMem[i].fileType,pos+sizeof(int)*2);
                readBytesFromPosition(filename, pos+sizeof(int)*3, I_list_temp.inodeMem[i].privilege,sizeof(I_list_temp.inodeMem[i].privilege));
                //readIntFromPosition(filename,I_list_temp.inodeMem[i].privilege,pos+sizeof(int)*3);
                readIntFromPosition(filename,I_list_temp.inodeMem[i].inodeType,pos+sizeof(int)*3+sizeof(I_list_temp.inodeMem[i].privilege));
                pos=pos+sizeof(int)*4+sizeof(I_list_temp.inodeMem[i].privilege);
            }
            inodeListMem[i_l]=I_list_temp;
        }

        //读blockMem
        int blockMem_start_pos=inodeListMem_start_pos+superBlock.inodeListMemByte;
        pos=blockMem_start_pos;
        for(int i_b=0;i_b<superBlock.max_usedBlockNum;++i_b){
            int type;
            readIntFromPosition(filename, type,pos);
            pos=pos+sizeof(int); //578136-->578140
            if(type==0){
                DirectoryBlock *dir=(struct DirectoryBlock*)&blockMem[i_b];
                dir->blockType=type;
                for(int i=0;i<ENTRY_NUMBER;++i){
                    readBytesFromPosition(filename,pos,dir->fileName[i],sizeof(dir->fileName[i]));
                    readIntFromPosition(filename,dir->inodeID[i],pos+sizeof(dir->fileName[i]));
                    pos=pos+sizeof(dir->fileName[i])+sizeof(dir->inodeID[i]);
                }
                // blockMem[i_b]=(struct FileBlock*)dir;//像上面第一句那样写
            }
            else if(type==1){
                FileBlock* file=(struct FileBlock*)&blockMem[i_b];;
                file->blockType=type;
                readBytesFromPosition(filename,pos,file->content,sizeof(file->content));
                readIntFromPosition(filename,file->pointBlockNum,pos+sizeof(file->content));    
                pos=pos+sizeof(file->content)+sizeof(int);
            }
            
        }


    }

    return true;

}

std::array<int, 2> inodeID_to_inodeListID(int inodeID){
    int n=inodeID%10;
    int inodeListID=(inodeID-n)/10;
    
    return {n,inodeListID};
}

int iListID_plus_offset_to_inodeID(int inodelistID,int offset){
    return inodelistID*10+offset;
}


void write_Block_To_File(SuperBlock superBlock,struct FileBlock blockMem[BLOCK_NUMBER],int blockID){
    std::string filename="disk.txt";
    int pos=sizeof(struct SuperBlock)+superBlock.inodeListMemByte+blockID*sizeof(struct FileBlock);
    int type=blockMem[blockID].blockType;
    if (type==0){
        DirectoryBlock *block=(struct DirectoryBlock*)&blockMem[blockID];
        writeIntAtPosition(filename,type,pos);
        pos=pos+sizeof(int);
        for(int i=0;i<ENTRY_NUMBER;++i){
            writeByteAtPosition(filename,pos,block->fileName[i],sizeof(char[MAX_FILENAME_LENGTH]));
            writeIntAtPosition(filename,block->inodeID[i],pos+sizeof(char[MAX_FILENAME_LENGTH]));
            pos=pos+sizeof(char[MAX_FILENAME_LENGTH])+sizeof(int);
        }
    }
    else if(type==1){
        writeIntAtPosition(filename,type,pos);
        writeByteAtPosition(filename,pos+sizeof(int),blockMem[blockID].content,sizeof(blockMem[blockID].content));
        writeIntAtPosition(filename,blockMem[blockID].pointBlockNum,pos+sizeof(int)+sizeof(blockMem[blockID].content));   
        pos=pos+sizeof(int)+sizeof(blockMem[blockID].content)+sizeof(int);
    }
}

void write_Inode_To_File(SuperBlock superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],int Mem_index,int offset){
    std::string filename="disk.txt";
    InodeList inodelist=inodeListMem[Mem_index];
    int pos=Mem_index*sizeof(struct InodeList)+sizeof(superBlock);
    writeIntAtPosition(filename, inodelist.inodeListID,pos);
    writeIntAtPosition(filename, inodelist.full,pos+sizeof(inodelist.inodeListID));
    writeIntAtPosition(filename, inodelist.level,pos+sizeof(inodelist.inodeListID)+sizeof(inodelist.full) );
    pos=pos+sizeof(inodelist.inodeListID)+sizeof(inodelist.full)+sizeof(inodelist.level)+sizeof(inodelist.inodeMem[offset])*offset;

    writeIntAtPosition(filename,inodelist.inodeMem[offset].inodeNumber,pos);
    writeIntAtPosition(filename,inodelist.inodeMem[offset].blockID,pos+sizeof(int)*1);
    writeIntAtPosition(filename,inodelist.inodeMem[offset].fileType,pos+sizeof(int)*2);
    writeByteAtPosition(filename,pos+sizeof(int)*3, inodelist.inodeMem[offset].privilege,sizeof(inodelist.inodeMem[offset].privilege));
    //writeIntAtPosition(filename,inodelist.inodeMem[offset].privilege,pos+sizeof(int)*3);
    writeIntAtPosition(filename,inodelist.inodeMem[offset].inodeType,pos+sizeof(int)*3+sizeof(inodelist.inodeMem[offset].privilege));

}

void write_WholeInode_To_File(SuperBlock superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],int Mem_index){
    std::string filename="disk.txt";
    InodeList inodelist=inodeListMem[Mem_index];
    int pos=Mem_index*sizeof(struct InodeList)+sizeof(superBlock);
    writeIntAtPosition(filename, inodelist.inodeListID,pos);
    writeIntAtPosition(filename, inodelist.full,pos+sizeof(inodelist.inodeListID));
    writeIntAtPosition(filename, inodelist.level,pos+sizeof(inodelist.inodeListID)+sizeof(inodelist.full) );
    pos=pos+sizeof(inodelist.inodeListID)+sizeof(inodelist.full)+sizeof(inodelist.level);

    for(int offset=0;offset<INODE_NUMBER;++offset){
        writeIntAtPosition(filename,inodelist.inodeMem[offset].inodeNumber,pos);
        writeIntAtPosition(filename,inodelist.inodeMem[offset].blockID,pos+sizeof(int)*1);
        writeIntAtPosition(filename,inodelist.inodeMem[offset].fileType,pos+sizeof(int)*2);
        writeByteAtPosition(filename,pos+sizeof(int)*3, inodelist.inodeMem[offset].privilege,sizeof(inodelist.inodeMem[offset].privilege));
        // writeIntAtPosition(filename,inodelist.inodeMem[offset].privilege,pos+sizeof(int)*3);
        writeIntAtPosition(filename,inodelist.inodeMem[offset].inodeType,pos+sizeof(int)*3+sizeof(inodelist.inodeMem[offset].privilege));
        pos=pos+sizeof(int)*4+sizeof(inodelist.inodeMem[offset].privilege);
    }
    

}

void write_SuperBlock_To_File(SuperBlock superBlock){
    std::string filename="disk.txt";
    writeIntAtPosition(filename, superBlock.max_usedInodeListNum, sizeof(superBlock.blockMemByte)+sizeof(superBlock.inodeListMemByte)+sizeof(superBlock.allfileSize));
    writeIntAtPosition(filename, superBlock.max_usedBlockNum,sizeof(superBlock.max_usedInodeListNum)+sizeof(superBlock.blockMemByte)+sizeof(superBlock.inodeListMemByte)+sizeof(superBlock.allfileSize));
    writeByteAtPosition(filename, sizeof(superBlock.max_usedInodeListNum)+sizeof(superBlock.blockMemByte)+sizeof(superBlock.inodeListMemByte)+sizeof(superBlock.allfileSize)+sizeof(superBlock.max_usedBlockNum), superBlock.blockBitmap,sizeof(superBlock.blockBitmap));
}





void createDirectory(char path[100],std::string &outputString ,std::vector<std::string>P, SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]) {
    inodeExtend(superBlock,inodeListMem,blockMem);
    char path_temp[100]="";
    strcpy(path_temp,path);
    std::ostringstream output;
    int i, j, flag;
    //char path[100];
    char *directory, *parent, *target;
    const char delimiter[2] = "/";
    //struct Inode *pointer = inodeMem;//指向inode的指针
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针
    char *blockBitmap=&superBlock.blockBitmap[0];

    std::unordered_map<int, int> inodeListid_to_MemIndex=superBlock.inodeListid_to_MemIndex;

    // printf("Input The Path:\n");
    // scanf("%s", path);//绝对路径

    // Access the pointer of parent directory recursively
    parent = NULL;
    directory = strtok(path, delimiter);
    // printf("AAAdirectory%s\n",directory);
    if (directory == NULL)//根目录
    {
        output<<"It is the root directory!\n";
        //printf("It is the root directory!\n");
        outputString = output.str();
        strcpy(path,path_temp);
        return;
    }

    //找到上一级目录的位置
    while (directory != NULL) {
        if (parent != NULL) //上一级不是根目录
        {
            block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
            flag = 0;
            for (i = 0; i < ENTRY_NUMBER; i++) { 
                if (strcmp(block->fileName[i], parent) == 0) //找到父目录的位置
                {   
                    int inode_temp=block->inodeID[i];
                    int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                    int offset=inodeID_to_inodeListID(inode_temp)[0];
                    int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
                    if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                        flag=1;
                        pointer=&inodeListMem[Mem_index].inodeMem[offset];
                        break;
                    }
                                    
                }
            }
            if (flag == 0) {
                // If the parent directory does not exist or is not a directory file
                output<<"The path does not exist!\n";
                outputString = output.str();
                //printf("The path does not exist!\n");
                strcpy(path,path_temp);
                return;
            }
        }
        parent = directory;
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }
    //最后出来的时候，要创建的目录就是parent；point指向父目录


    
    // Create the target directory
    int entry = -1;//块内第几项
    int n_block = -1;//选择了第几块来放置
    int n_inode = -1;//选择的inode#
    int n_inodeList=-1;//选择的inodeList#
    target = parent;
    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//父block
    int parent_blockID=pointer->blockID;
    

    for (i = 0; i < ENTRY_NUMBER; i++) {
        if (strcmp(block->fileName[i], target) == 0) {
            int inode_temp=block->inodeID[i];
            int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
            int n=inodeID_to_inodeListID(inode_temp)[0];
            int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
            if(inodeListMem[Mem_index].inodeMem[n].fileType==0){
                //父目录里已经有这个文件了
                output<<"The file already exists!\n";
                outputString = output.str();
                //printf("The file already exists!\n");
                strcpy(path,path_temp);
                return;
            }
        }

        if (block->inodeID[i] == -1) {
            entry = i;//将父目录的第i项放置该文件夹
            break;
        }
    }
    if (entry >= 0) {
        // Find a unused block
        for (i = 0; i < BLOCK_NUMBER/8; i++) {
            for (j = 0; j < 8; j++) {
                if ((blockBitmap[i] & (1 << j)) == 0) //第i块对应位图第i位，检查 blockBitmap[i] 中的第 j 位是否为0
                {
                    n_block = i * 8 + j;
                    break;
                }
            }
            if (n_block != -1) {
                break;
            }
        }
        if (n_block == -1) {
            output<<"The block is full!\n";
            outputString = output.str();
            //printf("The block is full!\n");
            strcpy(path,path_temp);
            return;
        }

        // Find a unused inode
        flag = 0;
        for(int i_l=0;i_l<MAX_INODELIST_NUMBER;++i_l){
            if(inodeListMem[i_l].full==1) continue;
            struct DirectInode *inodeMem=&inodeListMem[i_l].inodeMem[0];
            // std::copy(inodeListMem[i_l].inodeMem, inodeListMem[i_l].inodeMem + INODE_NUMBER, inodeMem);
            for(int inode_index=0;inode_index<INODE_NUMBER;++inode_index){
                if (inodeMem[inode_index].blockID == -1) {
                    flag = 1;
                    n_inodeList=inodeListMem[i_l].inodeListID;
                    n_inode = iListID_plus_offset_to_inodeID(n_inodeList,inode_index);

                    inodeMem[inode_index].blockID = n_block;
                    inodeMem[inode_index].fileType = 0;
                    inodeMem[inode_index].inodeNumber=n_inode;
                    inodeMem[inode_index].inodeType=0;
                    build_Inode_Privilige(P[0],P[1],P[2],inodeMem[inode_index].privilege);
                    //inodeMem[inode_index].privilege=0;//暂时，还没定权限

                    if(inode_index==INODE_NUMBER-1){
                        inodeListMem[i_l].full=1;
                    }
                    int sb_flag=0;
                    for(int sb=0;sb<MAX_INODELIST_NUMBER;++sb){
                        if(inodeMem[sb].blockID==-1){
                            sb_flag=1;
                        }
                    }
                    if(sb_flag==0){
                        inodeListMem[i_l].full=1;
                    }
                    
                    write_Inode_To_File(superBlock,inodeListMem,i_l,inode_index);
                    break;
                }
            }
            if(flag==1) break;
        }

        if (n_inode == -1) {
            output<<"The inode is full!\n";
            outputString = output.str();
            //printf("The inode is full!\n");
            strcpy(path,path_temp);
            return;
        }

        // Initialize the new directory file
        block->inodeID[entry] = n_inode;
        strcpy(block->fileName[entry], target);
        

        blockBitmap[n_block/8] |= 1 << (n_block % 8);//将位图中的第 n_block 位（对应于分配的数据块）设置为1
        if(1+n_block>superBlock.max_usedBlockNum){
            superBlock.max_usedBlockNum=n_block+1;
        }
        // if(n_inode>superBlock.max_usedInodeListNum){
        //     superBlock.max_usedBlockNum=n_inode;
        // }
        // superBlock.usedBlockNum+=1;
        // superBlock.usedInodeListNum+=1;
        
        block = (struct DirectoryBlock *) &blockMem[n_block];
        block->blockType=0;
        for (i = 0; i < ENTRY_NUMBER; i++) {
            block->inodeID[i] = -1;//clock中所有的inode——id设为-1
        }

        write_Block_To_File(superBlock,blockMem,parent_blockID);
        write_Block_To_File(superBlock,blockMem,n_block);
        write_SuperBlock_To_File(superBlock);
        
    }
    else {
        output<<"The directory is full!\n";
        //printf("The directory is full!\n");
    }
    outputString = output.str();

    strcpy(path,path_temp);
    return;
}

void listFiles(char path[100],std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]) {
    char path_temp[100]="";
    strcpy(path_temp,path);
    int i, flag;
    std::ostringstream output;
    //char path[100];
    char *directory, *parent;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针


    // printf("Input the path:\n");
    // scanf("%s", path);
    output<<"The directory includes following files\n";
    //printf("The directory includes following files\n");

    directory = strtok(path, delimiter);
    while (directory != NULL) {
        block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
        flag = 0;
        for (i = 0; i < ENTRY_NUMBER; i++) { 
            if (strcmp(block->fileName[i], directory) == 0) //找到父目录的位置
            {   
                int inode_temp=block->inodeID[i];
                int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                int offset=inodeID_to_inodeListID(inode_temp)[0];
                int Mem_index=superBlock.inodeListid_to_MemIndex[inodeList_temp];
                if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                    flag=1;
                    pointer=&inodeListMem[Mem_index].inodeMem[offset];
                    break;
                }
                                    
            }
        }
        if (flag == 0) {
            // If the parent directory does not exist or is not a directory file
            output<<"The path does not exist!\n";
            outputString = output.str();
            //printf("The path does not exist!\n");
            strcpy(path,path_temp);
            return;
        }
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }

    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];

    
    output<< "INode\tisDir\tFile Name\tPrivilege\n";

    //printf("INode\tisDir\tFile Name\tPrivilege\n");
    for (i = 0; i < ENTRY_NUMBER; i++) {
        int inode_temp=block->inodeID[i];
        int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
        int n=inodeID_to_inodeListID(inode_temp)[0];
        int memPos=superBlock.inodeListid_to_MemIndex[inodeList_temp];
        if (block->inodeID[i] != -1) {
            //printf("%-6d\t%-5d\t%-15s\t%s\n", block->inodeID[i], 1-inodeListMem[memPos].inodeMem[n].fileType, block->fileName[i],inodeListMem[memPos].inodeMem[n].privilege);
            output << std::left << std::setw(6) << block->inodeID[i] << "\t"
               << 1 - inodeListMem[memPos].inodeMem[n].fileType << "\t"
               << std::setw(15) << block->fileName[i]
               << Char_to_Eight_Bit(inodeListMem[memPos].inodeMem[n].privilege[0])<<" "
               <<Char_to_Eight_Bit(inodeListMem[memPos].inodeMem[n].privilege[1])<<" " 
               <<Char_to_Eight_Bit(inodeListMem[memPos].inodeMem[n].privilege[2])<<" "<< "\n";
        }
    }
    outputString = output.str();
    
    strcpy(path,path_temp);
    return;
}

void createFile(char path[100],std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]) {
    inodeExtend(superBlock,inodeListMem,blockMem);
    char path_temp[100]="";
    strcpy(path_temp,path);
    int i, j, flag;
    std::ostringstream output;
    std::vector<std::string> parent_inodeid(3);
    // char path[100];
    char *directory, *parent, *target;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针
    char *blockBitmap=&superBlock.blockBitmap[0];

    std::unordered_map<int, int> inodeListid_to_MemIndex=superBlock.inodeListid_to_MemIndex;


    // printf("Input the path:\n");
    // scanf("%s", path);//绝对路径

    // Access the pointer of parent directory recursively
    parent = NULL;
    directory = strtok(path, delimiter);
    // printf("AAAdirectory%s\n",directory);

    //找到上一级目录的位置
    while (directory != NULL) {
        if (parent != NULL) //上一级不是根目录
        {
            block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
            flag = 0;
            for (i = 0; i < ENTRY_NUMBER; i++) { 
                if (strcmp(block->fileName[i], parent) == 0) //找到父目录的位置
                {   
                    int inode_temp=block->inodeID[i];
                    int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                    int offset=inodeID_to_inodeListID(inode_temp)[0];
                    int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
                    if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                        flag=1;
                        pointer=&inodeListMem[Mem_index].inodeMem[offset];                        
                        break;
                    }
                                    
                }
            }
            if (flag == 0) {
                output<<"The path does not exist!\n";
                outputString = output.str();
                // If the parent directory does not exist or is not a directory file
                //printf("The path does not exist!\n");
                strcpy(path,path_temp);
                return;
            }
        }
        parent = directory;
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }
    //最后出来的时候，要创建的目录就是parent；point指向父目录

    // Create the target directory
    int entry = -1;//块内第几项
    int n_block = -1;//选择了第几块来放置
    int n_inode = -1;//选择的inode#
    int n_inodeList=-1;//选择的inodeList#
    target = parent;
    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//父block
    int parent_blockID=pointer->blockID;
    for(int p_i=0;p_i<3;++p_i){
        parent_inodeid[p_i]="00000";
        parent_inodeid[p_i]+=Char_to_Eight_Bit(pointer->privilege[p_i]).substr(5);
    }


    

     for (i = 0; i < ENTRY_NUMBER; i++) {
        if (strcmp(block->fileName[i], target) == 0) {
            int inode_temp=block->inodeID[i];
            int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
            int n=inodeID_to_inodeListID(inode_temp)[0];
            int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
            if(inodeListMem[Mem_index].inodeMem[n].fileType==1){
                //父目录里已经有这个文件了
                output<<"The file already exists!\n";
                outputString = output.str();
                //printf("The file already exists!\n");
                strcpy(path,path_temp);
                return;
            }
        }

        if (block->inodeID[i] == -1) {
            entry = i;//将父目录的第i项放置该文件夹
            break;
        }
    }

    if (entry >= 0) {
        // Find a unused block
        for (i = 0; i < BLOCK_NUMBER/8; i++) {
            for (j = 0; j < 8; j++) {
                if ((blockBitmap[i] & (1 << j)) == 0) //第i块对应位图第i位，检查 blockBitmap[i] 中的第 j 位是否为0
                {
                    n_block = i * 8 + j;
                    break;
                }
            }
            if (n_block != -1) {
                break;
            }
        }
        if (n_block == -1) {
            output<<"The block is full!\n";
            outputString = output.str();
            //printf("The block is full!\n");
            strcpy(path,path_temp);
            return;
        }

        // Find a unused inode
        flag = 0;
        for(int i_l=0;i_l<MAX_INODELIST_NUMBER;++i_l){
            if(inodeListMem[i_l].full==1) continue;
            struct DirectInode *inodeMem=&inodeListMem[i_l].inodeMem[0];
            // std::copy(inodeListMem[i_l].inodeMem, inodeListMem[i_l].inodeMem + INODE_NUMBER, inodeMem);
            for(int inode_index=0;inode_index<INODE_NUMBER;++inode_index){
                if (inodeMem[inode_index].blockID == -1) {
                    flag = 1;
                    n_inodeList=inodeListMem[i_l].inodeListID;
                    n_inode = iListID_plus_offset_to_inodeID(n_inodeList,inode_index);

                    inodeMem[inode_index].blockID = n_block;
                    inodeMem[inode_index].fileType = 1;
                    inodeMem[inode_index].inodeNumber=n_inode;
                    inodeMem[inode_index].inodeType=0;
                    build_Inode_Privilige(parent_inodeid[0],parent_inodeid[1],parent_inodeid[2],inodeMem[inode_index].privilege);
                    //inodeMem[inode_index].privilege=0;//暂时，还没定权限

                    if(inode_index==INODE_NUMBER-1){
                        inodeListMem[i_l].full=1;
                    }
                    int sb_flag=0;
                    for(int sb=0;sb<MAX_INODELIST_NUMBER;++sb){
                        if(inodeMem[sb].blockID==-1){
                            sb_flag=1;
                        }
                    }
                    if(sb_flag==0){
                        inodeListMem[i_l].full=1;
                    }


                    write_Inode_To_File(superBlock,inodeListMem,i_l,inode_index);
                    break;
                }
            }
            if(flag==1) break;
        }

        if (n_inode == -1) {
            output<<"The inode is full!\n";
            outputString = output.str();
            //printf("The inode is full!\n");
            strcpy(path,path_temp);
            return;
        }




        // Initialize the new directory file
        block->inodeID[entry] = n_inode;
        strcpy(block->fileName[entry], target);

        blockBitmap[n_block/8] |= 1 << (n_block % 8);//将位图中的第 n_block 位（对应于分配的数据块）设置为1
        if(n_block+1>superBlock.max_usedBlockNum){
            superBlock.max_usedBlockNum=n_block+1;
        }
        // if(n_inode>superBlock.max_usedInodeListNum){
        //     superBlock.max_usedBlockNum=n_inode;
        // }
        // superBlock.usedBlockNum+=1;
        // superBlock.usedInodeListNum+=1;

        // block = (struct FileBlock *) &blockMem[n_block];
        // block->blockType=1;
        // for (i = 0; i < ENTRY_NUMBER; i++) {
        //     block->inodeID[i] = -1;//clock中所有的inode——id设为-1
        // }

        write_Block_To_File(superBlock,blockMem,parent_blockID);
        write_Block_To_File(superBlock,blockMem,n_block);
        write_SuperBlock_To_File(superBlock);
    }
    else {
        output<<"The directory is full!\n";
        //printf("The directory is full!\n");
    }
    outputString = output.str();

    strcpy(path,path_temp);
    return;

}

void deleteFile(char path[100],std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]) {
    char path_temp[100]="";
    strcpy(path_temp,path);
    int i, j, flag;
    std::ostringstream output;
    //char path[100];
    char *directory, *parent, *target;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针
    char *blockBitmap=&superBlock.blockBitmap[0];

    std::unordered_map<int, int> inodeListid_to_MemIndex=superBlock.inodeListid_to_MemIndex;



    // printf("Input the path:\n");
    // scanf("%s", path);//绝对路径

    // Access the pointer of parent directory recursively
    parent = NULL;
    directory = strtok(path, delimiter);
    // printf("AAAdirectory%s\n",directory);

    //找到上一级目录的位置
    while (directory != NULL) {
        if (parent != NULL) //上一级不是根目录
        {
            block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
            flag = 0;
            for (i = 0; i < ENTRY_NUMBER; i++) { 
                if (strcmp(block->fileName[i], parent) == 0) //找到父目录的位置
                {   
                    int inode_temp=block->inodeID[i];
                    int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                    int offset=inodeID_to_inodeListID(inode_temp)[0];
                    int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
                    if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                        flag=1;
                        pointer=&inodeListMem[Mem_index].inodeMem[offset];
                        break;
                    }
                                    
                }
            }
            if (flag == 0) {
                // If the parent directory does not exist or is not a directory file
                output<<"The path does not exist!\n";
                outputString = output.str();
                //printf("The path does not exist!\n");
                strcpy(path,path_temp);
                return;
            }
        }
        parent = directory;
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }
    //最后出来的时候，要创建的目录就是parent；point指向父目录

    // Create the target directory
    int entry = -1;//块内第几项
    int n_block = -1;//第几块放置
    int n_inode = -1;//inode#
    int n_inodeList=-1;//选择的inodeList#
    int inodeListID=-1;
    int offset=-1;
    int Mem_index=-1;
    target = parent;
    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//父block
    int parent_blockID=pointer->blockID;
    
    //找到对应n_inode，并对父块的设置
    for (i = 0; i < ENTRY_NUMBER; i++) {
        if(strcmp(block->fileName[i], target) == 0){
            n_inode=block->inodeID[i];
            inodeListID=inodeID_to_inodeListID(n_inode)[1];
            offset=inodeID_to_inodeListID(n_inode)[0];
            Mem_index=inodeListid_to_MemIndex[inodeListID];
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==1){
                //删除父目录中的这条消息
                n_inode=block->inodeID[i];
                strcpy(block->fileName[i], "");
                block->inodeID[i]=-1;


                inodeListMem[Mem_index].full=0;
                n_block=inodeListMem[Mem_index].inodeMem[offset].blockID;
                inodeListMem[Mem_index].inodeMem[offset].blockID=-1;
                inodeListMem[Mem_index].inodeMem[offset].fileType=-1;
                build_Inode_Privilige("00000000","00000000","00000000",inodeListMem[Mem_index].inodeMem[offset].privilege);
                //inodeListMem[Mem_index].inodeMem[offset].privilege=-1;
                
                break;
            }
            else{
                n_inode=-1;
                inodeListID=-1;
                offset=-1;
                Mem_index=-1;
            }          
        }
    }
    if(i==ENTRY_NUMBER){
        output<<"Can't Find The File.\n";
        outputString = output.str();
        //printf("Can't Find The File.\n");
        strcpy(path,path_temp);
        return;
    }

    //清空该块内容
    while(n_block!=-1){
        //std::cout<<n_block<<std::endl;
        blockMem[n_block].blockType=1;
        // strcpy(blockMem[n_block].content, "");
        std::memset(blockMem[n_block].content, 0, sizeof(blockMem[n_block].content));
        int point_block=blockMem[n_block].pointBlockNum;
        blockMem[n_block].pointBlockNum=-1;
        write_Block_To_File(superBlock,blockMem,n_block);
        //设置位图
        blockBitmap[n_block/8] &= ~(1 << (n_block % 8));
        n_block=point_block;
    }
    
    


    


    write_Inode_To_File(superBlock,inodeListMem,Mem_index,offset);
    write_Block_To_File(superBlock,blockMem,parent_blockID);
    
    write_SuperBlock_To_File(superBlock);
    outputString = output.str();

    strcpy(path,path_temp);
    return;


}

void readFile(char path[100],std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]) {
    char path_temp[100]="";
    strcpy(path_temp,path);
    int i, j, flag;
    std::ostringstream output;
    //char path[100];
    char *directory, *parent, *target;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针
    char *blockBitmap=&superBlock.blockBitmap[0];

    std::unordered_map<int, int> inodeListid_to_MemIndex=superBlock.inodeListid_to_MemIndex;


    // printf("Input the path:\n");
    // scanf("%s", path);//绝对路径

    // Access the pointer of parent directory recursively
    parent = NULL;
    directory = strtok(path, delimiter);
    // printf("AAAdirectory%s\n",directory);

    //找到上一级目录的位置
    while (directory != NULL) {
        if (parent != NULL) //上一级不是根目录
        {
            block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
            flag = 0;
            for (i = 0; i < ENTRY_NUMBER; i++) { 
                if (strcmp(block->fileName[i], parent) == 0) //找到父目录的位置
                {   
                    int inode_temp=block->inodeID[i];
                    int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                    int offset=inodeID_to_inodeListID(inode_temp)[0];
                    int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
                    if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                        flag=1;
                        pointer=&inodeListMem[Mem_index].inodeMem[offset];
                        break;
                    }
                                    
                }
            }
            if (flag == 0) {
                // If the parent directory does not exist or is not a directory file
                output<<"The path does not exist!\n";
                outputString = output.str();
                strcpy(path,path_temp);
                //printf("The path does not exist!\n");
                strcpy(path,path_temp);
                return;
            }
        }
        parent = directory;
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }
    //最后出来的时候，要创建的目录就是parent；point指向父目录

    // Create the target directory
    int entry = -1;//块内第几项
    int n_block = -1;//第几块放置
    int n_inode = -1;//inode#
    int flag_2=0;
    target = parent;
    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//父block
    //找到对应n_inode，并对父块的设置
    for (i = 0; i < ENTRY_NUMBER; i++) {
        if(strcmp(block->fileName[i], target) == 0){
            n_inode=block->inodeID[i];
            int inodeListID=inodeID_to_inodeListID(n_inode)[1];
            int offset=inodeID_to_inodeListID(n_inode)[0];
            int Mem_index=inodeListid_to_MemIndex[inodeListID];
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==1){
                // n_inode=block->inodeID[i];
                flag_2=1;
                n_block=inodeListMem[Mem_index].inodeMem[offset].blockID;

                while(n_block!=-1){
                    struct  FileBlock* block_f = (struct  FileBlock*) &blockMem[n_block];
                    for (j = 0; j < BLOCK_SIZE; j++) {
                        if(block_f->content[j]=='$'){
                            block_f->content[j]='\0';
                            break;
                        }
                    }
                    //output<<block_f->content;
                    output.write(block_f->content, strlen(block_f->content));
                    outputString = output.str();
                    //printf("%s",block_f->content);
                    n_block=block_f->pointBlockNum;
                }

                // struct  FileBlock* block_f = (struct  FileBlock*) &blockMem[n_block];
                // for (i = 0; i < BLOCK_SIZE; i++) {
                    
                //     // if(block_f->content[i]==NULL) continue;
                //     // printf("%s",block_f->content[i]);
                // }
                // printf("%s",block_f->content);
                // break;
            }
        }
    }
    if(flag_2==0){
        output<<"Can't Find the File!\n";
        outputString = output.str();
        strcpy(path,path_temp);
        return;
    }
    outputString = output.str();

    strcpy(path,path_temp);
    //write_Block_To_File(superBlock,blockMem,n_block);

}

void writeFile(char path[100],std::string& outputString,std::string inputLine,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]) {
    char path_temp[100]="";
    strcpy(path_temp,path);
    int i, j, flag;
    std::ostringstream output;
    //char path[100];
    char *directory, *parent, *target;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针
    char *blockBitmap=&superBlock.blockBitmap[0];

    std::unordered_map<int, int> inodeListid_to_MemIndex=superBlock.inodeListid_to_MemIndex;


    // printf("Input the path:\n");
    // scanf("%s", path);//绝对路径

    // Access the pointer of parent directory recursively
    parent = NULL;
    directory = strtok(path, delimiter);
    // printf("AAAdirectory%s\n",directory);

    //找到上一级目录的位置
        while (directory != NULL) {
        if (parent != NULL) //上一级不是根目录
        {
            block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
            flag = 0;
            for (i = 0; i < ENTRY_NUMBER; i++) { 
                if (strcmp(block->fileName[i], parent) == 0) //找到父目录的位置
                {   
                    int inode_temp=block->inodeID[i];
                    int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                    int offset=inodeID_to_inodeListID(inode_temp)[0];
                    int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
                    if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                        flag=1;
                        pointer=&inodeListMem[Mem_index].inodeMem[offset];
                        break;
                    }
                                    
                }
            }
            if (flag == 0) {
                // If the parent directory does not exist or is not a directory file
                output<<"The path does not exist!\n";
                outputString = output.str();
                //printf("The path does not exist!\n");
                strcpy(path,path_temp);
                
                return;
            }
        }
        parent = directory;
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }
    //最后出来的时候，要创建的目录就是parent；point指向父目录

    // Create the target directory
    int entry = -1;//块内第几项
    int n_block = -1;//第几块放置
    int n_inode = -1;//inode#
    int parent_block=-1;
    int flag_2=0;
    target = parent;
    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//父block
    //找到对应n_inode，并对父块的设置
    for (i = 0; i < ENTRY_NUMBER; i++) {
        if(strcmp(block->fileName[i], target) == 0){
            n_inode=block->inodeID[i];
            int inodeListID=inodeID_to_inodeListID(n_inode)[1];
            int offset=inodeID_to_inodeListID(n_inode)[0];
            int Mem_index=inodeListid_to_MemIndex[inodeListID];
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==1){
                flag_2=1;
                // n_inode=block->inodeID[i];
                n_block=inodeListMem[Mem_index].inodeMem[offset].blockID;
                // if(n_block>superBlock.max_usedBlockNum){
                //     superBlock.max_usedBlockNum=n_block;
                // }
                struct  FileBlock* block_f = (struct  FileBlock*) &blockMem[n_block];

                //output<<"Input the file:"<<std::endl;
                // std::string inputLine;
                // std::getline(std::cin, inputLine, '$');
                //inputLine = inputLine + '$';
                //output<<inputLine<<std::endl;
                

                int size =BLOCK_SIZE;
                int turn = inputLine.size() /size;
                
                if (turn * size < inputLine.size()) turn += 1;
                //output<<"turn:"<<turn<<std::endl;
                
                for (int ii = 0; ii < turn; ++ii) {
                    //std::cout<<"ii:"<<ii<<std::endl;
                    if(ii>0){
                        //std::cout<<"III:"<<ii<<std::endl;
                        for (int i = 0; i < BLOCK_NUMBER/8; i++) {
                            for (int j = 0; j < 8; j++) {
                                //std::cout<<"(i,j):"<<i<<","<<j<<std::endl;
                                if ((blockBitmap[i] & (1 << j)) == 0) //第i块对应位图第i位，检查 blockBitmap[i] 中的第 j 位是否为0
                                {
                                    
                                    n_block = i * 8 + j;
                                    //std::cout<<"BLOCK!:"<<n_block<<std::endl;
                                    blockBitmap[n_block/8] |= 1 << (n_block % 8);//将位图中的第 n_block 位（对应于分配的数据块）设置为1
                                    block_f->pointBlockNum=n_block;
                                    write_Block_To_File(superBlock,blockMem,parent_block);
                                    if(n_block+1>superBlock.max_usedBlockNum){
                                        superBlock.max_usedBlockNum=n_block+1;
                                    }
                                    break;
                                }
                            }
                            if (n_block!=parent_block) {
                                break;
                            }
                        }
                        if (n_block == -1) {
                            output<<"The file is too big!Can't write!\n";
                            outputString = output.str();
                            //printf("The file is too big!Can't write!\n");
                            strcpy(path,path_temp);
                            return;
                        }
                    }
                    
                    //std::cout<<"n_block:"<<n_block<<std::endl;
                    block_f = (struct  FileBlock*) &blockMem[n_block];
                    int jj = 0;
                    for (; jj < size && (ii * size + jj) < inputLine.size(); ++jj) {
                        block_f->content[jj] = inputLine[jj+ii*size];
                    }
                    if(jj<size){
                        for(;jj<size;++jj){
                            block_f->content[jj] ='\0';
                            int pointnum=block_f->pointBlockNum;
                            while(pointnum!=-1){
                                blockBitmap[pointnum/8] &= ~(1 << (pointnum % 8));
                                struct  FileBlock* block_over = (struct  FileBlock*) &blockMem[pointnum];
                                int temp_block=pointnum;
                                pointnum=block_over->pointBlockNum;
                                block_over->pointBlockNum=-1;
                                write_Block_To_File(superBlock,blockMem,temp_block);
                            }

                            block_f->pointBlockNum=-1;
                        }
                    }
                    //std::cout<<"content[0]:"<<block_f->content[0]<<std::endl;
                    parent_block=n_block;
                    write_Block_To_File(superBlock,blockMem,n_block);
                }                
                break;
            }
        }
    }
    if(flag_2==0){
        output<<"Can't Find the File!\n";
        outputString = output.str();
        strcpy(path,path_temp);
        return;
    }
    write_SuperBlock_To_File(superBlock);
    outputString = output.str();

    strcpy(path,path_temp);
}

struct DirectoryBlock * deleteSingleDir(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],struct DirectoryBlock *block,char path[100]){
    int i, j, flag;
    char *directory, *parent, *target;
    // char *temp;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    //struct DirectoryBlock *block;//指向block的指针
    char *blockBitmap=&superBlock.blockBitmap[0];

    std::unordered_map<int, int> inodeListid_to_MemIndex=superBlock.inodeListid_to_MemIndex;


    char paths[100]="";
    //strcpy(paths, path);
    parent = NULL;
    // temp=NULL;
    directory = strtok(path, delimiter);

    //找到上一级目录的位置
    while (directory != NULL) {
        if (parent != NULL) //上一级不是根目录
        {
            strcat(paths, "/");
            strcat(paths, parent);
            block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
            flag = 0;
            for (i = 0; i < ENTRY_NUMBER; i++) { 
                if (strcmp(block->fileName[i], parent) == 0) //找到父目录的位置
                {   
                    int inode_temp=block->inodeID[i];
                    int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                    int offset=inodeID_to_inodeListID(inode_temp)[0];
                    int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
                    if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                        flag = 1;
                        pointer = &inodeListMem[Mem_index].inodeMem[offset];//指针指向对应block的inode#
                        break;
                    }
                    
                }
            }
            if (flag == 0) {
                // If the parent directory does not exist or is not a directory file
                //printf("The path does not exist!\n");
                //strcpy(path, temp);
                strcpy(path, paths);
                return block;
            }
        }
        
        parent = directory;
        
        directory = strtok(NULL, delimiter);
        
        // printf("directory%s\n",directory);
    }
    //最后出来的时候，要创建的目录就是parent；point指向父目录


    // Create the target directory
    int entry = -1;//块内第几项
    int n_block = -1;//第几块放置
    int n_inode = -1;//inode#
    int n_inodeList=-1;//选择的inodeList#
    int inodeListID=-1;
    int offset=-1;
    int Mem_index=-1;
    target = parent;
    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//父block
    int parent_blockID=pointer->blockID;
    //找到对应n_inode，并对父块的设置
    for (i = 0; i < ENTRY_NUMBER; i++) {
        if(strcmp(block->fileName[i], target) == 0){
            n_inode=block->inodeID[i];
            inodeListID=inodeID_to_inodeListID(n_inode)[1];
            offset=inodeID_to_inodeListID(n_inode)[0];
            Mem_index=inodeListid_to_MemIndex[inodeListID];
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                strcpy(block->fileName[i], "");
                block->inodeID[i]=-1;

                inodeListMem[Mem_index].full=0;
                n_block=inodeListMem[Mem_index].inodeMem[offset].blockID;
                inodeListMem[Mem_index].inodeMem[offset].blockID=-1;
                inodeListMem[Mem_index].inodeMem[offset].fileType=-1;
                build_Inode_Privilige("00000000","00000000","00000000",inodeListMem[Mem_index].inodeMem[offset].privilege);
                //inodeListMem[Mem_index].inodeMem[offset].privilege=-1;
                break;
            }
            else{
                n_inode=-1;
                inodeListID=-1;
                offset=-1;
                Mem_index=-1;
            }  
            
        }
    }
    if(i==ENTRY_NUMBER){
        //printf("Can't Find The Folder.\n");
        //strcpy(path, temp);
        strcpy(path, paths);
        return block;
    }
    //清空该块内容
    struct DirectoryBlock *block_now = (struct DirectoryBlock *) &blockMem[n_block];
    block_now->blockType=1;
    for(int i=0;i<ENTRY_NUMBER;++i){
        strcpy(block_now->fileName[i], "");
        block_now->inodeID[i];
    }
    // strcpy(blockMem[n_block].content, "");

    // //设置inode表
    // inodeMem[n_inode].blockID=-1;
    // inodeMem[n_inode].fileType=-1;

    //设置位图
    blockBitmap[n_block/8] &= ~(1 << (n_block % 8));
    
    write_Inode_To_File(superBlock,inodeListMem,Mem_index,offset);
    write_Block_To_File(superBlock,blockMem,parent_blockID);
    write_Block_To_File(superBlock,blockMem,n_block);
    write_SuperBlock_To_File(superBlock);
    
    //strcpy(path, temp);
    strcpy(path, paths);

    return block;
}

void deleteSingleFile(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],char path[100]) {
    int i, j, flag;
    //char path[100];
    char paths[100]="";
    char *directory, *parent, *target;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针
    char *blockBitmap=&superBlock.blockBitmap[0];

    std::unordered_map<int, int> inodeListid_to_MemIndex=superBlock.inodeListid_to_MemIndex;



    // printf("Input the path:\n");
    // scanf("%s", path);//绝对路径

    // Access the pointer of parent directory recursively
    parent = NULL;
    directory = strtok(path, delimiter);
    // printf("AAAdirectory%s\n",directory);

    //找到上一级目录的位置
    while (directory != NULL) {
        if (parent != NULL) //上一级不是根目录
        {
            strcat(paths, "/");
            strcat(paths, parent);
            block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
            flag = 0;
            for (i = 0; i < ENTRY_NUMBER; i++) { 
                if (strcmp(block->fileName[i], parent) == 0) //找到父目录的位置
                {   
                    int inode_temp=block->inodeID[i];
                    int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                    int offset=inodeID_to_inodeListID(inode_temp)[0];
                    int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
                    if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                        flag=1;
                        pointer=&inodeListMem[Mem_index].inodeMem[offset];
                        break;
                    }
                                    
                }
            }
            if (flag == 0) {
                // If the parent directory does not exist or is not a directory file
                //printf("The path does not exist!\n");
                strcpy(path, paths);
                return ;
            }
        }
        parent = directory;
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }
    //最后出来的时候，要创建的目录就是parent；point指向父目录

    // Create the target directory
    int entry = -1;//块内第几项
    int n_block = -1;//第几块放置
    int n_inode = -1;//inode#
    int n_inodeList=-1;//选择的inodeList#
    int inodeListID=-1;
    int offset=-1;
    int Mem_index=-1;
    target = parent;
    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//父block
    int parent_blockID=pointer->blockID;
    
    //找到对应n_inode，并对父块的设置
    for (i = 0; i < ENTRY_NUMBER; i++) {
        if(strcmp(block->fileName[i], target) == 0){
            n_inode=block->inodeID[i];
            inodeListID=inodeID_to_inodeListID(n_inode)[1];
            offset=inodeID_to_inodeListID(n_inode)[0];
            Mem_index=inodeListid_to_MemIndex[inodeListID];
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==1){
                //删除父目录中的这条消息
                n_inode=block->inodeID[i];
                strcpy(block->fileName[i], "");
                block->inodeID[i]=-1;


                inodeListMem[Mem_index].full=0;
                n_block=inodeListMem[Mem_index].inodeMem[offset].blockID;
                inodeListMem[Mem_index].inodeMem[offset].blockID=-1;
                inodeListMem[Mem_index].inodeMem[offset].fileType=-1;
                build_Inode_Privilige("00000000","00000000","00000000",inodeListMem[Mem_index].inodeMem[offset].privilege);
                //inodeListMem[Mem_index].inodeMem[offset].privilege=-1;
                
                break;
            }
            else{
                n_inode=-1;
                inodeListID=-1;
                offset=-1;
                Mem_index=-1;
            }          
        }
    }
    if(i==ENTRY_NUMBER){
        //printf("Can't Find The File.\n");
        strcpy(path, paths);
        return ;
    }

    //清空该块内容
    while(n_block!=-1){
        //std::cout<<n_block<<std::endl;
        blockMem[n_block].blockType=1;
        // strcpy(blockMem[n_block].content, "");
        std::memset(blockMem[n_block].content, 0, sizeof(blockMem[n_block].content));
        int point_block=blockMem[n_block].pointBlockNum;
        blockMem[n_block].pointBlockNum=-1;
        write_Block_To_File(superBlock,blockMem,n_block);
        //设置位图
        blockBitmap[n_block/8] &= ~(1 << (n_block % 8));
        n_block=point_block;
    }


    //设置位图
    // blockBitmap[n_block/8] &= ~(1 << (n_block % 8));


    write_Inode_To_File(superBlock,inodeListMem,Mem_index,offset);
    write_Block_To_File(superBlock,blockMem,parent_blockID);
    // write_Block_To_File(superBlock,blockMem,n_block);
    write_SuperBlock_To_File(superBlock);

    strcpy(path, paths);
    return ;


}

struct DirectoryBlock * DFS(std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],struct DirectoryBlock * block,char parent[100]){
    //printf("刚进来的parent%s\n",parent);
    std::ostringstream output;  
    for(int i=0;i<ENTRY_NUMBER;++i){
        //std::cout<<"block->inodeID[i]:"<<block->inodeID[i]<<std::endl;
        if(block->inodeID[i] != -1){
            char* relat_name=block->fileName[i];
            //std::cout<<"relat_name:"<<relat_name<<std::endl;
            strcat(parent,"/");
            strcat(parent,relat_name);


            int inode_temp=block->inodeID[i];
            int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
            int offset=inodeID_to_inodeListID(inode_temp)[0];
            int Mem_index=superBlock.inodeListid_to_MemIndex[inodeList_temp];
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==0)//保证type是文件夹
            {   
                struct DirectInode *pointer = &inodeListMem[Mem_index].inodeMem[offset];
                block = (struct DirectoryBlock *) &blockMem[pointer->blockID];
                block=DFS(outputString,superBlock,inodeListMem,blockMem,block,parent);
                //printf("BACK:%s\n",parent);
            }
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==1)//保证type是文件
            {   
                output<<"FILE DELETE:"<<parent<<std::endl;
                //printf("FILE DELETE:%s\n",parent);
                deleteSingleFile(superBlock,inodeListMem,blockMem,parent);
            }
        }
    }
    output<<"FOLDER DELETE:"<<parent<<std::endl;
    //printf("FOLDER DELETE:%s\n",parent);
    block=deleteSingleDir(superBlock,inodeListMem,blockMem,block,parent);
    outputString += output.str();
    return block;
}

void deleteDirectory(char path[100],std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]) {
    char path_temp[100]="";
    strcpy(path_temp,path);
    int i, j, flag;
    std::ostringstream output;
    //char path[100];
    char *directory, *parent, *target;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针
    char *blockBitmap=&superBlock.blockBitmap[0];

    std::unordered_map<int, int> inodeListid_to_MemIndex=superBlock.inodeListid_to_MemIndex;

    // printf("Input the path:\n");
    // scanf("%s", path);//绝对路径
    char paths[100]="";
    strcpy(paths, path);
    // Access the pointer of parent directory recursively
    parent = NULL;
    directory = strtok(path, delimiter);
    // printf("AAAdirectory%s\n",directory);

    //找到上一级目录的位置
    while (directory != NULL) {
        if (parent != NULL) //上一级不是根目录
        {
            block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
            flag = 0;
            for (i = 0; i < ENTRY_NUMBER; i++) { 
                if (strcmp(block->fileName[i], parent) == 0) //找到父目录的位置
                {   
                    int inode_temp=block->inodeID[i];
                    int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                    int offset=inodeID_to_inodeListID(inode_temp)[0];
                    int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
                    if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                        flag=1;
                        pointer=&inodeListMem[Mem_index].inodeMem[offset];
                        break;
                    }
                                    
                }
            }
            if (flag == 0) {
                // If the parent directory does not exist or is not a directory file
                output<<"The path does not exist!\n";
                outputString = output.str();
                //printf("The path does not exist!\n");
                strcpy(path,path_temp);
                return;
            }
        }
        parent = directory;
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }
    //最后出来的时候，要创建的目录就是parent；point指向父目录
    
    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//父block
    int parent_blockID=pointer->blockID;
    target=parent;
    for(i=0;i<ENTRY_NUMBER;++i){
        if(strcmp(block->fileName[i], target) == 0){
            int n_inode=block->inodeID[i];
            int inodeListID=inodeID_to_inodeListID(n_inode)[1];
            int offset=inodeID_to_inodeListID(n_inode)[0];
            int Mem_index=inodeListid_to_MemIndex[inodeListID];
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==0)//保证type是文件
            { 
                pointer=&inodeListMem[Mem_index].inodeMem[offset];
                block = (struct DirectoryBlock *) &blockMem[pointer->blockID];
                //printf("isok!\nblockID%d\n",pointer->blockID);
                break;
            }
        }
    }
    if(i==ENTRY_NUMBER){
        output<<"Can't Find The Folder.\n";
        outputString = output.str();
        //printf("Can't Find The Folder.\n");
        strcpy(path,path_temp);
        return ;
    }
    // map<char*,char*>absolute_path;
    // absolute_path.insert(make_pair(target, path));
    //printf("paths:%s\n",paths);
    //std::cout<<"n_block:"<<pointer->blockID<<std::endl;
    std::string dfs_string;
    //output<<"HHHHHHHHHHHHHHHHHH\n";
    DFS(dfs_string,superBlock,inodeListMem,blockMem,block,paths);
    //output<<"AAAAAAAAAAAAAAAAAA\n";
    outputString = output.str()+dfs_string;

    strcpy(path,path_temp);
}



void init_inodelist(struct InodeList inodeListMem[MAX_INODELIST_NUMBER],int startPos,int pointLevel,int inodelistID){
    struct InodeList* inodelist;
    inodelist=(struct InodeList*)&inodeListMem[startPos];
    inodelist->inodeListID=inodelistID;
    inodelist->full=0;
    inodelist->level=pointLevel;
    inodelist->init_inodeMem();
}

void init_Indirect_inodelist(struct InodeList inodeListMem[MAX_INODELIST_NUMBER],int startPos,int pointLevel,int inodelistID){
    struct InodeList* inodelist;
    inodelist=(struct InodeList*)&inodeListMem[startPos];
    inodelist->inodeListID=inodelistID;
    inodelist->full=1;
    inodelist->level=pointLevel;
    inodelist->init_inodeMem();
}

void A_copyto_B(struct InodeList inodeListMem[MAX_INODELIST_NUMBER],int A_Pos,int B_memPos){
    struct InodeList* inodelist;
    inodelist=(struct InodeList*)&inodeListMem[B_memPos];
    inodelist->inodeMem[0].blockID=inodeListMem[0].inodeMem[A_Pos].blockID;
    inodelist->inodeMem[0].fileType=inodeListMem[0].inodeMem[A_Pos].fileType;
    //inodelist->inodeMem[0].inodeNumber=inodeListMem[0].inodeMem[A_Pos].inodeNumber;
    inodelist->inodeMem[0].inodeType=inodeListMem[0].inodeMem[A_Pos].inodeType;  
    std::copy(inodeListMem[0].inodeMem[A_Pos].privilege, inodeListMem[0].inodeMem[A_Pos].privilege + 4, inodelist->inodeMem[0].privilege);
    // inodelist->inodeMem[0].privilege=inodeListMem[0].inodeMem[A_Pos].privilege; 
}
void change_inode_type(struct InodeList inodeListMem[MAX_INODELIST_NUMBER],int memPos,int offset,int pointLevel,int inodeID){
    IndirectInode* ind_inode=(IndirectInode*)&inodeListMem[memPos].inodeMem[offset];
    ind_inode->inodeNumber=inodeID;
    ind_inode->inodeListID=inodeID;
    ind_inode->inodeType=0;
    ind_inode->pointLevel=pointLevel;
}

void init_inodeList_and_inode(SuperBlock superblock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER], int tobe_level){
    int startPos=superblock.inodeListid_to_MemIndex[tobe_level];
    if(tobe_level==1) {
        //初始化inodelist

        init_inodelist(inodeListMem,startPos,1,1);
        //最外层首块copy信息
        A_copyto_B(inodeListMem,1,startPos);
        //对中间level和起点块改类别
        change_inode_type(inodeListMem,0,1,1,1);

        write_Inode_To_File(superblock,inodeListMem,0,1);
        write_WholeInode_To_File(superblock,inodeListMem,1);
    }

    else if(tobe_level==2) {
        //初始化inodelist
        //init_inodelist(inodeListMem,startPos,1,2);
        init_Indirect_inodelist(inodeListMem,startPos,1,2);

        IndirectInode* ind_inode;
        for(int i=0;i<10;++i){
            //初始化inodelist
            init_inodelist(inodeListMem,1+startPos+i,2,20+i);
            if(i==0){
                //最外层首块copy信息
                A_copyto_B(inodeListMem,2,1+startPos+i);
            }
            write_WholeInode_To_File(superblock,inodeListMem,1+startPos+i);
        }
        //对中间level和起点块改类别
        for(int i=0;i<INODE_NUMBER;++i){
            change_inode_type(inodeListMem,2,i,2,20+i);
            //int mempos=superblock.inodeListid_to_MemIndex[20+i];
            
        }      
        write_WholeInode_To_File(superblock,inodeListMem,2);
        change_inode_type(inodeListMem,0,2,1,2);
        write_Inode_To_File(superblock,inodeListMem,0,2);
    }


    else if(tobe_level==3) {
        //初始化inodelist
        //init_inodelist(inodeListMem,startPos,1,3);
        init_Indirect_inodelist(inodeListMem,startPos,1,3);

        IndirectInode* ind_inode;
        for(int i=0;i<10;++i){
            //初始化inodelist
            //init_inodelist(inodeListMem,1+startPos+i,2,30+i);
            init_Indirect_inodelist(inodeListMem,1+startPos+i,2,30+i);
        }
        for(int i=0;i<100;++i){
            //初始化inodelist
            init_inodelist(inodeListMem,11+startPos+i,3,300+i);
            if(i==0){
                //最外层首块copy信息
                A_copyto_B(inodeListMem,3,11+startPos+i);
            }
            write_WholeInode_To_File(superblock,inodeListMem,11+startPos+i);
        }
        //对中间level和起点块改类别
        //int mempos=superblock.inodeListid_to_MemIndex[3];
        for(int i=0;i<INODE_NUMBER;++i){            
            change_inode_type(inodeListMem,startPos,i,2,30+i);         
        }
        write_WholeInode_To_File(superblock,inodeListMem,startPos);

        for(int j=0;j<INODE_NUMBER;++j){
            for(int i=0;i<INODE_NUMBER;++i){
                change_inode_type(inodeListMem,j+startPos+1,i,3,(j+30)*10+i);
                // int mempos=superblock.inodeListid_to_MemIndex[];
                write_WholeInode_To_File(superblock,inodeListMem,j+startPos+1);
            }
        }
        change_inode_type(inodeListMem,0,3,1,3);
        write_Inode_To_File(superblock,inodeListMem,0,2);
    }
}
    
void inodeExtend(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    int full=0;
    int i=0;
    for(;i<MAX_INODELIST_NUMBER;++i){
        if(inodeListMem[i].inodeListID==-1) break;
        full+=inodeListMem[i].full;
    }
    int max_level=inodeListMem[i-1].level;
    int limit=-1;
    int tobe_level=-1;
    int origin_inodeid=-1;
    int new_inodeid=-1;
    int max_usedInodeListNum;
    if(max_level==0){
        limit=1;
        tobe_level=1;
        origin_inodeid=1;
        new_inodeid=10;
        max_usedInodeListNum=2;
    }
    else if(max_level==1){
        limit=2;
        tobe_level=2;
        origin_inodeid=2;
        new_inodeid=200;
        max_usedInodeListNum=13;
    }
    else if(max_level==2){
        limit=13;
        tobe_level=3;
        origin_inodeid=3;
        new_inodeid=3000;
        max_usedInodeListNum=124;
    }
    else if(max_level==3){
        limit=124;
    }
    if(full!=limit) return;
    if(max_level==3) {
        std::cout<<"THE FILE SYSTEM IS FULL";
        return;
    }
    init_inodeList_and_inode(superBlock,inodeListMem,tobe_level);


    for(int i=0;i<superBlock.max_usedBlockNum;++i){
        DirectoryBlock* block=(DirectoryBlock*)&blockMem[i];
        if(block->blockType==0){
            for(int j=0;j<ENTRY_NUMBER;++j){
                if(block->inodeID[j]==origin_inodeid){
                    block->inodeID[j]=new_inodeid;
                    write_Block_To_File(superBlock,blockMem,i);
                }
            }
            
        }
    }
    superBlock.max_usedInodeListNum=max_usedInodeListNum;
    write_SuperBlock_To_File(superBlock);
}




void ls_only_name(char path[100],std::string& outputString,int f_type,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]) {
    char path_temp[100]="";
    strcpy(path_temp,path);
    int i, flag;
    std::ostringstream output;
    //char path[100];
    char *directory, *parent;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针


    directory = strtok(path, delimiter);
    while (directory != NULL) {
        block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
        flag = 0;
        for (i = 0; i < ENTRY_NUMBER; i++) { 
            if (strcmp(block->fileName[i], directory) == 0) //找到父目录的位置
            {   
                int inode_temp=block->inodeID[i];
                int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                int offset=inodeID_to_inodeListID(inode_temp)[0];
                int Mem_index=superBlock.inodeListid_to_MemIndex[inodeList_temp];
                if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                    flag=1;
                    pointer=&inodeListMem[Mem_index].inodeMem[offset];
                    break;
                }
                                    
            }
        }
        if (flag == 0) {
            // If the parent directory does not exist or is not a directory file
            output<<"The path does not exist!\n";
            outputString = output.str();
            //printf("The path does not exist!\n");
            return;
        }
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }

    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];


    //printf("INode\tisDir\tFile Name\tPrivilege\n");
    for (i = 0; i < ENTRY_NUMBER; i++) {
        int inode_temp=block->inodeID[i];
        int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
        int n=inodeID_to_inodeListID(inode_temp)[0];
        int memPos=superBlock.inodeListid_to_MemIndex[inodeList_temp];
        if (block->inodeID[i] != -1) {
            if(inodeListMem[memPos].inodeMem[n].fileType==f_type){
                output<<block->fileName[i]<<"%";
            }
        }
    }
    outputString = output.str();

    strcpy(path,path_temp);
    return;
}


void hash_to_file(std::map<std::string, int> hash,char path[100],SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    char path_temp[100]="";
    strcpy(path_temp,path);
    std::string hashContent;
    for (const auto& pair : hash) {
        hashContent+=pair.first+":"+std::to_string(pair.second)+";";
    }
    std::string output;
    writeFile(path,output,hashContent,superBlock,inodeListMem,blockMem);
    strcpy(path,path_temp);
}
void file_to_hash(std::map<std::string, int> &hash,char path[100],SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    char path_temp[100]="";
    strcpy(path_temp,path);
    std::string hashContent;
    readFile(path,hashContent,superBlock,inodeListMem,blockMem);
    char delimiter = ';';
    std::istringstream stream(hashContent);
	std::string token;
    std::string value;
    std::string key;
	while (std::getline(stream, token, delimiter)){
        std::string temp = token;
		size_t lastPlusPos = temp.find_last_of(':');
        key=temp.substr(0,lastPlusPos);
		value = temp.substr(lastPlusPos+1);
        try{
            hash[key]=std::stoi(value);
        }catch (const std::invalid_argument& ia) {
            //std::cerr << "Invalid argument: " << ia.what() << std::endl;
        } catch (const std::out_of_range& oor) {
            //std::cerr << "Out of range: " << oor.what() << std::endl;
        }
    }

    strcpy(path,path_temp);
}
void string_to_char100(std::string s,char c[100]){
    const char* charArray = s.c_str();
    strncpy(c, charArray, sizeof(s) - 1);
    c[sizeof(s) - 1] = '\0';
}

void copy_file(char source[100],char target[100],SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    std::string out_creat;
    std::string out_read;
    std::string out_write;
    // char targets[100]="";
    // strcpy(targets, target);
    // char sources[100]="";
    // strcpy(targets, target);
    deleteFile(target,out_creat,superBlock,inodeListMem,blockMem);
    createFile(target,out_creat,superBlock,inodeListMem,blockMem);
    readFile(source,out_read,superBlock,inodeListMem,blockMem);
    writeFile(target,out_write,out_read,superBlock,inodeListMem,blockMem);
}
void copy_folder(char source[100],char target[100],std::vector<std::string>P,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    std::string out_creat;
    createDirectory(target,out_creat,P,superBlock,inodeListMem,blockMem);
}
bool detect_if_copy(char path[100]){
    char path_temp[100]="";
    strcpy(path_temp, path);
    const char delimiter[2] = "/";
    char *directory;
    directory = strtok(path, delimiter);
    strcpy(path, path_temp);
    if((std::string)directory!="COPY"){
        return true;
    }
    
    else {return false;}
}

bool path_exist(char path[100],int f_type,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    char path_temp[100]="";
    strcpy(path_temp,path);
    int i, j, flag;
    //std::ostringstream output;
    //char path[100];
    char *directory, *parent, *target;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针
    //char *blockBitmap=&superBlock.blockBitmap[0];

    std::unordered_map<int, int> inodeListid_to_MemIndex=superBlock.inodeListid_to_MemIndex;

    // Access the pointer of parent directory recursively
    parent = NULL;
    directory = strtok(path, delimiter);
    // printf("AAAdirectory%s\n",directory);

    //找到上一级目录的位置
    while (directory != NULL) {
        if (parent != NULL) //上一级不是根目录
        {
            block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
            flag = 0;
            for (i = 0; i < ENTRY_NUMBER; i++) { 
                if (strcmp(block->fileName[i], parent) == 0) //找到父目录的位置
                {   
                    int inode_temp=block->inodeID[i];
                    int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                    int offset=inodeID_to_inodeListID(inode_temp)[0];
                    int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
                    if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                        flag=1;
                        pointer=&inodeListMem[Mem_index].inodeMem[offset];
                        break;
                    }
                                    
                }
            }
            if (flag == 0) {
                // If the parent directory does not exist or is not a directory file
                // output<<"The path does not exist!\n";
                // outputString = output.str();
                //printf("The path does not exist!\n");
                strcpy(path,path_temp);
                return false;
            }
        }
        parent = directory;
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }
    //最后出来的时候，要创建的目录就是parent；point指向父目录

    // Create the target directory
    int entry = -1;//块内第几项
    int n_block = -1;//第几块放置
    int n_inode = -1;//inode#
    int flag_2=0;
    target = parent;
    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//父block
    //找到对应n_inode，并对父块的设置
    for (i = 0; i < ENTRY_NUMBER; i++) {
        if(strcmp(block->fileName[i], target) == 0){
            n_inode=block->inodeID[i];
            int inodeListID=inodeID_to_inodeListID(n_inode)[1];
            int offset=inodeID_to_inodeListID(n_inode)[0];
            int Mem_index=inodeListid_to_MemIndex[inodeListID];
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==f_type){
                flag_2=1;
                strcpy(path,path_temp);
                return true;
            }
        }
    }
    if(flag_2==0){
        strcpy(path,path_temp);
        return false;
    }
}

void writeLog(char path[100],int operation,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    char LogPath[100]="/COPY/log";
    std::string strPath(path);
    std::string logContent;
    std::string output;
    std::string allContent;
    std::string thisContent=std::to_string(operation)+","+strPath+";";
    readFile(LogPath,logContent,superBlock,inodeListMem,blockMem);
    allContent=thisContent+logContent;
    // if(allContent.size()>BLOCK_SIZE){
    //     std::cout<<"1"<<std::endl;
    // }
    //readFile(LogPath,logContent,superBlock,inodeListMem,blockMem);
    // if (!allContent.empty() && allContent[allContent.length() - 1] == '\n') {
    //     allContent.erase(allContent.length() - 1);
    // }
    allContent+='$';
    if(allContent.size()>BLOCK_SIZE){
        deleteFile(LogPath,output,superBlock,inodeListMem,blockMem);
        createFile(LogPath,output,superBlock,inodeListMem,blockMem);
    }
    writeFile(LogPath,output,allContent,superBlock,inodeListMem,blockMem);
}

void writeLog_for_DB(int operation,int ID,std::string password,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    std::string info=std::to_string(operation)+","+std::to_string(ID)+","+password+";";
    std::string logContent;
    std::string allContent;
    std::string output;
    char LogPath[100]="/COPY/log";
    readFile(LogPath,logContent,superBlock,inodeListMem,blockMem);
    allContent=info+logContent;
    allContent+='$';
    if(allContent.size()>BLOCK_SIZE){
        deleteFile(LogPath,output,superBlock,inodeListMem,blockMem);
        createFile(LogPath,output,superBlock,inodeListMem,blockMem);
    }
    writeFile(LogPath,output,allContent,superBlock,inodeListMem,blockMem);
}

void Copy_Operation(char path[100],int f_type,int operation,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    if(path[0]!='/'){
        std::string usedPath(path);
        usedPath='/'+usedPath;
        string_to_char100(usedPath,path);
    }
    
    if(!path_exist(path,f_type,superBlock,inodeListMem,blockMem)){
        return;//放到下一个函数里处理报错
    }  
    //--------------文件拷贝操作-----------
    //确定文件夹名命名方式：查hash表,写hash表
    int fileconsist=0;
    char target[100];
    int max_intName=-1;
    std::map<std::string, int> hash;
    char hashPath[100]="/COPY/hash";
    //string_to_char100(str_hashPath,hashPath);
    file_to_hash(hash,hashPath,superBlock,inodeListMem,blockMem);
    std::string str_f_type=std::to_string(f_type);
    std::string strPath(path);
    strPath=str_f_type+strPath;
    // char path_for_hash[100];
    // string_to_char100(strPath,path_for_hash);
    auto it = hash.find(strPath);
    if(it!=hash.end()){
        fileconsist=1;
        std::string strTarget="/COPY/copy_files/";
        strTarget+=std::to_string(hash[strPath]);
        string_to_char100(strTarget,target);
    }
    else{
        for (const auto& pair : hash) {
            if(pair.second>max_intName){
                max_intName=pair.second;
            }
        }
        max_intName++;
        hash[strPath]=max_intName;
        std::string strTarget="/COPY/copy_files/";
        strTarget+=std::to_string(max_intName);
        
        string_to_char100(strTarget,target);
        //char hashPath2[100]="/COPY/hash";
        hash_to_file(hash,hashPath,superBlock,inodeListMem,blockMem);
        if(f_type==0){
            std::vector<std::string>P(3);
            // P[2]="11111111";
            // P[1]="11111111";
            // P[0]="11111111";
            get_Privilige_by_Path(path,f_type,P,superBlock,inodeListMem,blockMem);
            copy_folder(path,target,P,superBlock,inodeListMem,blockMem);
        }
        if(f_type==1){
            copy_file(path,target,superBlock,inodeListMem,blockMem);
        }
    }


    //--------------写日志操作--------------
    //在尾部写文件的操作
    //按相应格式写
    writeLog(path,operation,superBlock,inodeListMem,blockMem);
    // std::cout<<"COPY!"<<std::endl;
}


void Copy_Operation_For_Create(char path[100],int f_type,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    if(path[0]!='/'){
        std::string usedPath(path);
        usedPath='/'+usedPath;
        string_to_char100(usedPath,path);
    }
    writeLog(path,f_type,superBlock,inodeListMem,blockMem);
}

void start_copy(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    char copy_files_path[100]="/COPY/copy_files";
    char hash_path[100]="/COPY/hash";
    char log_path[100]="/COPY/log";
    std::vector<std::string> p(3);
    p[0]="11111110";
    p[1]="11111110";
    p[2]="11111110";
    std::string temp_output;
    deleteDirectory(copy_files_path,temp_output,superBlock,inodeListMem,blockMem);
    createDirectory(copy_files_path,temp_output,p,superBlock,inodeListMem,blockMem);
    deleteFile(hash_path,temp_output,superBlock,inodeListMem,blockMem);
    createFile(hash_path,temp_output,superBlock,inodeListMem,blockMem);
    deleteFile(log_path,temp_output,superBlock,inodeListMem,blockMem);
    createFile(log_path,temp_output,superBlock,inodeListMem,blockMem);
}


void readLog(std::vector<std::vector<std::string>>&log,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    char path[100]="/COPY/log";
    std::string logContent;
    readFile(path,logContent,superBlock,inodeListMem,blockMem);
    
    std::istringstream logContent_Iss(logContent);
    std::string part;
    while (std::getline(logContent_Iss, part, ';')) {
        std::vector<std::string> log_part;
        std::istringstream subIss(part);
        std::string subPart;
        while (std::getline(subIss, subPart, ',')) {
            log_part.push_back(subPart);
        }
        log.push_back(log_part);
    }
}

std::string getCopyFile(char path[100],int f_type,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    std::string pathStr(path);
    std::string hashKey=std::to_string(f_type)+pathStr;
    std::map<std::string, int> hash;
    char hashPath[100]="/COPY/hash";
    file_to_hash(hash,hashPath,superBlock,inodeListMem,blockMem);
    int copy_file=hash[hashKey];
    return "/COPY/copy_files/"+std::to_string(copy_file);
}

void deleteUser_in_DB(){}
void recoverPassword_in_DB(){}

void recover_From_Log(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],MYSQL* mysql){
    std::vector<std::vector<std::string>>log;
    readLog(log,superBlock,inodeListMem,blockMem);
    for(int i=0;i<log.size();++i){
        int operation=std::stoi(log[i][0]);
        std::string pathStr=log[i][1];
        char path[100];
        std::string output;
        string_to_char100(pathStr,path);
        switch (operation){
            case 0://creatD
            {
                deleteDirectory(path,output,superBlock,inodeListMem,blockMem);
                break;
            }
            case 1://creatF
            {
                deleteFile(path,output,superBlock,inodeListMem,blockMem);
                break;
            }
            case 2://deleteF
            {
                std::string copyPathStr=getCopyFile(path,1,superBlock,inodeListMem,blockMem);
                char copyPath[100];
                string_to_char100(copyPathStr,copyPath);
                copy_file(copyPath,path,superBlock,inodeListMem,blockMem);
                break;
            }
            case 3://deleteSF
            {
                std::string copyPathStr=getCopyFile(path,1,superBlock,inodeListMem,blockMem);
                char copyPath[100];
                string_to_char100(copyPathStr,copyPath);
                copy_file(copyPath,path,superBlock,inodeListMem,blockMem);
                break;
            }
            case 4://deleteSD
            {
                std::string copyPathStr=getCopyFile(path,0,superBlock,inodeListMem,blockMem);
                char copyPath[100];
                string_to_char100(copyPathStr,copyPath);
                std::vector<std::string>P(3);
                get_Privilige_by_Path(copyPath,0,P,superBlock,inodeListMem,blockMem);
                copy_folder(copyPath,path,P,superBlock,inodeListMem,blockMem);
                break;
            }
            case 6://writeF
            {
                std::string copyPathStr=getCopyFile(path,1,superBlock,inodeListMem,blockMem);
                char copyPath[100];
                string_to_char100(copyPathStr,copyPath);
                copy_file(copyPath,path,superBlock,inodeListMem,blockMem);
                break;
            }
            case 7://add user
            {
                std::string ID=log[i][1];
                std::string password=log[i][2];
                delete_user(mysql,ID);
                break;
            }
            case 8://change password
            {
                std::string ID=log[i][1];
                std::string password=log[i][2];
                change_password(mysql,ID,password);
                break;
            }
            default:
                printf("Please input a valid command number\n");
                break;

        }
    }
    start_copy(superBlock,inodeListMem,blockMem);
}



void CreateD(char path[100],std::string &outputString ,std::vector<std::string>P,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    if(detect_if_copy(path)){
        // Copy_Operation(path,outputString,0,superBlock,inodeListMem,blockMem);
        Copy_Operation_For_Create(path,0,superBlock,inodeListMem,blockMem);
        //inodeExtend(superBlock,inodeListMem,blockMem);
        createDirectory(path,outputString,P,superBlock,inodeListMem,blockMem);
    }
    else{
        std::cout<<"Use wrong function!"<<std::endl;
    }
}

void CreateF(char path[100],std::string &outputString ,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    if(detect_if_copy(path)){
        //Copy_Operation(path,outputString,1,superBlock,inodeListMem,blockMem);
        Copy_Operation_For_Create(path,1,superBlock,inodeListMem,blockMem);
        //inodeExtend(superBlock,inodeListMem,blockMem);
        createFile(path,outputString,superBlock,inodeListMem,blockMem);
    }
    else{
        std::cout<<"Use wrong function!"<<std::endl;
    }
}
void DeleteF(char path[100],std::string &outputString ,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    if(detect_if_copy(path)){
        Copy_Operation(path,1,2,superBlock,inodeListMem,blockMem);
        deleteFile(path,outputString,superBlock,inodeListMem,blockMem);
    }
    else{
        std::cout<<"Use wrong function!"<<std::endl;
    }
}
void WriteF(char path[100],std::string &outputString,std::string inputLine ,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
    if(detect_if_copy(path)){
        Copy_Operation(path,1,6,superBlock,inodeListMem,blockMem);
        writeFile(path,outputString,inputLine,superBlock,inodeListMem,blockMem);
    }
    else{
        std::cout<<"Use wrong function!"<<std::endl;
    }
}

struct DirectoryBlock * DeleteSingleD(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],struct DirectoryBlock *block,char path[100]){
   if(detect_if_copy(path)){
        std::string outputString;
        Copy_Operation(path,0,4,superBlock,inodeListMem,blockMem);
        return deleteSingleDir(superBlock,inodeListMem,blockMem,block,path);
    }
    else{
        std::cout<<"Use wrong function!"<<std::endl;
    }
}
void DeleteSingleF(SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],char path[100]){
   if(detect_if_copy(path)){
        std::string outputString;
        
        Copy_Operation(path,1,3,superBlock,inodeListMem,blockMem);
        deleteSingleFile(superBlock,inodeListMem,blockMem,path);
    }
    else{
        std::cout<<"Use wrong function!"<<std::endl;
    }
}

struct DirectoryBlock * DFS_notCopy(std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER],struct DirectoryBlock * block,char parent[100]){
    //printf("刚进来的parent%s\n",parent);
    std::ostringstream output;  
    for(int i=0;i<ENTRY_NUMBER;++i){
        //std::cout<<"block->inodeID[i]:"<<block->inodeID[i]<<std::endl;
        if(block->inodeID[i] != -1){
            char* relat_name=block->fileName[i];
            //std::cout<<"relat_name:"<<relat_name<<std::endl;
            strcat(parent,"/");
            strcat(parent,relat_name);


            int inode_temp=block->inodeID[i];
            int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
            int offset=inodeID_to_inodeListID(inode_temp)[0];
            int Mem_index=superBlock.inodeListid_to_MemIndex[inodeList_temp];
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==0)//保证type是文件夹
            {   
                struct DirectInode *pointer = &inodeListMem[Mem_index].inodeMem[offset];
                block = (struct DirectoryBlock *) &blockMem[pointer->blockID];
                block=DFS_notCopy(outputString,superBlock,inodeListMem,blockMem,block,parent);
                //printf("BACK:%s\n",parent);
            }
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==1)//保证type是文件
            {   
                output<<"FILE DELETE:"<<parent<<std::endl;
                //printf("FILE DELETE:%s\n",parent);
                DeleteSingleF(superBlock,inodeListMem,blockMem,parent);
            }
        }
    }
    output<<"FOLDER DELETE:"<<parent<<std::endl;
    //printf("FOLDER DELETE:%s\n",parent);
    block=DeleteSingleD(superBlock,inodeListMem,blockMem,block,parent);
    outputString += output.str();
    return block;
}

void DeleteDir(char path[100],std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]) {
    char path_temp[100]="";
    strcpy(path_temp,path);
    int i, j, flag;
    std::ostringstream output;
    //char path[100];
    char *directory, *parent, *target;
    const char delimiter[2] = "/";
    struct DirectInode *pointer=&inodeListMem[0].inodeMem[0];
    struct DirectoryBlock *block;//指向block的指针
    //char *blockBitmap=&superBlock.blockBitmap[0];

    std::unordered_map<int, int> inodeListid_to_MemIndex=superBlock.inodeListid_to_MemIndex;

    // printf("Input the path:\n");
    // scanf("%s", path);//绝对路径
    char paths[100]="";
    strcpy(paths, path);
    // Access the pointer of parent directory recursively
    parent = NULL;
    directory = strtok(path, delimiter);
    // printf("AAAdirectory%s\n",directory);

    //找到上一级目录的位置
    while (directory != NULL) {
        if (parent != NULL) //上一级不是根目录
        {
            block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//一级一级往下找
            flag = 0;
            for (i = 0; i < ENTRY_NUMBER; i++) { 
                if (strcmp(block->fileName[i], parent) == 0) //找到父目录的位置
                {   
                    int inode_temp=block->inodeID[i];
                    int inodeList_temp=inodeID_to_inodeListID(inode_temp)[1];
                    int offset=inodeID_to_inodeListID(inode_temp)[0];
                    int Mem_index=inodeListid_to_MemIndex[inodeList_temp];
                    if(inodeListMem[Mem_index].inodeMem[offset].fileType==0){
                        flag=1;
                        pointer=&inodeListMem[Mem_index].inodeMem[offset];
                        break;
                    }
                                    
                }
            }
            if (flag == 0) {
                // If the parent directory does not exist or is not a directory file
                output<<"The path does not exist!\n";
                outputString = output.str();
                //printf("The path does not exist!\n");
                return;
            }
        }
        parent = directory;
        directory = strtok(NULL, delimiter);
        // printf("BBBdirectory%s\n",directory);
    }
    //最后出来的时候，要创建的目录就是parent；point指向父目录
    
    block = (struct DirectoryBlock *) &blockMem[pointer->blockID];//父block
    int parent_blockID=pointer->blockID;
    target=parent;
    for(i=0;i<ENTRY_NUMBER;++i){
        if(strcmp(block->fileName[i], target) == 0){
            int n_inode=block->inodeID[i];
            int inodeListID=inodeID_to_inodeListID(n_inode)[1];
            int offset=inodeID_to_inodeListID(n_inode)[0];
            int Mem_index=inodeListid_to_MemIndex[inodeListID];
            if(inodeListMem[Mem_index].inodeMem[offset].fileType==0)//保证type是文件
            { 
                pointer=&inodeListMem[Mem_index].inodeMem[offset];
                block = (struct DirectoryBlock *) &blockMem[pointer->blockID];
                //printf("isok!\nblockID%d\n",pointer->blockID);
                break;
            }
        }
    }
    if(i==ENTRY_NUMBER){
        output<<"Can't Find The Folder.\n";
        outputString = output.str();
        //printf("Can't Find The Folder.\n");
        return ;
    }
    // map<char*,char*>absolute_path;
    // absolute_path.insert(make_pair(target, path));
    //printf("paths:%s\n",paths);
    //std::cout<<"n_block:"<<pointer->blockID<<std::endl;
    std::string dfs_string;
    DFS_notCopy(dfs_string,superBlock,inodeListMem,blockMem,block,paths);
    //output<<"HHHHHHHHHHHHHHHHHH\n";
    //DFS(dfs_string,superBlock,inodeListMem,blockMem,block,paths);
    //output<<"AAAAAAAAAAAAAAAAAA\n";
    outputString = output.str()+dfs_string;

    strcpy(path,path_temp);
}

void DeleteD(char path[100],std::string& outputString,SuperBlock &superBlock,struct InodeList inodeListMem[MAX_INODELIST_NUMBER],struct FileBlock blockMem[BLOCK_NUMBER]){
   if(detect_if_copy(path)){
        DeleteDir(path,outputString,superBlock,inodeListMem,blockMem);
    }
    else{
        std::cout<<"Use wrong function!"<<std::endl;
    }
}




// int main() 
// {
//     // printf("Size of SuperBlock: %zu bytes\n", sizeof(struct SuperBlock));
//     // printf("Size of InodeList: %zu bytes\n", sizeof(struct InodeList));
//     // printf("Size of DirectInode: %zu bytes\n", sizeof(struct DirectInode));
//     // printf("Size of IndirectInode: %zu bytes\n", sizeof(struct IndirectInode));
//     // printf("Size of DirectoryBlock: %zu bytes\n", sizeof(struct DirectoryBlock));
//     // printf("Size of FileBlock: %zu bytes\n", sizeof(struct FileBlock));
    
//     //初始化部分
//     SuperBlock superBlock;
//     struct InodeList inodeListMem[MAX_INODELIST_NUMBER];//inodelist空间,一共有124个inodelist   //212*124  //有层次结构，不需要用bitmap
//     struct FileBlock blockMem[BLOCK_NUMBER];//存block的空间，一共有128个block  //512*1120
//     init_inode(superBlock,inodeListMem,blockMem);

//     // //测试inode拓展部分的代码
//     // for(int i=0;i<11;++i){
//     //     inodeExtend(superBlock,inodeListMem,blockMem);
//     //     createDirectory(superBlock,inodeListMem,blockMem);
//     // }

//     // char path[100];
//     // std::cout<<"Input the path:"<<std::endl;
//     // scanf("%s", path);
//     // std::string s;
//     // // Copy_Operation(path,s,1,superBlock,inodeListMem,blockMem);



//     // //大批量测试用例
//     // std::string outputString1;
//     // std::vector<std::string> P1(3);
//     // P1[0]="11101110";
//     // P1[1]="11001011";
//     // P1[2]="01101100";


//     // char a[100]="/d";
//     // createDirectory(a,outputString1,P1,superBlock,inodeListMem,blockMem);
//     // char a1[100]="/d/f1";
//     // createFile(a1,outputString1,superBlock,inodeListMem,blockMem);
//     // char a2[100]="/d/d1";
//     // createDirectory(a2,outputString1,P1,superBlock,inodeListMem,blockMem);


//     // // char a3[100]="/d/d3";
//     // // CreateD(a3,outputString1,P1,superBlock,inodeListMem,blockMem);
//     // // char a4[100]="/d/f1";
//     // // CreateF(a4,outputString1,superBlock,inodeListMem,blockMem);
//     // // char a5[100]="/d/d1/f2";
//     // // CreateF(a5,outputString1,superBlock,inodeListMem,blockMem);
//     // // char a6[100]="/d/d2/d3";
//     // // CreateD(a6,outputString1,P1,superBlock,inodeListMem,blockMem);
//     // // char a7[100]="/d/d2/d3/d4";
//     // // CreateD(a7,outputString1,P1,superBlock,inodeListMem,blockMem);
//     // // char a8[100]="/d/d2/f3";
//     // // CreateF(a8,outputString1,superBlock,inodeListMem,blockMem);
//     // // char a9[100]="/d/d2/d3/f4";
//     // // CreateF(a9,outputString1,superBlock,inodeListMem,blockMem);

//     // for(int i=0;i<30;i++){
//     //     char aa[100];
//     //     std::string str=std::to_string(i);
//     //     string_to_char100(str,aa);
//     //     CreateD(aa,str,P1,superBlock,inodeListMem,blockMem);        
//     // }
//     // //start_copy(superBlock,inodeListMem,blockMem);
//     // for(int i=0;i<30;i++){
//     //     char aa[100];
//     //     std::string str="/1/"+std::to_string(i);
//     //     string_to_char100(str,aa);
//     //     CreateD(aa,str,P1,superBlock,inodeListMem,blockMem);        
//     // }
//     // //start_copy(superBlock,inodeListMem,blockMem);
//     // for(int i=0;i<30;i++){
//     //     char aa[100];
//     //     std::string str="/2/"+std::to_string(i);
//     //     string_to_char100(str,aa);
//     //     CreateD(aa,str,P1,superBlock,inodeListMem,blockMem);        
//     // }
//     // // //start_copy(superBlock,inodeListMem,blockMem);
//     // for(int i=0;i<30;i++){
//     //     if(i==22){
//     //         std::cout<<"im here!"<<std::endl;
//     //     }
//     //     char aa[100];
//     //     std::string str="/3/"+std::to_string(i);
//     //     string_to_char100(str,aa);
//     //     CreateD(aa,str,P1,superBlock,inodeListMem,blockMem);        
//     // }

//     // // std::string cccout;
//     // // readFile(log,cccout,superBlock,inodeListMem,blockMem);
//     // // char a[100]="/d";
//     // // createDirectory(a,outputString1,P1,superBlock,inodeListMem,blockMem);
//     // // char a1[100]="/d/d1";
//     // // createDirectory(a1,outputString1,P1,superBlock,inodeListMem,blockMem);
//     // // char a2[100]="/d/d2";
//     // // createDirectory(a2,outputString1,P1,superBlock,inodeListMem,blockMem);
//     // // char a3[100]="/d/d3";
//     // // createDirectory(a3,outputString1,P1,superBlock,inodeListMem,blockMem);
//     // // char a4[100]="/d/f1";
//     // // createFile(a4,outputString1,superBlock,inodeListMem,blockMem);
//     // // char a5[100]="/d/d1/f2";
//     // // createFile(a5,outputString1,superBlock,inodeListMem,blockMem);
//     // // char a6[100]="/d/d2/d3";
//     // // createDirectory(a6,outputString1,P1,superBlock,inodeListMem,blockMem);
//     // // char a7[100]="/d/d2/d3/d4";
//     // // createDirectory(a7,outputString1,P1,superBlock,inodeListMem,blockMem);
//     // // char a8[100]="/d/d2/f3";
//     // // createFile(a8,outputString1,superBlock,inodeListMem,blockMem);
//     // // char a9[100]="/d/d2/d3/f4";
//     // // createFile(a9,outputString1,superBlock,inodeListMem,blockMem);



//     int running = 1;
//     int choice;
//     while (running == 1) {
//         //每次运行均检查是否需要拓展inode

//         //inodeExtend(superBlock,inodeListMem,blockMem);
//         printf("The system supports following commands:\n"
//                "1. Create a directory\n"
//                "2. Delete a directory\n"
//                "3. List Files\n"
//                "4. Create a file\n"
//                "5. Delete a file\n"
//                "6. Read a file\n"
//                "7. Write a file\n"
//                "9. Exit\n"
//                "Input your command number:\n"
//                );
//         scanf("%i", &choice);
//         char path[100];
//         std::cout<<"Input the path:"<<std::endl;
//         scanf("%s", path);
//         std::string outputString;
//         std::string inputLine;

//         switch (choice)
//         {
//         case 1:{
//             std::cout<<"input vector string:"<<std::endl;
//             std::vector<std::string> P(3);
//             // P[0]="11101010";
//             // P[1]="11101010";
//             // P[2]="11101010";
//             std::cin>>P[0]>>P[1]>>P[2];
//             //createDirectory(path,outputString,P,superBlock,inodeListMem,blockMem);
            
//             CreateD(path,outputString,P,superBlock,inodeListMem,blockMem);
//             break;
//         }
//         case 2:
//             //deleteDirectory(path,outputString,superBlock,inodeListMem,blockMem);
//             DeleteD(path,outputString,superBlock,inodeListMem,blockMem);
//             break;

//         case 3:
//             listFiles(path,outputString,superBlock,inodeListMem,blockMem);
//             //ls_only_name(path,outputString,0,superBlock,inodeListMem,blockMem);
//             break;

//         case 4:
//             //createFile(path,outputString,superBlock,inodeListMem,blockMem);
//             CreateF(path,outputString,superBlock,inodeListMem,blockMem);
//             break;

//         case 5:
//             //deleteFile(path,outputString,superBlock,inodeListMem,blockMem);
//             DeleteF(path,outputString,superBlock,inodeListMem,blockMem);
//             break;

//         case 6:
//             readFile(path,outputString,superBlock,inodeListMem,blockMem);
//             break;

//         case 7:
//             std::cout<<"Input the file:"<<std::endl;
//             std::getline(std::cin, inputLine, '$');
//             //writeFile(path,outputString,inputLine,superBlock,inodeListMem,blockMem);
//             WriteF(path,outputString,inputLine,superBlock,inodeListMem,blockMem);
//             break;

//         case 8:
//         {
//             start_copy(superBlock,inodeListMem,blockMem);
//             break;
//         }
//         case 10:
//             writeLog_for_DB(8,64700141,"im the best",superBlock,inodeListMem,blockMem);
//             writeLog_for_DB(7,64690213,"hihihihi",superBlock,inodeListMem,blockMem);
//             break;

//         case 11:
//             recover_From_Log(superBlock,inodeListMem,blockMem);
//             break;

//         case 9:
//             exit(0);
        
//         default:
//             printf("Please input a valid command number\n");
//             break;
//         }
//         std::cout<<"path:"<<path<<std::endl;
//         std::cout<<outputString;
//     }    
//     return 0;
// }
