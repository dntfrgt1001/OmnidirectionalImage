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
#include "Quaternion.hpp"
#include "Rotation.hpp"

class IMUProcess {
public:
    IMUProcess(IMU& imu);
    
    // コンパスが0でないデータをフィルタ
    bool isMagZero(const IMUData& data);
    
    
    // ジャイロが閾値以下のデータをフィルタ
    int filterGyroDrift(const int gyro) {
        if (abs(gyro) < 100) return 0; else return gyro;
    }
    
    // 姿勢を更新
    void renewPose(const IMUData& data);
    
    // 現在の姿勢を出力
    void printCurPose();
    
private:
    // 現在の姿勢
    cv::Mat matCur;
    Quaternion quatCur;
    
    // センサー
    IMU& imu;
};

#endif /* IMUProcess_hpp */
