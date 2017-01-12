//
//  IMUEstimator.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/12/21.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "IMUEstimator.hpp"

IMUEstimator::IMUEstimator(const Transform& tf, IMU& imu):
// 状態の次元[w, x, y, z, bx, by, bz]
// 観測の次元[gx, gy, gz]
// 制御の次元[]
Estimator(tf), imu(imu), kalman(cv::KalmanFilter(7, 3, 0, CV_32FC1))
{
    // 遷移行列の初期化 ダミー
    cv::setIdentity(kalman.transitionMatrix);
    
    // 観測行列の初期化 ダミー
    cv::setIdentity(kalman.measurementMatrix);
    
    // 遷移誤差分散共分散行列の初期化
    cv::setIdentity(kalman.processNoiseCov, cv::Scalar::all(1e-5));
    
    // 観測誤差分散共分散行列の初期化
    cv::setIdentity(kalman.measurementNoiseCov, cv::Scalar::all(0.5));
    
    // 誤差分散共分散行列の初期化
    cv::setIdentity(kalman.errorCovPost, cv::Scalar::all(0.5));
    
    // 状態の初期化 q = [1: 0, 0, 0], b = [0, 0, 0]
    kalman.statePost = (cv::Mat_<float>(7,1) << 1, 0, 0, 0, 1e-3, 1e-3, 1e-3);
//    kalman.statePost = (cv::Mat_<float>(7,1) << 1/sqrtf(2), -1/sqrtf(2), 0, 0, 0, 0, 0);
    
    printMatrix();
    
}

cv::Mat IMUEstimator::getTransMat(const cv::Vec3f &angVel)
{
    // 現在のバイアスベクトルを取り出す
    cv::Vec3f biasVec(kalman.statePre.at<float>(4),
                      kalman.statePre.at<float>(5),
                      kalman.statePre.at<float>(6));

    // バイアスを考慮した回転ベクトル
    cv::Vec3f rotVecBias = angVel - biasVec;
    const float angleBias = cv::norm(rotVecBias);
    
    // 無回転なら単位行列を返す
    if (angleBias == 0.0) return cv::Mat::eye(7, 7, CV_32FC1);
    
    // バイアスを考慮した回転の四元数
    Quaternion quatBias(angleBias, rotVecBias);

    std::cout << "cur rot quaternion = " << std::endl << quatBias << std::endl;
    
    return cv::Mat_<float>(7, 7) <<
        quatBias.w, -quatBias.x, -quatBias.y, -quatBias.z, 0, 0, 0,
        quatBias.x,  quatBias.w, -quatBias.z,  quatBias.y, 0, 0, 0,
        quatBias.y,  quatBias.z,  quatBias.w, -quatBias.x, 0, 0, 0,
        quatBias.z, -quatBias.y,  quatBias.x,  quatBias.w, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 1;
}

cv::Mat IMUEstimator::getMeasureMat()
{
    // 重力加速度
    const float g = 9.8;
    
    // 予測後の四元数
    Quaternion quat(kalman.statePre.at<float>(0),
                    kalman.statePre.at<float>(1),
                    kalman.statePre.at<float>(2),
                    kalman.statePre.at<float>(3));
    
    // 観測行列を返す
    return g * (cv::Mat_<float>(3,7) <<
                 quat.z,  quat.y,  quat.x,  quat.w, 0, 0, 0,
                 quat.w, -quat.x,  quat.y, -quat.z, 0, 0, 0,
                -quat.x, -quat.w,  quat.x,  quat.y, 0, 0, 0);
}


cv::Mat IMUEstimator::getRotMat
(const cv::Mat &forImg, const cv::Mat &latImg, const int frameNum)
{
    // IMUデータの取り出し
    std::vector<IMUData> datas;
    const int dataNum = imu.inputDataSet(datas);
    
    // 姿勢変化集積
    cv::Mat rotMat = cv::Mat::eye(3, 3, CV_32FC1);
    
    // データ組数だけ更新処理
    for (int i = 0; i < dataNum; i++) {
        updatePose(datas[i]);
    }
    
    // 更新処理後の姿勢を回転行列で返す
    return Rotation::Quat2RotMat(Quaternion
                                 (kalman.statePost.at<float>(0),
                                  kalman.statePost.at<float>(1),
                                  kalman.statePost.at<float>(2),
                                  kalman.statePost.at<float>(3)));
}

void IMUEstimator::updatePose(const IMUData& data)
{
    std::cout << "prev state = " << std::endl << kalman.statePost.t() << std::endl;

    
    // センサ値から回転ベクトルを得る
    cv::Vec3f angVelVec(IMU::getGyroValue(data.gyro_x),
                        IMU::getGyroValue(data.gyro_y),
                        IMU::getGyroValue(data.gyro_z));
    
    std::cout << "rot vector = " << std::endl << angVelVec << std::endl;
    
    // 現在の遷移行列を設定
    kalman.transitionMatrix = getTransMat(angVelVec);
    
    std::cout << "transition matrix = " << std::endl << kalman.transitionMatrix << std::endl;
    
    // 予測
    kalman.predict();
    
    // 現在の観測行列を設定
    kalman.measurementMatrix = getMeasureMat();
    
    // センサ値から加速度を得る
    cv::Mat measure = (cv::Mat_<float>(3, 1) <<
                       IMU::getAccelValue(data.accel_x),
                       IMU::getAccelValue(data.accel_y),
                       IMU::getAccelValue(data.accel_z));
    
    std::cout << "measure = " << std::endl << measure << std::endl;
    std::cout << "measure mult = " << std::endl << kalman.measurementMatrix*kalman.statePre << std::endl;
    std::cout << "error = " << measure - kalman.measurementMatrix*kalman.statePre << std::endl;
    
    std::cout << "measurement matrix = " << std::endl << kalman.measurementMatrix << std::endl;
    
    // 修正
    kalman.correct(measure);
    
    // 四元数を正規化
    normalQuat();
    
    std::cout << "next state = " << std::endl << kalman.statePost.t() << std::endl;
}

void IMUEstimator::normalQuat()
{
    Quaternion quat(kalman.statePost.at<float>(0),
                    kalman.statePost.at<float>(1),
                    kalman.statePost.at<float>(2),
                    kalman.statePost.at<float>(3));
    
    Quaternion::normalQuart(quat);
    
    kalman.statePost.at<float>(0) = quat.w;
    kalman.statePost.at<float>(1) = quat.x;
    kalman.statePost.at<float>(2) = quat.y;
    kalman.statePost.at<float>(3) = quat.z;
}
void IMUEstimator::printMatrix()
{
    std::cout << "transition matrix = " << std::endl << kalman.transitionMatrix << std::endl;
    
    std::cout << "measurement matrix = " << std::endl << kalman.measurementMatrix << std::endl;
    
    std::cout << "process noise covariance matrix = " << std::endl << kalman.processNoiseCov << std::endl;
    
    std::cout << "measurement noise covariance matrix = " << std::endl << kalman.measurementNoiseCov << std::endl;
    
    std::cout << "post state matrix = " << std::endl << kalman.statePost << std::endl;
    
    std::cout << "post error covariance matrix = " << std::endl << kalman.errorCovPost << std::endl;
}


