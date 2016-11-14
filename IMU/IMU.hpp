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

#include <termios.h>
#include <fcntl.h>

class  IMU
{
public:
    IMU(const std::string& outPath, const std::string& port,
        const speed_t baudRate, const size_t bufSize,
        const std::string& splPat);
    ~IMU();
    
    // termios構造体の初期化
    void initTermios(const std::string& port, const speed_t baudRate);
    
    // センサからデータを入力
    void inputData();
    
    // char型のセンサデータをstring型に変換
    void charInput2strInput
    (const char* charIn, const size_t size, std::string& strIn);
    
private:
    int fd;
    const size_t bufSize;
    termios terNew;
    termios terOld;
    const std::string splPat;
    
};

#endif /* IMU_hpp */
