//
//  IMUProcess.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/22.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef IMUProcess_hpp
#define IMUProcess_hpp

#include <stdio.h>

typedef struct IMU_Data {
    int accel_x, accel_y, accel_z;
    int gyro_x, gyro_y, gyro_z;
    int mag_x, mag_y, mag_z;
} IMU_Data;

class IMUProcess {
public:
    IMUProcess();
    
    
};

#endif /* IMUProcess_hpp */
