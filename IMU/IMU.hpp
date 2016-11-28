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

/*
typedef struct IMUData {
    int accel_x, accel_y, accel_z;
    int gyro_x, gyro_y, gyro_z;
    int mag_x, mag_y, mag_z;
} IMUData;
*/
 
class IMU {
public:
    IMU(){};
    
    virtual int inputDataSet(std::vector<IMUData>& datas) = 0;
    
    static void printIMUData(const IMUData& data);
};


class IMUData {
public:
    IMUData();
    IMUData(const int accel_x, const int accel_y, const int accel_z,
            const int gyro_x, const int gyro_y, const int gyro_z,
            const int mag_x, const int mag_y, const int mag_z);
    
    void 
    
    int accel_x, accel_y, accel_z;
    int gyro_x, gyro_y, gyro_z;
    int mag_x, mag_y, mag_z;
    
private:
    // 更新時間間隔
    static const float dt;
    // センサ感度
    static const float senGyro;
};

IMUData::IMUData():IMUData(0,0,0,0,0,0,0,0,0) {}

IMUData::IMUData(const int accel_x, const int accel_y, const int accel_z,
                 const int gyro_x, const int gyro_y, const int gyro_z,
                 const int mag_x, const int mag_y, const int mag_z):
accel_x(accel_x), accel_y(accel_y), accel_z(accel_z),
gyro_x(gyro_x), gyro_y(gyro_y), gyro_z(gyro_z),
mag_x(mag_x), mag_y(mag_y), mag_z(mag_z) {}

const float IMUData::dt = 0.008;
const float IMUData::senGyro = 0.03;

#endif /* IMU_hpp */
