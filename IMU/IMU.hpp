//
//  IMU.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/11.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef IMU_hpp
#define IMU_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

class  IMU
{
public:
    IMU(const std::string& outputFile, const std::string& port,
        const speed_t baudRate, const size_t bufSize,
        const char* charSplPat, const int patSize);
    ~IMU();
    
    // termios構造体の初期化
    void initTermios(const std::string& port, const speed_t baudRate);
    
    // センサからデータを入力
    void inputData();
    
    // char型のセンサデータをstring型に変換
    void charInput2strInputBlank
    (const char* charIn, const size_t size, std::string& strInBlank);

    // 有効なセンサデータの組を文字列で切り出す
    int extValidData
    (const std::string& storedString, std::vector<std::string>& extStrings);
    
    // センサデータを文字列からshort型に変換
    void getShortData
    (const std::string& extString, std::vector<short>& data);
    
    // char型を連結してshort型にする
    short getShortValue(const short upper, const short lower) {
        return ((upper << 8) & 0xff00) | (lower & 0x00ff);
    }
    
    // データを出力
    void printData(const std::vector<short>& data);
    
private:
    int fd;
    termios terNew;
    termios terOld;
    
    std::ofstream ofs;
    const size_t bufSize;
    std::string splPat;
    std::string storedString;
    
};

#endif /* IMU_hpp */
