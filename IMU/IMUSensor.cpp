//
//  IMUSensor.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/11.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "IMUSensor.hpp"

IMUSensor::IMUSensor
(const std::string& port, const speed_t baudRate,
 const int bufferSize, const char* splitPattern, const int patternSize,
 const std::string& output):
 IMU(), bufferSize(bufferSize),
 splitPattern(splitPattern), patternSize(patternSize), storeSize(0),
 imuBeginFlag(false)
{
    //charInput2strInputBlank(charSplPat, patSize, splPat);
 
    if (output != "") {
        ofs.open(output);
        
        if (!ofs) {
            std::cerr << "output file cannnot open file" << std::endl;
            exit(1);
        }
    }
    
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

IMUSensor::~IMUSensor()
{
    // 元のtermiosに戻す
    tcsetattr(fd, TCSANOW, &terOld);
    
    // 確保したメモリ領域を開放
    delete inputBuffer;
    delete storeBuffer;
    delete skipTable;
}

void IMUSensor::initTermios(const std::string& port, const speed_t baudRate)
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

int IMUSensor::inputDataSet(std::vector<IMUData> &datas)
{
    // IMUからデータ読み込み
    const int inLen = (int) read(fd, inputBuffer, bufferSize);

    /*
    std::cout << "------------------------------" << std::endl;
    std::cout << "input size = " << inLen << std::endl;
    std::cout << "input = " << std::endl;
    printChar(inputBuffer, inLen);
    std::cout << "------------------------------" << std::endl;
    */
    
    // 入力なしのとき
    if (inLen < 0) return 0;
    
    // 直前の残りデータと連結
    memcpy(&storeBuffer[storeSize], inputBuffer, inLen);
    storeSize = storeSize + inLen;
    
   /*
    std::cout << "------------------------------" << std::endl;
    std::cout << "store size = " << storeSize << std::endl;
    std::cout << "store = " << std::endl;
    printChar(storeBuffer, storeSize);
    std::cout << "------------------------------" << std::endl;
    */
    
    // IMUデータの切り出し
    const int pairSize = storeSize / (18 + patternSize) + 1;
    char validData[pairSize][18];
    const int dataSize = extValidData(validData);
    
    // データ領域を確保
    datas = std::vector<IMUData>(dataSize);
    
    // short型データへ変換
    for (int i = 0; i < dataSize; i++) {
        // データをshort型に変換
        short shortData[9];
        getShortData(validData[i], shortData);
        
        // データを格納
        datas[i] = getDataSet(shortData);
        
//        printData(shortData);
    }
    
    return dataSize;
}

IMUData IMUSensor::getDataSet(const short *shortData)
{
    IMUData imu_data = {shortData[0], shortData[1], shortData[2],
                        shortData[3], shortData[4], shortData[5],
                        shortData[6], shortData[7], shortData[8]};
    return imu_data;
}

int IMUSensor::extValidData(char validData[][18])
{
    int findPos = 0, curPos = 0, dataIndex = 0;
    
    while (true) {
        findPos = BoyerMoore(&(storeBuffer[curPos]), storeSize-curPos);
//        findPos = bruteForceMatch(&(storeBuffer[curPos]), storeSize-curPos);

        // パターンが見つからなかったらリターン
        if (findPos == -1) break;
        
        // パターン検出絶対位置を計算
        int globalFindPos = curPos + findPos;
        
        // IMUデータの終了を表すパターンならデータを取り出す
        if (imuBeginFlag) {
            // データを取り出す
            memcpy(validData[dataIndex],
                   &(storeBuffer[globalFindPos - 18]), 18);
            
            // データ組数をインクリメント
            dataIndex++;
            
            // フラグをクリア
            imuBeginFlag = false;
        }
        // IMUデータの開始を表すパターンか判定し，フラグを操作
        else {
            // データのタイプを確認 'E' 'B' '(Type)'
            if (storeBuffer[globalFindPos + 2] == 'S') {
                // フラグをセット
                imuBeginFlag = true;
            }
        }
        
        /*
        // 先頭の中途半端なデータを弾く
        if (globalFindPos - 18 >= 0) {
            // データをコピー
            memcpy(validData[dataIndex],
                   &(storeBuffer[globalFindPos - 18]), 18);

            // データ組数を更新
            dataIndex++;
        }*/
        
        // 現在位置をずらす
//        curPos = globalFindPos + patternSize;
        // パターンサイズ'E''B'とは異なるサイズでスキップ
        
        curPos = globalFindPos + std::min(4, storeSize - globalFindPos);
        
//        std::cout << "find pos = " << findPos << std::endl;
//        std::cout << "global find pos = " << globalFindPos << std::endl;
//        std::cout << "cur pos = " << curPos << std::endl;
//        std::cout << "store size = " << storeSize << std::endl;
        
    }

    // ストアサイズの更新
    storeSize = storeSize - curPos;
    
    // 残りのデータを保存
    memmove(storeBuffer, &storeBuffer[curPos], storeSize);

    /*
    std::cout << "------------------------------" << std::endl;
//    std::cout << "cur pos = " << curPos << std::endl;
    std::cout << "rest size = " << storeSize << std::endl;
    std::cout << "rest = " << std::endl;
    printChar(storeBuffer, storeSize);
    std::cout << "------------------------------" << std::endl;
    */
    
    return dataIndex;
}

int IMUSensor::BoyerMoore
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

int IMUSensor::bruteForceMatch(const char *target, const int targetSize)
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

void IMUSensor::getShortData(const char *charData, short *shortData)
{
    for (int i = 0; i < 9; i++) {
        shortData[i] = getShortValue(charData[i*2], charData[i*2 + 1]);
    }
}

void IMUSensor::printData(const short *shortData)
{
    char fillSaved = std::cout.fill();
    std::cout << "[ ";
    for (int i = 0; i < 9; i++) {
        std::cout << std::setw(6) << shortData[i] << " ";
    }
    std::cout << "]" << std::endl;
    std::cout.fill(fillSaved);
}

void IMUSensor::outputData(const IMUData& data)
{
    std::ios::fmtflags flagsSaved = std::cout.flags();
    char fillSaved = std::cout.fill();
    
    ofs
        << std::setw(6) << data.accel_x << " "
        << std::setw(6) << data.accel_y << " "
        << std::setw(6) << data.accel_z << " "
        << std::setw(6) << data.gyro_x << " "
        << std::setw(6) << data.gyro_y << " "
        << std::setw(6) << data.gyro_z << " "
        << std::setw(6) << data.mag_x << " "
        << std::setw(6) << data.mag_y << " "
        << std::setw(6) << data.mag_z << " "
    << std::endl;
    
    std::cout.fill(fillSaved);
    std::cout.flags(flagsSaved);
}


