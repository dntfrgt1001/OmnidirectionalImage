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
#include <assert.h>

typedef struct IMU_Data {
    int accel_x, accel_y, accel_z;
    int gyro_x, gyro_y, gyro_z;
    int mag_x, mag_y, mag_z;
} IMU_Data;

class  IMU
{
public:
    IMU(const std::string& outputFile, const std::string& port,
        const speed_t baudRate, const int bufferSize,
        const char* splitPattern, const int patternSize);
    ~IMU();
    
    // termios構造体の初期化
    void initTermios(const std::string& port, const speed_t baudRate);
    
    //void inputData(const char input[], const int inputSize);
    void inputData();
    
    // データをchar型で取り出す
    int extValidData(char validData[][18]);
    
    // ブルートフォースでパターンマッチング
    int bruteForceMatch(const char* target, const int targetSize);
    
    // ボイヤームーア法でパターンマッチング
    int BoyerMoore(const char* target, const int targetSize);
    
    // char型のデータ組をshort型に変換
    void getShortData(const char charData[], short shortData[]);
    
    // char型を連結してshort型にする
    short getShortValue(const char upper, const char lower) {
        return ((short) upper << 8) | ((short) lower & 0x00ff);
    }
    
    // スキップテーブルの生成
    void setSkipValue(const char charIndex, const int skipValue) {
        skipTable[getSkipIndex(charIndex)] = skipValue;
    }
    // スキップテーブルの参照
    int getSkipValue(const char charIndex) {
        return skipTable[getSkipIndex(charIndex)];
    }
    // スキップテーブルのインデックス変換 (負にならないように)
    int getSkipIndex(const char charIndex) {
        return (int) charIndex - CHAR_MIN;
    }
    
    // データを出力
    void printData(const short shortData[]);
    
    void printChar(const char* pointer, const int size) {
        std::ios::fmtflags flagsSaved = std::cout.flags();
        char fillSaved = std::cout.fill();
        std::cout << std::hex << std::uppercase;
        for (int i = 0; i < size; i++){
            std::cout << std::setw(2) << std::setfill('0') << (int) (pointer[i] & 0x00ff) << " ";
            if ((i+1) % 10 == 0) std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout.fill(fillSaved);
        std::cout.flags(flagsSaved);
    }
    
    /*
    // センサデータを読み込み
    int inputFromIMU(char* inputBuf);
    
    // センサからデータを入力
    void inputData();
    
    // テスト用
    void inputData(char* inputBuf, const int inputLen);
    
    // char型のセンサデータをstring型に変換
    void charInput2strInputBlank
    (const char* charIn, const size_t size, std::string& strInBlank);

    // 有効なセンサデータの組を文字列で切り出す
    int extValidData
    (const std::string& storedString, std::vector<std::string>& extStrings);
    
    // センサデータを文字列からshort型に変換
    void getShortData
    (const std::string& extString, std::vector<short>& data);
    */
    

//    void printData(const std::vector<short>& data);
    

    
private:
    int fd;
    termios terNew;
    termios terOld;
    
    std::ofstream ofs;
    const int bufferSize;
    const char* const splitPattern;
    const int patternSize;
    
    char* inputBuffer;
    char* storeBuffer;
    int storeSize;
    
    bool beginFlag;
    
    int* skipTable;
};


#endif /* IMU_hpp */
