#ifndef txt_H
#define txt_H
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

bool init_file(const std::string& fileName,std::streampos desiredSize);

bool isSuperBlockEmpty(const std::string& filename, int N);

std::streamsize getFileSize(const std::string& fileName);

void readBytesFromPosition(const std::string& inputFileName, std::streampos bytePosition, char* buffer, std::streamsize bufferSize) ;

void writeByteAtPosition(const std::string& fileName, std::streampos position,const char* buffer, std::streamsize bufferSize) ;

void readIntFromPosition(const std::string& fileName, int& data,std::streampos bytePosition);

void writeIntAtPosition(const std::string& fileName, const int& data,std::streampos position);

#endif