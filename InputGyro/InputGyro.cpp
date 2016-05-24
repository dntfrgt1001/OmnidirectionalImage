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
pattern(pattern), bufferSize(bufferSize),
resolution(0.01526 / 2), storeSize(3), deltaTime(0.012),
sensorValueSize(18)
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
    
    char patbuf[4] = {'E', 'B', 'S', 0x01};

    changegSensorValueFromCharToString(patbuf, 4, this->pattern);
    
    for (int i=0; i<storeSize; i++) {
        std::vector<short> tmpVector;
        
        for (int i=0; i<3; i++) {
            tmpVector.push_back(0);
        }
        
        sensorValuess.push_back(tmpVector);
    }
    
    for (int i=0; i<3; i++) {
        curAngle[i] = 0;
    }
    
    for (int i=0; i<sensorValueSize/2; i++) {
        estimatedSensorValues.push_back(0);
    }
}

InputGyro::~InputGyro()
{
    tcsetattr(fd, TCSANOW, &oldTio);
    ofs.close();
    /*
    std::vector<std::string>().swap(foundStrings);
    std::vector<short>().swap(estimatedSensorValues);
    
    for (int i=0; i<sensorValueSize/2; i++) {
        std::vector<short>().swap(sensorValuess[i]);
    }
    std::vector<std::vector<short>>().swap(sensorValuess);
     */
}

void InputGyro::printCurrentAngle()
{
    for (int i=0; i<3; i++) {
        std::cout << curAngle[i] << " ";
    }
    
    std::cout << std::endl;
}

void InputGyro::printCurrentSensorValue()
{
    std::cout << "current sensor values = ";
    
    for (int i=3; i<6; i++) {
        std::cout << sensorValuess[storeSize-1][i] << " ";
    }
    
    std::cout << std::endl;
}

int InputGyro::inputFromGyro()
{
    char charInBuffer[bufferSize];
    
    // センサーから送られてきた値を読み込む
    int inLength = (int) read(fd, charInBuffer, bufferSize);
    
    // 読み込む値がない場合，バッファを超えるサイズの値を読み込む場合
    if (inLength <= 0 || bufferSize < inLength) {
        charInBuffer[0] = '\0';
        return -1;
    }
    
    // 文字列として扱うために終端文字を追加
    charInBuffer[inLength] = '\0';
    
    // 各値の間に空白を入れながらstring型に変換
    changegSensorValueFromCharToString(charInBuffer, inLength, inBuffer);
    
    // 元々のバッファと現在のバッファを結合
    searchBuffer = searchBuffer + inBuffer;

    // 切り出したデータの組の数を返す
    int cutCount = cutout();
    
    if (cutCount > 0) {
        for (int i=0; i<cutCount; i++) {
            std::vector<short> tmpSensorValues;
            
            extractSensorValueAsShortFromString(foundStrings[i], tmpSensorValues);
            renewCurrentSensorValues(tmpSensorValues);
        }
        
        estimateSensorValues();
        
        renewCurrentAngle();
    }
    
    return cutCount;
}

int InputGyro::cutout()
{
    // 組ごとにデータ列をstring型として切り出す
    split(searchBuffer, pattern);
    
    for (int i=0; i<foundStrings.size(); i++) {
        if (i != foundStrings.size()-1){
            std::vector<short> tmpSensorValues;
            
            extractSensorValueAsShortFromString(foundStrings[i], tmpSensorValues);
        }
    }
    
    // 残りのstringを保存
    searchBuffer = foundStrings[foundStrings.size()-1];
    
    // 切り出したデータの組の数を返す
    return (int)foundStrings.size() - 1;
}

void InputGyro::split
(const std::string &str, const std::string &pattern)
{
    foundStrings.clear();
    
    size_t current = 0, found, patlen = pattern.size();
    
    // データ列を組ごとに切り出し
    while ((found = str.find(pattern, current)) != std::string::npos) {
        foundStrings.push_back(std::string(str, current, found - current));
        current = found + patlen;
    }

    foundStrings.push_back(std::string(str, current, str.size()-current));
}

