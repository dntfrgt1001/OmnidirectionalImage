//
//  IMU.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/11.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "IMU.hpp"

IMU::IMU
(const std::string& outPath, const std::string& port, const speed_t baudRate,
 const size_t bufSize, const std::string& splPat):
bufSize(bufSize), splPat(splPat)
{
    initTermios(port, baudRate);
}

IMU::~IMU()
{
    // 元のtermiosに戻す
    tcsetattr(fd, TCSANOW, &terOld);
}

void IMU::initTermios(const std::string& port, const speed_t baudRate)
{
    // ポートを開く
    fd = open(port.c_str(), O_RDONLY|O_NONBLOCK);
    if (fd < 0) {
        std::cout << "port cannot be opened" << std::endl;
        exit(1);
    }
    
    // 古いtermiosを保存
    tcgetattr(fd, &terOld);
    
    // termios構造体をゼロで初期化
    memset(&terNew, 0, sizeof(termios));
    // 文字サイズを設定
    terNew.c_cflag = CS8;
    // 入出力速度設定
    cfsetspeed(&terNew, baudRate);
    // パラメータ設定
    tcsetattr(fd, TCSANOW, &terNew);
}

void IMU::inputData()
{
    // ポートからデータを読むためのバッファ
    char inputBuf[bufSize];
    
    // データの読み込み
    int inputLen = read(fd, inputBuf, bufSize);
    
    // 文字列として扱うために終端文字を追加
    inputBuf[inputLen] = '\0';
    
    std::string inputString;
    
}

void IMU::charInput2strInput
(const char *charIn, const size_t size, std::string &strIn)
{
    strIn = "";
    
    for (int i = 0; i < size; i++) {
        <#statements#>
    }
}