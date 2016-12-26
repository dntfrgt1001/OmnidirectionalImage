//
//  IMUSensor.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/11.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef IMUSensor_hpp
#define IMUSensor_hpp

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

#include "IMU.hpp"

class  IMUSensor: public IMU {
public:
    IMUSensor(const std::string& port, const speed_t baudRate,
              const int bufferSize, const char* splitPattern,
              const int patternSize,
              const std::string& output = std::string(""));
    
    ~IMUSensor();
    
    // termios構造体の初期化
    void initTermios(const std::string& port, const speed_t baudRate);
    
    // IMUクラスのインタフェース
    int inputDataSet(std::vector<IMUData>& datas);
    
    void outputData(const IMUData& data);
    
    // データをchar型で取り出す
    int extValidData(char validData[][18]);
    
    // ブルートフォースでパターンマッチング
    int bruteForceMatch(const char* target, const int targetSize);

    // char型のデータ組をshort型に変換
    void getShortData(const char charData[], short shortData[]);
    
    IMUData getDataSet(const short shortData[]);
    
    // char型を連結してshort型にする
    short getShortValue(const char upper, const char lower) {
        return ((short) upper << 8) | ((short) lower & 0x00ff);
    }
    
    // ボイヤームーア法でパターンマッチング
    int BoyerMoore(const char* target, const int targetSize);
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
        
private:
    // シリアル通信のための構造体
    int fd;
    termios terNew;
    termios terOld;
    
    // バッファメモリ
    const int bufferSize;
    char* inputBuffer;
    char* storeBuffer;
    int storeSize;
    
    // 区切りパターン
    const char* const splitPattern;
    const int patternSize;
    // 区切りの開始か終了か
    bool imuBeginFlag;
    
    // センサデータのファイル出力
    std::ofstream ofs;
    
    // ボイヤームーア方のスキップテーブル
    int* skipTable;
};


#endif /* IMUSensor_hpp */
