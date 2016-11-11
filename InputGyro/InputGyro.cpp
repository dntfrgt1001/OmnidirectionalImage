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
resolution(0.06f), storeSize(10), deltaTime(0.008f),
sensorValueSize(9), inBuffer(""), outBuffer(""), searchBuffer("")
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

    // ２次元配列生成，初期化
    sensorValuess = new short*[storeSize];
    for (int i=0; i<storeSize; i++) {
        sensorValuess[i] = new short[sensorValueSize];
    }
    for (int i=0; i<storeSize; i++) {
        for (int j=0; j<sensorValueSize; j++) {
            sensorValuess[i][j] = 0;
        }
    }
    
    // １次元配列生成，初期化
    estimatedSensorValues = new short[sensorValueSize];
    for (int i=0; i<sensorValueSize; i++) {
        estimatedSensorValues[i] = 0;
    }
    
    for (int i=0; i<3; i++) {
        angle[i] = 0;
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
    
    for (int i=0; i<storeSize; i++) {
        delete(sensorValuess[i]);
    }
    delete(sensorValuess);
    
    delete(estimatedSensorValues);
}

void InputGyro::printCurrentAngle()
{
    std::cout << "current angle = ";
    
    for (int i=0; i<3; i++) {
        std::cout << angle[i] << " ";
    }
    
    std::cout << std::endl;
}

void InputGyro::printCurrentAccel()
{
    std::cout << "current accel = ";
    for (int i=0; i<3; i++) {
        std::cout << estimatedSensorValues[i+6] << " ";
    }
    std::cout << std::endl;
}

void InputGyro::printCurrentSensorValue()
{
    std::cout << "current sensor values = ";
    
    for (int i=0; i<3; i++) {
        std::cout << sensorValuess[storeSize-1][i+3] << " ";
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
            //std::vector<short> tmpSensorValues;
            short tmpValues[9] = {};
            
            //extractSensorValueAsShortFromString(foundStrings[i], tmpSensorValues);
            extractSensorValueAsShortFromString(foundStrings[i], tmpValues);
            renewCurrentSensorValues(tmpValues);
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
    
    /*　いらない？
    for (int i=0; i<foundStrings.size(); i++) {
        if (i != foundStrings.size()-1){
            std::vector<short> tmpSensorValues(9,0);
            
            extractSensorValueAsShortFromString(foundStrings[i], tmpSensorValues);
        }
    }
     */
    
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
(const std::string &in, short* tmpSensorValues)
{
    short c[18];
    
    // String型からchar型の値を取り出す
    sscanf(in.c_str(),
           "%hd %hd %hd %hd %hd %hd %hd %hd %hd \
           %hd %hd %hd %hd %hd %hd %hd %hd %hd ",
            &c[0],  &c[1],  &c[2],  &c[3],  &c[4],  &c[5],
            &c[6],  &c[7],  &c[8],  &c[9], &c[10], &c[11],
           &c[12], &c[13], &c[14], &c[15], &c[16], &c[17] );
    
    for (int i=0; i<sensorValueSize*2; i+=2) {
        short tmpShort = char2short(c[i], c[i+1]);
        
//        tmpSensorValues.push_back(tmpShort);
        tmpSensorValues[i/2] = tmpShort;
    }
}

void InputGyro::renewCurrentSensorValues(short* tmpSensorValues)
{
    for (int i=0; i<storeSize-1; i++) {
        for (int j=0; j<sensorValueSize; j++) {
            sensorValuess[i][j] = sensorValuess[i+1][j];
        }
    }
    
    filterDriftAndNoise(tmpSensorValues);
    
    for (int j=0; j<sensorValueSize; j++) {
        sensorValuess[storeSize-1][j] = tmpSensorValues[j];
    }
}

void InputGyro::filterDriftAndNoise(short* tmpSensorValues)
{
    if (SHRT_MIN + 18 < tmpSensorValues[3]) {
        tmpSensorValues[3] += -18;
    }
    if (tmpSensorValues[4] < SHRT_MAX - 16) {
        tmpSensorValues[4] += 16;
    }
    if (tmpSensorValues[5] < SHRT_MAX - 40) {
        tmpSensorValues[5] += 40;
    }
    
    for (int i=0; i<3; i++) {
        if (tmpSensorValues[i+3] < SHRT_MIN + 50 || SHRT_MAX - 50 < tmpSensorValues[i+3]) {
            //sleep(1);
        }
    }
    
    for (int i=0; i<3; i++) {
        if (-50 < tmpSensorValues[i+3] && tmpSensorValues[i+3] < 50) {
            tmpSensorValues[i+3] = 0;
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
        angle[i] += estimatedSensorValues[i+3] * resolution * deltaTime;
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
