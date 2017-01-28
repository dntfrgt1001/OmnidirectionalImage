//
//  IMU.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/22.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "IMU.hpp"

const float IMU::dt = 1.0 / 30.0;
const float IMU::sensAccel = 0.00061;
const float IMU::sensGyro = 0.00763 *  M_PI / 180;

void IMU::printIMUData(const IMUData &data)
{
    std::ios::fmtflags flagsSaved = std::cout.flags();
    char fillSaved = std::cout.fill();

    std::cout << "[ " << std::setw(6) << data.accel_x << " "
                      << std::setw(6) << data.accel_y << " "
                      << std::setw(6) << data.accel_z << " "
                      << std::setw(6) << data.gyro_x << " "
                      << std::setw(6) << data.gyro_y << " "
                      << std::setw(6) << data.gyro_z << " "
                      << std::setw(6) << data.mag_x << " "
                      << std::setw(6) << data.mag_y << " "
                      << std::setw(6) << data.mag_z << " "
              << " ]" << std::endl;
    
    std::cout.fill(fillSaved);
    std::cout.flags(flagsSaved);
}
