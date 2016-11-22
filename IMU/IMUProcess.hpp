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

#include "IMU.hpp"

class IMUProcess {
public:
    IMUProcess(IMU& imu);
    
    bool filterMagZero(IMUData& data);
    
private:
    IMU& imu;
};

#endif /* IMUProcess_hpp */
