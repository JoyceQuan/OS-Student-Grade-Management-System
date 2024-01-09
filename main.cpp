#include "inode.h"
using namespace std;
int main() 
{
    //初始化部分
    SuperBlock superBlock;
    struct InodeList inodeListMem[MAX_INODELIST_NUMBER];//inodelist空间,一共有124个inodelist   //212*124  //有层次结构，不需要用bitmap
    struct FileBlock blockMem[BLOCK_NUMBER];//存block的空间，一共有128个block  //512*1120
    init_inode(superBlock,inodeListMem,blockMem);

    int running = 1;
    int choice;
    while (running == 1) {
        //每次运行均检查是否需要拓展inode

        //inodeExtend(superBlock,inodeListMem,blockMem);
        printf("The system supports following commands:\n"
               "1. Create a directory\n"
               "2. Delete a directory\n"
               "3. List Files\n"
               "4. Create a file\n"
               "5. Delete a file\n"
               "6. Read a file\n"
               "7. Write a file\n"
               "9. Exit\n"
               "Input your command number:\n"
               );
        scanf("%i", &choice);
        char path[100];
        std::cout<<"Input the path:"<<std::endl;
        scanf("%s", path);
        std::string outputString;
        std::string inputLine;

        switch (choice)
        {
        case 1:{
            std::cout<<"input vector string:"<<std::endl;
            std::vector<std::string> P(3);
            // P[0]="11101010";
            // P[1]="11101010";
            // P[2]="11101010";
            std::cin>>P[0]>>P[1]>>P[2];
            //createDirectory(path,outputString,P,superBlock,inodeListMem,blockMem);
            
            CreateD(path,outputString,P,superBlock,inodeListMem,blockMem);
            break;
        }
        case 2:
            //deleteDirectory(path,outputString,superBlock,inodeListMem,blockMem);
            DeleteD(path,outputString,superBlock,inodeListMem,blockMem);
            break;

        case 3:
            listFiles(path,outputString,superBlock,inodeListMem,blockMem);
            //ls_only_name(path,outputString,0,superBlock,inodeListMem,blockMem);
            break;

        case 4:
            //createFile(path,outputString,superBlock,inodeListMem,blockMem);
            CreateF(path,outputString,superBlock,inodeListMem,blockMem);
            break;

        case 5:
            //deleteFile(path,outputString,superBlock,inodeListMem,blockMem);
            DeleteF(path,outputString,superBlock,inodeListMem,blockMem);
            break;

        case 6:
            readFile(path,outputString,superBlock,inodeListMem,blockMem);
            break;

        case 7:
            std::cout<<"Input the file:"<<std::endl;
            std::getline(std::cin, inputLine, '$');
            //writeFile(path,outputString,inputLine,superBlock,inodeListMem,blockMem);
            WriteF(path,outputString,inputLine,superBlock,inodeListMem,blockMem);
            break;

        case 8:
        {
            start_copy(superBlock,inodeListMem,blockMem);
            break;
        }
        case 10:
            writeLog_for_DB(8,64700141,"im the best",superBlock,inodeListMem,blockMem);
            writeLog_for_DB(7,64690213,"hihihihi",superBlock,inodeListMem,blockMem);
            break;

        case 11:
            recover_From_Log(superBlock,inodeListMem,blockMem);
            break;

        case 9:
            exit(0);
        
        default:
            printf("Please input a valid command number\n");
            break;
        }
        std::cout<<"path:"<<path<<std::endl;
        std::cout<<outputString;
    }    
    return 0;
}
