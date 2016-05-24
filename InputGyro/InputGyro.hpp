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
    
    int inputFromGyro(); // センサーからの入力をバッファに格納
    int cutout(); // バッファから組ごとにセンサー値を切り出し
    
    void printCurrentAngle();
    void printCurrentSensorValue();
    
    void split(const std::string& str, const std::string& pattern);
    // センサー値を読み込みバッファに追加
    void changegSensorValueFromCharToString
    (const char* in, size_t size, std::string& out);
    // char型のセンサー値をshort型のセンサー値に
    void extractSensorValueAsShortFromString
    (const std::string& in, std::vector<short>& tmpSensorValues);
    void outputToFile(const std::string& dataString);
    // 現在保存しているセンサー値を更新
    void renewCurrentSensorValues(std::vector<short>& tmpSensorValues);
    // 閾値以下のノイズをカット，ドリフト値について補正
    void filterDriftAndNoise(std::vector<short>& tmpSensorValues);
    // 現在保存しているセンサー値から正しい値を推定
    void estimateSensorValues();
    // センサー値を使って現在の角度を更新
    void renewCurrentAngle();
    
    
    
    short char2short(char upper, char lower);
    short char2short(short upper, short lower);
    
    
private:
    int fd;
    std::ofstream ofs;
    std::string pattern;
    termios oldTio;
    
    const size_t bufferSize;
    std::string inBuffer;
    std::string outBuffer;
    std::string searchBuffer;
    
    std::vector<std::string> foundStrings;
    
    std::vector<std::vector<short>> sensorValuess;
    std::vector<short> estimatedSensorValues;
    const size_t sensorValueSize;
    const size_t storeSize;
    
    float curAngle[3];
    const float resolution;
    const float deltaTime;
};

#endif /* InputGyro_hpp */
