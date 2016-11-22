//
//  IMUFile.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/21.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef IMUFile_hpp
#define IMUFile_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>

typedef struct IMU_Data {
    int accel_x, accel_y, accel_z;
    int gyro_x, gyro_y, gyro_z;
    int mag_x, mag_y, mag_z;
} IMU_Data;

class IMUFile {
public:
    IMUFile(const std::string& fileName);
    ~IMUFile();
    
    // データを一組取り出す
    bool inputDataSet(IMU_Data& data);
    
private:
    std::ifstream ifs;
    
};

#endif /* IMUFile_hpp */
