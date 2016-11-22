//
//  IMUProcess.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/22.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "IMUProcess.hpp"

IMUProcess::IMUProcess(IMU& imu):
imu(imu)
{
    
}

bool IMUProcess::filterMagZero(IMUData &data)
{
    if (data.mag_x==0 && data.mag_y==0 && data.mag_z==0) {
        return true;
    } else {
        return false;
    }
}
