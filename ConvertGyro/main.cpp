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
*/
    
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string outfile = path + "logtest.txt";
    const std::string port = "/dev/tty.usbmodem1412";
    
    const int bufSize = 4 * 1024;
    const speed_t baudRate = B57600;
    const char splitPat[] = {'E', 'B', 'S', 0x01, '\0'};
    
    InputGyro gyro(outfile, port, baudRate, bufSize, std::string(""));
    
    const int patSize = sizeof(splitPat)/sizeof(*splitPat);
    IMU imu(outfile, port, baudRate, bufSize, splitPat, patSize);
   
    const char test1[] = {0x00,
                         0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                         0x07, 0x08, 0x09, 0x10, 0x11, 0x12,
                         0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                         'E', 'B', 'S', 0x19, 0x20};
    const char test2[] = {0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
                          0x27, 0x28, 0x29, 0x30, 0x31, 0x32,
                          0x33, 0x34, 0x35, 0x36,
                         'E', 'B', 'S', 0x01, 0x59};
  
    std::string testString1 = test1;
    std::string testString2 = test2;
    
    
    
    
    
    
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
