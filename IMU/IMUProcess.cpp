//
//  IMUProcess.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/22.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "IMUProcess.hpp"

IMUProcess::IMUProcess(IMU& imu):
imu(imu), quatCur(1, 0, 0, 0), matCur(cv::Mat::eye(3, 3, CV_32FC1)),
dt(0.008), sens(0.03)
{
    
}

bool IMUProcess::filterMagZero(const IMUData &data)
{
    if (data.mag_x==0 && data.mag_y==0 && data.mag_z==0) {
        return true;
    } else {
        return false;
    }
}

void IMUProcess::renewPose(const IMUData &data)
{
    if (filterMagZero(data) && filterGyroDrift(data)) {
    
    // 角速度ベクトル
    cv::Vec3f angVel(data.gyro_x * sens * dt,
                     data.gyro_y * sens * dt,
                     data.gyro_z * sens * dt);
    
    // 単位行列 + 無限小回転行列
    cv::Mat infEyeMat = cv::Mat::eye(3, 3, CV_32FC1) +
                            Rotation::getInfRot(angVel);
    
    // 更新
    matCur = infEyeMat * matCur;
    
    } else {
        return;
    }
}

void IMUProcess::printCurPose()
{
    std::cout << matCur << std::endl;
}
