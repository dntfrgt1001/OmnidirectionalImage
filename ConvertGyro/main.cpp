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
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string outfile = path + "logtest.txt";
    const std::string port = "/dev/tty.usbmodem1412";
    
    const int bufSize = 4 * 1024;
    const speed_t baudRate = B57600;
//    const std::string splitPat = "EBS^A";
    const char splitPat[] = {'E', 'B', 'S', 0x01};
    
    
    InputGyro gyro(outfile, port, baudRate, bufSize, std::string(""));
    
    const int patSize = sizeof(splitPat)/sizeof(*splitPat);
    IMU imu(outfile, port, baudRate, bufSize, splitPat, patSize);
   
    const char test[] = {-0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                         0x07, 0x08, 0x09, 0x10, 0x11, 0x12,
                         0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                         'E', 'B', 'S', 0x01, 0x41};
    const char test2[] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
        0x47, 0x48, 0x49, 0x50, 0x51, 0x52,
        0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        'E', 'B', 'S', 0x01, 0x59};
  
    std::string testString = test;
//    std::string testString;
//    imu.charInput2strInputBlank
//    (test2, sizeof(test2)/sizeof(*test2), testString);
    std::vector<std::string> extStrings;
    imu.extValidData(testString, extStrings);
    std::vector<short> shortData;
    imu.getShortData(extStrings[0], shortData);
    imu.printData(shortData);
    std::cout << extStrings[1] << std::endl;
    
    time_t start, end;
    
    time(&start);
    int count = 0;
    int iterNum = 10000;

    for (int i = 0; i < iterNum; i++) {
        imu.inputData();
        
        usleep(1000);
        
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
