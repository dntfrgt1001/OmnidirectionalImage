//
//  IMUEstimator.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/12/21.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef IMUEstimator_hpp
#define IMUEstimator_hpp

#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>

#include "Estimator.hpp"
#include "IMU.hpp"
#include "Transform.hpp"
#include "Quaternion.hpp"
#include "Rotation.hpp"

class IMUEstimator: public Estimator {
public:
    IMUEstimator(const Transform& tf, IMU& imu);

    // 画像入力はダミー
    cv::Mat getRotMat
    (const cv::Mat& forImg, const cv::Mat& latImg, const int frameNum);
  
    // 角速度から遷移行列を生成（線型）
    cv::Mat getTransMat(const cv::Vec3f& angVel);
    
    // 観測行列を生成(非線形)
    cv::Mat getMeasureMat();

    // 姿勢更新
    void updatePose(const IMUData& data);
    
    // 行列の確認
    void printMatrix();
    
    // 四元数を正規化
    void normalQuat();
    
private:
    IMU& imu;

    cv::KalmanFilter kalman;
};

#endif /* IMUEstimator_hpp */
