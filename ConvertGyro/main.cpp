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

#include "InputGyro.hpp"
#include "IMU.hpp"



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
    const std::string outfile = path + "logtest.txt";
    const std::string port = "/dev/tty.usbmodem1412";
    
    const long bufSize = 4 * 1024 * 1024;
    const speed_t baudRate = B57600;
//    const char splitPattern[] = {'E', 'B', 'S', 0x01};
    const char splitPattern[] = {'E', 'B'};
    
    //InputGyro gyro(outfile, port, baudRate, bufSize, std::string(""));
    
    const int patSize = sizeof(splitPattern)/sizeof(*splitPattern);
    IMU imu(outfile, port, baudRate, bufSize, splitPattern, patSize);
   
    /*
    char test1[bufSize] = {0x00,
                         -0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                         0x07, 0x08, 0x09, 0x10, 0x11, 0x12,
                         0x13, 0x14, 0x15, 0x16, 0x17, 0x00,
                         'E', 'B', 'S', 0x01, 0x7f, 0x1a};
    char test2[bufSize] = {-0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
                          0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
                          0x27, 0x28, 0x29, 0x00,
                         'E', 'B', 'S', 0x01, 0x2b};
    
    char test1[bufSize] = {0x41,
                           0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                           0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d,
                           0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53,
                           'E', 'B', 'S', 0x01, 0x54, 0x55};
    char test2[bufSize] = {0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b,
                           0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61,
                           0x62, 0x63, 0x64, 0x65,
                           'E', 'B', 'S', 0x01, 0x66, 0x67};
    */
//    std::string testString1 = test1;
//    std::string testString2 = test2;
    
//    imu.inputData(test1, 25);
    
//    imu.inputData(test2, 21);
    
    
    
    time_t start, end;
    
    time(&start);
    int count = 0;
    int iterNum = 10000;

    for (int i = 0; i < iterNum; i++) {
        imu.inputData();
        
        usleep(5000);
        
        /*
        if(gyro.inputFromGyro() > 0){
            std::cout << '-' << i << '-' << ' ' << std::endl;
            count++;
            //gyro.printCurrentAngle();
            //gyro.printCurrentAccel();
            //gyro.printCurrentSensorValue();
            usleep(3000);
            i++;
        }
        std::cout << "-----" << std::endl;
    */
    }
    time(&end);

    long elapsed = (long) difftime(end, start);
    
    std::cout << "count = " << count << " times" << std::endl;
    std::cout << "time = " << elapsed << " s" << std::endl;
    
    return 0;
}
