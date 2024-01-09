// #include <iostream>
// #include <fstream>
// #include <cstdlib>
// #include <cstring>
#include "txt.h"


// void initializeFile(const std::string& fileName, std::streamsize fileSize) {
//     std::ofstream outputFile(fileName, std::ios::binary);
//     if (!outputFile) {
//         std::cerr << "Error opening file: " << fileName << std::endl;
//         exit(EXIT_FAILURE);
//     }

//     // 定位到文件末尾
//     outputFile.seekp(fileSize - 1, std::ios::beg);

//     // 写入一个字节，以便创建指定大小的文件
//     outputFile.put(0);

//     // 关闭输出文件
//     outputFile.close();
// }

bool init_file(const std::string& filename,std::streampos desiredSize){
    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out | std::ios::ate); // Open for reading and appending, create if not exists

    if (!file.is_open()) {
        std::cout << "File does not exist. Creating a new file." << std::endl;
        file.open(filename, std::ios::out);  // Open for writing, creates if not exists
    }

    if (!file.is_open()) {
        std::cout << "Error opening the file." << std::endl;
        return false;
    }

    // Determine the current file size
    std::streampos fileSize = file.tellg(); // Get the current position (which is the file size)

    std::cout << "Current file size: " << fileSize << " bytes" << std::endl;

    if (fileSize < desiredSize) {
        std::cout << "Resizing the file to " << desiredSize << " bytes..." << std::endl;

        // Move the file pointer to the desired size and write a byte to extend the file
        file.seekp(desiredSize-1, std::ios::beg);
        file.put('\0');
    }
    else if(fileSize > desiredSize){
        std::cout<<"Something is WRONG!\nPROBLEM:fileSize > desiredSize\n";
    }

    file.close();
    return true;
}

bool isSuperBlockEmpty(const std::string& filename, int N) {
    std::ifstream file(filename,std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening the file: " << filename << std::endl;
        return false; // 无法打开文件，返回false
    }

    char ch;
    int count = 0;
    while (count < N && file.get(ch)) {
        if (ch!='\0') {
            // 前N个字符中包含非空白字符
            return false;
        }
        ++count;
    }
    file.close();
    // 文件前N个字符为空或只包含空白字符
    return true;
}


std::streamsize getFileSize(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }

    return file.tellg();
}

void readBytesFromPosition(const std::string& inputFileName, std::streampos bytePosition, char* buffer, std::streamsize bufferSize) {
    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening input file: " << inputFileName << std::endl;
        exit(EXIT_FAILURE);
    }

    // 定位到指定的字节位置
    inputFile.seekg(0, std::ios::beg);
    inputFile.seekg(bytePosition, std::ios::beg);

    // 读取字节
    std::streamsize bytesRead = inputFile.readsome(buffer, bufferSize);
    //inputFile.read(buffer, bufferSize);

    // 关闭输入文件
    inputFile.close();
    // for (std::streamsize i = bytesRead; i < bufferSize; ++i) {
    //     buffer[i] = '\0';
    // }

    // 将读取的字节写入标准输出
    //std::cout.write(buffer, bufferSize);
}

void writeByteAtPosition(const std::string& fileName, std::streampos position,const char* buffer, std::streamsize bufferSize) {
    std::ofstream outputFile(fileName, std::ios::binary | std::ios::ate | std::ios::in | std::ios::out);

    if (!outputFile) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }

    // 获取文件大小
    outputFile.seekp(0, std::ios::end);
    std::streampos fileSize = outputFile.tellp();

    // 检查要写入的位置是否超过文件大小
    if (position >= fileSize) {
        std::cerr << "Error: Attempting to write beyond end of file." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 移动文件指针到指定位置
    outputFile.seekp(position, std::ios::beg);

    //std::streamsize actualSize = strlen(buffer);

    // 写入数据
    outputFile.write(buffer,bufferSize);

    // 将剩余部分填充为空字符
    // for (std::streamsize i = actualSize; i < bufferSize; ++i) {
    //     outputFile.put('\0');
    // }

    // 关闭文件
    outputFile.close();
}

// 从文件读取int数据

void readIntFromPosition(const std::string& fileName, int& data,std::streampos bytePosition) {
    //T data;

    std::ifstream inputFile(fileName, std::ios::binary | std::ios::in);

    if (!inputFile) {
        std::cerr << "Error opening input file: " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }

    // 定位到指定的字节位置
    inputFile.seekg(0, std::ios::beg);
    inputFile.seekg(bytePosition, std::ios::beg);

    // 读取数据
    std::streamsize bytesRead = inputFile.readsome(reinterpret_cast<char*>(&data), sizeof(int));

    // 关闭文件
    inputFile.close();


}

// 写入int数据到文件

void writeIntAtPosition(const std::string& fileName, const int& data,std::streampos position) {
    std::ofstream outputFile(fileName, std::ios::binary | std::ios::ate | std::ios::in | std::ios::out);

    if (!outputFile) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }

    // 获取文件大小
    outputFile.seekp(0, std::ios::end);
    std::streampos fileSize = outputFile.tellp();

    // 检查要写入的位置是否超过文件大小
    if (position >= fileSize) {
        std::cerr << "Error: Attempting to write beyond end of file." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 移动文件指针到指定位置
    outputFile.seekp(position, std::ios::beg);

    // 写入数据
    outputFile.write(reinterpret_cast<const char*>(&data), sizeof(int));

    // 关闭文件
    outputFile.close();
}




// int main() {
//     // ////init_file("disk.txt",10000);
//     // ////std::cout<<isSuperBlockEmpty("disk.txt",160)<<std::endl;

//     // const std::string inputFileName = "input.txt";
//     // const std::streampos bytePosition = 3; // 将命令行参数转换为长整型
//     // const std::streamsize bufferSize = 1024; // 将命令行参数转换为长整型
//     // const std::string outputFileName = "output.txt";

//     // // //initializeFile(inputFileName,100);
//     // init_file(outputFileName,10000);
//     // std::streamsize outfileSize =getFileSize(outputFileName);

//     // std::streamsize fileSize = getFileSize(inputFileName);

//     // // 检查是否指定的字节位置在文件范围内
//     // if (bytePosition >= fileSize) {
//     //     std::cerr << "Byte position is beyond the end of the file." << std::endl;
//     //     return 1;
//     // }

//     // // 为读取分配缓冲区
//     // char buffer [bufferSize];

//     // // 读取字节
//     // readBytesFromPosition(inputFileName, 0, buffer, bufferSize);


//     // char buffers[bufferSize];
//     // // //将读取的字节写入标准输出
//     // std::cin.read(buffers, bufferSize);
//     // // 获取实际读取的字符数
//     // std::streamsize bytesRead = std::cin.gcount();

//     // // 将未使用的部分填充为空字符
//     // for (std::streamsize i = bytesRead; i < bufferSize; ++i) {
//     //     buffers[i] = '\0';
//     // }
//     // // // 将字节写入文件
//     // // writeBytesToFile(outputFileName, bytePosition,buffer, bufferSize);

//     // writeByteAtPosition(outputFileName,bytePosition,buffers,bufferSize);


//     // return 0;
// }

