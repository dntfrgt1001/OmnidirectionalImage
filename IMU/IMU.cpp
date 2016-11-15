//
//  IMU.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/11.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "IMU.hpp"

IMU::IMU
(const std::string& outputFile, const std::string& port,
 const speed_t baudRate, const int bufferSize,
 const char* splitPattern, const int patternSize):
 ofs(outputFile), bufferSize(bufferSize),
 splitPattern(splitPattern), patternSize(patternSize), storeSize(0)
{
    //charInput2strInputBlank(charSplPat, patSize, splPat);
 
    // termiosの保存と初期化
    initTermios(port, baudRate);
    
    // 保存用バッファのメモリ確保
    inputBuffer = new char[bufferSize];
    storeBuffer = new char[bufferSize];
    
    // ボイヤームーア法のスキップテーブル
    skipTable = new int[UCHAR_MAX + 1];
    for (int i = CHAR_MIN; i <= CHAR_MAX; i++) {
        setSkipValue(i, patternSize);
    }
    for (int i = 0; i < patternSize - 1; i++) {
        setSkipValue(splitPattern[i], patternSize - 1 - i);
    }
}

IMU::~IMU()
{
    // 元のtermiosに戻す
    tcsetattr(fd, TCSANOW, &terOld);
    
    // 出力ファイルストリームを閉じる
    ofs.close();
    
    delete inputBuffer;
    delete storeBuffer;
    delete skipTable;
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
    // IMUからデータ読み込み
    const int inLen = (int) read(fd, inputBuffer, bufferSize);
    
//    std::cout << "input size = " << inLen << std::endl;
//    std::cout << "input = " << std::endl;
//    printChar(inputBuffer, inLen);
    
    // 入力なしのとき
    if (inLen < 0) return;
    
    // 直前の残りデータと連結
    // strcatだとヌル文字に対処できない
    memcpy(&storeBuffer[storeSize], inputBuffer, inLen);
    storeSize = storeSize + inLen;
    
//    std::cout << "store size = " << storeSize << std::endl;
//    std::cout << "store = " << std::endl;
//    printChar(storeBuffer, storeSize);
    
    // IMUデータの切り出し
    const int pairSize = storeSize / (18 + patternSize) + 1;
    char validData[pairSize][18];
    const int dataSize = extValidData(validData);
    
    // short型データへ変換
    for (int i = 0; i < dataSize; i++) {
        short shortData[9];
        getShortData(validData[i], shortData);
        
        printData(shortData);
    }
}

int IMU::extValidData(char validData[][18])
{
    int findPos = 0, curPos = 0, dataIndex = 0;
    
    while (true) {
        findPos = BoyerMoore(&(storeBuffer[curPos]), storeSize-curPos);
//        findPos = bruteForceMatch(&(storeBuffer[curPos]), storeSize-curPos);
        
        if (findPos == -1) break;
        
        // パターン検出絶対位置
        int globalFindPos = curPos + findPos;
        // 先頭の中途半端なデータを弾く
        if (globalFindPos - 18 >= 0) {
            // データをコピー
            memcpy(validData[dataIndex],
                   &(storeBuffer[globalFindPos - 18]), 18);

            // データ組数を更新
            dataIndex++;
        }
        
        // 現在位置をずらす
        curPos = curPos + findPos + patternSize;
        
//        std::cout << "find pos = " << findPos << std::endl;
//        std::cout << "global find pos = " << globalFindPos << std::endl;
//        std::cout << "cur pos = " << curPos << std::endl;
//        std::cout << "store size = " << storeSize << std::endl;
        
    }

    // ストアサイズの更新
    storeSize = storeSize - curPos;
    
    // 残りのデータを保存
    memmove(storeBuffer, &storeBuffer[curPos], storeSize);
    
//    std::cout << "cur pos = " << curPos << std::endl;
//    std::cout << "rest size = " << storeSize << std::endl;
//    std::cout << "rest = " << std::endl;
//    printChar(storeBuffer, storeSize);
    
    return dataIndex;
}

int IMU::BoyerMoore
(const char *target, const int targetSize)
{
//    std::cout << "target size = " << targetSize << std::endl;
//    std::cout << "target = " << std::endl;
//    printChar(target, targetSize);
    
    int tIdx, pIdx;
    tIdx = patternSize - 1;
    while (tIdx < targetSize) {
        pIdx = patternSize - 1;
        
        while (target[tIdx] == splitPattern[pIdx]) {
            if (pIdx == 0) return tIdx;
            tIdx--;
            pIdx--;
        }
        
        tIdx = tIdx +
               std::max(getSkipValue(target[tIdx]), patternSize-pIdx);
    }
    
    return -1;
}

int IMU::bruteForceMatch(const char *target, const int targetSize)
{
//    std::cout << "target size = " << targetSize << std::endl;
//    std::cout << "target = " << std::endl;
//    printChar(target, targetSize);
    
    for (int i = 0; i <= targetSize - patternSize; i++) {
        for (int j = 0; j < patternSize; j++) {
            if (target[j+i] != splitPattern[j]) break;
            
            if (j == patternSize - 1) {
                return i;
            }
        }
    }
    
    return -1;
}

