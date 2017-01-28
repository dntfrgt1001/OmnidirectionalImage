//
//  main.cpp
//  ConvertGyro
//
//  Created by masakazu nakazawa on 2016/03/23.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "InputGyro.hpp"
#include "IMUSensor.hpp"
#include "IMUFile.hpp"
#include "IMUProcess.hpp"

int main(int argc, const char * argv[])
{
    /*
    const char pat[] = {'E', 'B', 'S', '\0'};
    const std::string patString = pat;
    const char neg[] = {0x01, 0x02, 0x01, -0x02, 'E', 'B', 'S', 0x01, 0x02};
    const std::string negString = neg;
    
    int found = negString.find(pat);

    
    char* str1 = new char[10];
    char* str2 = new char[10];
    for (int i = 0; i < 10; i++) {
        str1[i] = 0x41 + i;
        str2[i] = 0x7f;
    }
    
    memcpy(&str1[2], str2, 3);
*/
    
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string inputfile = path + "log.txt";
    
    // シリアルポート設定
    const std::string port = "/dev/tty.usbmodem1422";
    const speed_t baudRate = B57600;
    
    // バッファサイズ
    const long bufSize = 4 * 1024 * 1024;

    // 区切りパターン
    const char splitPattern[] = {'E', 'B'};
    const int patSize = sizeof(splitPattern)/sizeof(*splitPattern);
    
    // IMUデータの読み込み先
//    IMUFile imuFile(outfile);
//    IMU& imu = imuFile;
    // ファイル出力
    const std::string outputfile = path + "logtest.txt";
    IMUSensor imuSensor(port, baudRate, bufSize, splitPattern, patSize,outputfile);
//    IMUSensor imuSensor(port, baudRate, bufSize, splitPattern, patSize);
    IMU& imu = imuSensor;

    // IMUデータ処理
    IMUProcess imuPro(imu);
    
    struct timeval startt, midt, endt;
    gettimeofday(&startt, NULL);
    
    time_t start, end;
    time(&start);
    
    int count = 0;
    long int midCount = 1;
    double period = (double) 1.0 / 30.0;
    const int maxCount = 1000;
    IMUData tmpIMUData;
    while (count < maxCount) {
        // データ組を入力
        std::vector<IMUData> datas;
        const int dataNum = imu.inputDataSet(datas);
        count += dataNum;
        std::cout << "# data = " << dataNum << std::endl;
        
        // 経過時間を計測
        gettimeofday(&midt, NULL);
        double midTime = (double)(midt.tv_sec - startt.tv_sec) +
                            (midt.tv_usec - startt.tv_usec) * 1e-6;
        
        // １周期分を超えたら最後のデータをファイル出力
        if (midTime > period * midCount) {
            std::cout << "---------------------------------" << std::endl;
            imu.printIMUData(tmpIMUData);
            std::cout << "---------------------------------" << std::endl;
            imuSensor.outputData(tmpIMUData);
            midCount ++;
        }
        
        // データを表示
        for (int j = 0; j < dataNum; j++) {
            imuSensor.invAccel(datas[j]);
            imu.printIMUData(datas[j]);
            tmpIMUData = datas[dataNum - 1];
//            imuSensor.outputData(datas[j]);
//            imuPro.renewPose(datas[j]);
//            imuPro.printCurPose();
        }
        
        // 50ms/#data
        usleep(8000);
    }
    
    time(&end);
    long elapsed = (long) difftime(end, start);
    
    gettimeofday(&endt, NULL);
    double elapsed2 = (double)(endt.tv_sec - startt.tv_sec) +
                        (endt.tv_usec - startt.tv_usec) * 1e-6;
    
    std::cout << "count = " << count << " times" << std::endl;
    std::cout << "mid count = " << midCount << " times" << std::endl;
    std::cout << "time = " << elapsed << " s" << std::endl;
    std::cout << "time2 = " << elapsed2 << " s" << std::endl;
    
    return 0;
}
