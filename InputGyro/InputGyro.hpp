//
//  InputGyro.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/03/26.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef InputGyro_hpp
#define InputGyro_hpp

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <vector>
#include <iostream>
#include <fstream>

class InputGyro
{
public:
    InputGyro(const std::string& fileName, const std::string& portName,
              speed_t baudRate, size_t bufferSize, const std::string& pattern);
    ~InputGyro();
    
    int inputFromGyro();
    void cutout();
    
    void printData(const char* head, int size);
    
    void split(const std::string& str, const std::string& pattern,
               std::vector<std::string>& foundStrings);
    void iStrToAStr(const char* in, size_t size, std::string& out);
    void outputToFile(const std::string& dataString);
    
    
private:
    int fd;
    std::ofstream ofs;
    std::string pattern;
    termios oldTio;
    const size_t bufferSize;
    std::string inBuffer;
    std::string outBuffer;
    std::string searchBuffer;
};

#endif /* InputGyro_hpp */
