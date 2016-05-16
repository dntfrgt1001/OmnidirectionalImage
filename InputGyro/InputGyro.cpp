//
//  InputGyro.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/03/26.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "InputGyro.hpp"

InputGyro::InputGyro
(const std::string& fileName, const std::string& portName,
 speed_t baudRate, size_t bufferSize, const std::string& pattern):
pattern(pattern), bufferSize(bufferSize)
{
    fd = open(portName.c_str(), O_RDONLY|O_NONBLOCK);
    if (fd < 0) {
        printf("port cannnot open\n");
        exit(1);
    }
     
    tcgetattr(fd, &oldTio);
    
    termios tio;
    memset(&tio, 0, sizeof(termios));
    tio.c_cflag = CS8;
    cfsetspeed(&tio, baudRate);
    tcsetattr(fd, TCSANOW, &tio);
    
    ofs.open(fileName);

    //std::string tmpPattern = pattern;
    //iStrToAStr(tmpPattern.c_str(), tmpPattern.size(), this->pattern);
    
    char patbuf[4] = {'E', 'B', 'S', 0x01};

    cAry2cStr(patbuf, 4, this->pattern);
    
    std::cout << "pattern = " << this->pattern << std::endl;

    
}

InputGyro::~InputGyro()
{
    tcsetattr(fd, TCSANOW, &oldTio);
    ofs.close();
}

void InputGyro::printData(const char *head, int size)
{
    for (int i=0; i<size; i++) {
        printf("%c ", head[i]);
    }

    printf("\n");
}

int InputGyro::inputFromGyro()
{
    char fpInBuffer[bufferSize];
    int inLength = (int) read(fd, fpInBuffer, bufferSize);
    
    if (inLength <= 0 || bufferSize < inLength) {
        fpInBuffer[0] = '\0';
        return -1;
    }
    
    fpInBuffer[inLength] = '\0';
    
    
    cAry2cStr(fpInBuffer, inLength, inBuffer);
    
    searchBuffer = searchBuffer + inBuffer;
/*
    std::cout << "inBuffer" << std::endl;
    std::cout << inBuffer << std::endl;
    
    std::cout << "searchBuffer" << std::endl;
    std::cout << searchBuffer << std::endl;
  */
    return inLength;
}

int InputGyro::cutout()
{
    std::vector<std::string> foundStrings;
    
    split(searchBuffer, pattern, foundStrings);
    
//    std::cout << std::endl << "**************" << std::endl;
    for (int i=0; i<foundStrings.size(); i++) {
        if (i != foundStrings.size()-1){
//            std::cout << foundStrings[i] << std::endl;
            
            std::string shortString;
            //outputToFile(foundStrings[i]);
            
            cStr2sStr(foundStrings[i], shortString);
            
//            outputToFile(shortString);
            std::cout << shortString << std::endl << std::endl;
        }
    }
//    std::cout << std::endl << "**************" << std::endl;
    
    searchBuffer = foundStrings[foundStrings.size()-1];
    
//    std::cout << std::endl << std::endl << "loop end" << std::endl << std::endl;
    
    return (int)foundStrings.size() - 1;
}

void InputGyro::split
(const std::string &str, const std::string &pattern,
 std::vector<std::string> &foundStrings)
{
    foundStrings.clear();
    
    size_t current = 0, found, patlen = pattern.size();
    

    while ((found = str.find(pattern, current)) != std::string::npos) {
        foundStrings.push_back(std::string(str, current, found - current));
        current = found + patlen;
    }

    
    foundStrings.push_back(std::string(str, current, str.size()-current));
}

void InputGyro::cAry2cStr(const char *in, size_t size, std::string& out)
{
    out = "";
    
    for (int i=0; i<size; i++) {
        int tmpInt = in[i];

        out = out + std::to_string(tmpInt) + " ";
    }
}

void InputGyro::cStr2sStr(const std::string& in, std::string &out)
{
    out = "";
    
    short c[18];
    sscanf(in.c_str(),
           "%hd %hd %hd %hd %hd %hd %hd %hd %hd \
            %hd %hd %hd %hd %hd %hd %hd %hd %hd ",
             &c[0],  &c[1],  &c[2],  &c[3],  &c[4],  &c[5],
             &c[6],  &c[7],  &c[8],  &c[9], &c[10], &c[11],
            &c[12], &c[13], &c[14], &c[15], &c[16], &c[17]   );
    
    for (int i=0; i<18; i+=2) {
        short tmpShort = char2short(c[i], c[i+1]);
        
        out = out + std::to_string(tmpShort) + " ";
    }
    
    
}

void InputGyro::outputToFile(const std::string &dataString)
{
    ofs << dataString << std::endl;
}
short InputGyro::char2short(short upper, short lower)
{
    short concated;
    
    concated = (upper << 8) | (0x00ff & lower);

    return concated;
}

short InputGyro::char2short(char upper, char lower)
{
    short concated;
    short shortUpper = upper;
    short shortLower = lower;
    
    concated = (shortUpper << 8) | (0x00ff & shortLower);
    
    return concated;
}