void IMU::getShortData(const char *charData, short *shortData)
{
    for (int i = 0; i < 9; i++) {
        shortData[i] = getShortValue(charData[i*2], charData[i*2 + 1]);
    }
}

void IMU::printData(const short *shortData)
{
    std::cout << "current data = ";
    for (int i = 0; i < 9; i++) {
        std::cout << std::setw(6) << shortData[i] << " ";
    }
    std::cout << std::endl;
}

/*
int IMU::inputFromIMU(char *inputBuf)
{
    return (int) read(fd, inputBuf, bufSize);
}*/

/*
void IMU::inputData()
{
    // データの読み込み
    char inputBuf[bufSize];
    const int inputLen = inputFromIMU(inputBuf);
    
    std::cout << "input length = " << inputLen << std::endl;
    if (inputLen < 0) {
        return;
    }
    
    // string型に変換
    inputBuf[inputLen] = '\0';
    std::string inputBufString = inputBuf;
    
    printf("input = ");
    for (int i = 0; i < inputLen; i++) {
        printf("0x%02X ", inputBuf[i] );
    }
    printf("\n");
    
//    std::string inputBufString;
//    charInput2strInputBlank(inputBuf, inputLen, inputBufString);
    
    // 前の残りデータと結合
    storedString = storedString + inputBufString;
    
    // 有効データの切り出し
    std::vector<std::string> validStrings;
    const int validSize = extValidData(storedString, validStrings);
    
    // 残りデータの保存
    storedString = validStrings[validSize];
    std::cout << "valid pair = " << validSize << std::endl;
    std::cout << "store data = " << storedString.size() << std::endl;
    
    // 有効データのshort型への変換
    for (int i = 0; i < validSize; i++) {
        std::vector<short> shortData;
        getShortData(validStrings[i], shortData);
        
        printData(shortData);
    }
}

 */

/*
void IMU::inputData(char *inputBuf, const int inputLen)
{
    // string型に変換
    inputBuf[inputLen] = '\0';
    std::string inputBufString = inputBuf;
 
    
    printf("input = ");
    for (int i = 0; i < inputLen; i++) {
        printf("0x%02X ", inputBuf[i] );
    }
    printf("\n");
    
    
    //    std::string inputBufString;
    //    charInput2strInputBlank(inputBuf, inputLen, inputBufString);
    
    // 前の残りデータと結合
    storedString = storedString + inputBufString;
    
    // 有効データの切り出し
    std::vector<std::string> validStrings;
    const int validSize = extValidData(storedString, validStrings);
    
    // 残りデータの保存
    storedString = validStrings[validSize];
    std::cout << "valid pair = " << validSize << std::endl;
    std::cout << "store data = " << storedString.size() << std::endl;
    
    // 有効データのshort型への変換
    for (int i = 0; i < validSize; i++) {
        std::vector<short> shortData;
        getShortData(validStrings[i], shortData);
        
        printData(shortData);
    }
}

void IMU::charInput2strInputBlank
(const char *charIn, const size_t size, std::string &strInBlank)
{
 
    // 空白を入れたインプット
    char charInBlank[size * 2 + 1];
    
    // 空白を挟む
    for (int i = 0; i < size; i++) {
        charInBlank[i*2] = charIn[i];
        charInBlank[i*2 + 1] = ' ';
    }
    // 文字列にするために終端文字を追加
    charInBlank[size * 2] = '\0';
        
    // string型に変換
    strInBlank = charInBlank;
 
}

int IMU::extValidData
(const std::string &storedString, std::vector<std::string>& extStrings)
{
    size_t findPos, curPos = 0, patLen = splPat.size();
    
    // 区切りパターンの文字列を検索し切り出す
    while ((findPos=storedString.find(splPat, curPos)) !=
           std::string::npos) {
        // 文字列のcurPosの位置から(findPos - curPos)までコピー
        extStrings.push_back
        (std::string(storedString, curPos, findPos - curPos));
        
        curPos = findPos + patLen;
    }
    
    // 残りのデータ
    extStrings.push_back
    (std::string(storedString, curPos, storedString.size()-curPos));
    
    return (int) extStrings.size() - 1;
}

void IMU::getShortData
(const std::string &extString, std::vector<short> &data)
{
    // データは9*2=18個
    const char* extChar = extString.c_str();
    
    // short型の数値データに格納
    data = std::vector<short>(9);
    for (int i = 0; i < 9; i++) {
        data[i] = getShortValue(extChar[i*2], extChar[i*2 + 1]);
    }
   
    
    for (int i = 0; i < 18; i++) {
        printf("0x%02X ", extChar[i]);
    }
    printf("\n");
    
    for (int i = 0; i < 9; i++) {
        printf("0x%04X ", data[i]);
    }
    printf("\n");
}


void IMU::printData(const std::vector<short> &data)
{
    std::cout << "current data = ";
    for (int i = 0; i < data.size(); i++) {
        std::cout << std::setw(6) << data[i] << " ";
    }

    std::cout << std::endl;
}

*/

