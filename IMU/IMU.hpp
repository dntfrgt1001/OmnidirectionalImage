//
//  IMU.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/22.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef IMU_hpp
#define IMU_hpp

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <vector>


typedef struct IMUData {
    int accel_x, accel_y, accel_z;
    int gyro_x, gyro_y, gyro_z;
    int mag_x, mag_y, mag_z;
} IMUData;

 
class IMU {
public:
    IMU(){};
    
    virtual int inputDataSet(std::vector<IMUData>& datas) = 0;
    
    static void printIMUData(const IMUData& data);
    
    static float getAccelValue(const int sensorValue) {
        return sensorValue * sensAccel * dt;
    }
    static float getGyroValue(const int sensorValue) {
        return sensorValue * sensGyro * dt;
    };
    
private:
    // データ更新時間感覚
    static const float dt;
    // センサデータの感度
    static const float sensAccel;
    static const float sensGyro;
};

#endif /* IMU_hpp */
