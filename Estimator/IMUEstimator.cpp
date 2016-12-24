//
//  IMUEstimator.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/12/21.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "IMUEstimator.hpp"

IMUEstimator::IMUEstimator(const Transform& tf, IMU& imu, const float dt):
// 状態の次元[w, x, y, z, bx, by, bz]
// 観測の次元[gx, gy, gz]
// 制御の次元[]
Estimator(tf), imu(imu), kalman(cv::KalmanFilter(7, 3, 0, CV_32FC1)), dt(dt)
{
    // 推移行列の初期化 ダミー
    cv::setIdentity(kalman.transitionMatrix);
    
    // 観測行列の初期化 ダミー
    cv::setIdentity(kalman.measurementMatrix);
    
    // 遷移誤差分散共分散行列の初期化
    cv::setIdentity(kalman.processNoiseCov, cv::Scalar::all(1e-5));
    
    // 観測誤差分散共分散行列の初期化
    cv::setIdentity(kalman.measurementNoiseCov, cv::Scalar::all(1));
    
    // 誤差分散共分散行列の初期化
    cv::setIdentity(kalman.errorCovPost, cv::Scalar::all(1));
    
    // 状態の初期化 q = [1: 0, 0, 0], b = [0, 0, 0]
    kalman.statePost = (cv::Mat_<float>(7,1) << 1, 0, 0, 0, 0, 0, 0);
}

cv::Mat IMUEstimator::getTransMat(const cv::Vec3f &angVel)
{
    // 現在のバイアスベクトルを取り出す
    cv::Vec3f biasVec(kalman.statePre.at<float>(4),
                      kalman.statePre.at<float>(5),
                      kalman.statePre.at<float>(6));

    // バイアスを考慮した回転ベクトル
    cv::Vec3f rotVecBias = angVel * dt - biasVec;
    const float  angleBias = cv::norm(rotVecBias);
    
    // 無回転なら単位行列を返す
    if (angleBias == 0.0) return cv::Mat::eye(7, 7, CV_32FC1);
    
    // バイアスを考慮した回転の四元数
    Quaternion quatBias(angleBias, rotVecBias);

    return cv::Mat_<float>(7, 7) <<
        quatBias.w, -quatBias.x, -quatBias.y, -quatBias.z, 0, 0, 0,
        quatBias.x,  quatBias.w, -quatBias.z,  quatBias.y, 0, 0, 0,
        quatBias.y,  quatBias.z,  quatBias.w, -quatBias.x, 0, 0, 0,
        quatBias.z, -quatBias.y,  quatBias.x,  quatBias.w, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 1;
}

cv::Mat IMUEstimator::getObserMat(const cv::Vec3f &angVel)
{
    
    
    
}




