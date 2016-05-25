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

int main(int argc, const char * argv[])
{
    std::string path = "/Users/masakazu/Desktop/";
    std::string outfile = "logtest.txt";
    std::string port = "/dev/tty.usbmodem1412";
    
    int buffSize = 4096;
    speed_t baudRate = B57600;
    std::string pattern = "EBS^A";
    InputGyro gyro(path + outfile, port, baudRate, buffSize, pattern);
    
    time_t start, end;
    
    time(&start);
    int count = 0;
    int limit = 1000000;
    sleep(1);
    for (int i=0; i<limit; i++) {
        if(gyro.inputFromGyro() > 0){
            std::cout << '-' << i << '-' << ' ' << std::endl;
            count++;
            gyro.printCurrentAngle();
            //gyro.printCurrentAccel();
            gyro.printCurrentSensorValue();
            usleep(10000);
        }
    }
    time(&end);

    long elapsed = (long) difftime(end, start);
    
    std::cout << "count = " << count << " times" << std::endl;
    std::cout << "time = " << elapsed << " s" << std::endl;
    
    return 0;
}
