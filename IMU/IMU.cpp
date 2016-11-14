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
 const speed_t baudRate, const size_t bufSize,
 const char* charSplPat, const int patSize):
 ofs(outputFile), bufSize(bufSize), storedString(""), splPat(charSplPat)
{
    //charInput2strInputBlank(charSplPat, patSize, splPat);
 
    initTermios(port, baudRate);
}

IMU::~IMU()
{
    // 元のtermiosに戻す
    tcsetattr(fd, TCSANOW, &terOld);
    
    // 出力ファイルストリームを閉じる
    ofs.close();
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
    const int inputLen = (int) read(fd, inputBuf, bufSize);
    
    // string型に変換，空白を挟む
    inputBuf[inputLen] = '\0';
    std::string inputBufString = inputBuf;
//    std::string inputBufString;
//    charInput2strInputBlank(inputBuf, inputLen, inputBufString);
    
    // 前の残りデータと結合
    storedString = storedString + inputBufString;
    
    // 有効データの切り出し
    std::vector<std::string> validStrings;
    const int validSize = extValidData(storedString, validStrings);
    
    // 残りデータの保存
    storedString = validStrings[validSize];
    
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
    /*
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
     */
}

int IMU::extValidData
(const std::string &storedString, std::vector<std::string>& extStrings)
{
    size_t findPos, curPos = 0, patLen = splPat.size();
    
    // 区切りパターンの文字列を検索し切り出す
    while ((findPos=storedString.find(splPat, curPos)) !=
           std::string::npos) {
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
    
    /*
    // 文字列から数値データに変換
    sscanf(extString.c_str(),
           "%c %c %c %c %c %c %c %c %c \
           %c %c %c %c %c %c %c %c %c ",
//           "%hd %hd %hd %hd %hd %hd %hd %hd %hd \
//           %hd %hd %hd %hd %hd %hd %hd %hd %hd ",
           &c[0],  &c[1],  &c[2],  &c[3],  &c[4],  &c[5],
           &c[6],  &c[7],  &c[8],  &c[9], &c[10], &c[11],
           &c[12], &c[13], &c[14], &c[15], &c[16], &c[17] );
    */
     
    // short型の数値データに格納
    data = std::vector<short>(9);
    for (int i = 0; i < 9; i++) {
        data[i] = getShortValue(extChar[i*2], extChar[i*2 + 1]);
    }
}

void IMU::printData(const std::vector<short> &data)
{
    std::cout << "current data = ";
    for (int i = 0; i < data.size(); i++) {
        std::cout << std::setw(6) << data[i] << " ";
    }

    std::cout << std::endl;
}