void InputGyro::changegSensorValueFromCharToString
(const char *in, size_t size, std::string& out)
{
    out = "";
    
    for (int i=0; i<size; i++) {
        int tmpInt = in[i];

        out = out + std::to_string(tmpInt) + " ";
    }
}

void InputGyro::extractSensorValueAsShortFromString
(const std::string &in, std::vector<short>& tmpSensorValues)
{
    short c[18];
    
    // String型からchar型の値を取り出す
    sscanf(in.c_str(),
           "%hd %hd %hd %hd %hd %hd %hd %hd %hd \
           %hd %hd %hd %hd %hd %hd %hd %hd %hd ",
            &c[0],  &c[1],  &c[2],  &c[3],  &c[4],  &c[5],
            &c[6],  &c[7],  &c[8],  &c[9], &c[10], &c[11],
           &c[12], &c[13], &c[14], &c[15], &c[16], &c[17] );
    
    for (int i=0; i<sensorValueSize; i+=2) {
        short tmpShort = char2short(c[i], c[i+1]);
        
        tmpSensorValues.push_back(tmpShort);
    }
}

void InputGyro::renewCurrentSensorValues(std::vector<short> &tmpSensorValues)
{
    sensorValuess.erase(sensorValuess.begin());
    
    filterDriftAndNoise(tmpSensorValues);
    
    sensorValuess.push_back(tmpSensorValues);
}

void InputGyro::filterDriftAndNoise(std::vector<short> &tmpSensorValues)
{
    for (int i=3; i<6; i++) {
        if (-50 < tmpSensorValues[i] && tmpSensorValues[i] < 50) {
            tmpSensorValues[i] = 0;
        }
    }
}

void InputGyro::estimateSensorValues()
{
    // 移動平均
    for (int j=0; j<sensorValueSize; j++) {
        int accum = 0;
        
        for (int i=0; i<storeSize; i++) {
            accum += sensorValuess[i][j];
        }
        
        accum /= (int) storeSize;
        
        estimatedSensorValues[j] = accum;
    }
}

void InputGyro::renewCurrentAngle()
{
    for (int i=0; i<3; i++) {
        curAngle[i] += estimatedSensorValues[i+3] * resolution * deltaTime;
    }
}

/*
void InputGyro::changeSensorValueFromCharToShort
(const std::string& in, std::string &out)
{
    out = "";
    
    short c[18];
    sscanf(in.c_str(),
           "%hd %hd %hd %hd %hd %hd %hd %hd %hd \
            %hd %hd %hd %hd %hd %hd %hd %hd %hd ",
             &c[0],  &c[1],  &c[2],  &c[3],  &c[4],  &c[5],
             &c[6],  &c[7],  &c[8],  &c[9], &c[10], &c[11],
            &c[12], &c[13], &c[14], &c[15], &c[16], &c[17]   );
    
    // 現在保存しているデータ群のうち最も古い組を削除
    sensorValuess.erase(sensorValuess.begin());
    
    std::vector<short> tmpVector;
    
    for (int i=0; i<18; i+=2) {
        short tmpShort = char2short(c[i], c[i+1]);
        
        tmpVector.push_back(tmpShort);
        
        out = out + std::to_string(tmpShort) + " ";
    }
    
    // 現在処理中のデータをデータ群に追加
    sensorValuess.push_back(tmpVector);
    
    for (int j=0; j<9; j++) {
        int tmp = 0;
        for (int i=0; i<storeSize; i++) {
            tmp += sensorValuess[i][j];
        }
        // int型でキャストしないと符号なしになる
        tmp /= (int)storeSize;
        
        tmpVector.push_back(tmp);
    }
    std::cout << std::endl;
    
    for (int i=3; i<6; i++) {
        std::cout << tmpVector[i] << " ";
    }
    std::cout << std::endl;
    
    for (int i=0; i<3; i++) {
        curAngle[i] += tmpVector[i+3] * resolution * deltaTime;
        std::cout << curAngle[i] << " ";
    }
    std::cout << std::endl;
    
}

*/

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