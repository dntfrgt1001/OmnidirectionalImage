//
//  Rotation.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/07/04.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Rotation.hpp"

void Rotation::getRotMat
(const float angle, const cv::Vec3f &axis, cv::Mat rotMat)
{
    cv::Vec3f rotVec;
    getRotVec(angle, axis, rotVec);
    RotVec2RotMat(rotVec, rotMat);
}

void Rotation::getRotVec
(const float angle, const cv::Vec3f &axis, cv::Vec3f &rotVec)
{
    rotVec = angle * axis;
}

void Rotation::getQuaternion
(const float angle, const cv::Vec3f &axis, Quaternion &quat)
{
    quat = Quaternion(angle, axis);
}

void Rotation::Quat2RotMat
(const Quaternion &quat, cv::Mat &rotMat)
{
    rotMat = cv::Mat(3, 3, CV_32FC1);
    
    rotMat.at<float>(0, 0) = 1 - 2*(quat.y*quat.y + quat.z*quat.z);
    rotMat.at<float>(1, 0) = 2*(quat.x*quat.y + quat.t*quat.z);
    rotMat.at<float>(2, 0) = 2*(quat.x*quat.z - quat.t*quat.y);
    rotMat.at<float>(0, 1) = 2*(quat.x*quat.y - quat.t*quat.z);
    rotMat.at<float>(1, 1) = 1 - 2*(quat.x*quat.x + quat.z*quat.z);
    rotMat.at<float>(2, 1) = 2*(quat.y*quat.z + quat.t*quat.x);
    rotMat.at<float>(0, 2) = 2*(quat.x*quat.z + quat.t*quat.y);
    rotMat.at<float>(1, 2) = 2*(quat.y*quat.z - quat.t*quat.x);
    rotMat.at<float>(2, 2) = 1 - 2*(quat.x*quat.x + quat.y*quat.y);
}

void Rotation::RotMat2Quat(const cv::Mat &rotMat, Quaternion &quat)
{
    float trace = (cv::trace(rotMat))(0);
    
    quat.t = sqrtf(trace + 1) / 2.0;
    quat.x = (rotMat.at<float>(2, 1)-rotMat.at<float>(1, 2)) / (4*quat.t);
    quat.y = (rotMat.at<float>(0, 2)-rotMat.at<float>(2, 0)) / (4*quat.t);
    quat.z = (rotMat.at<float>(1, 0)-rotMat.at<float>(0, 1)) / (4*quat.t);
}

void Rotation::RotVec2RotMat
(const cv::Vec3f& rotVec, cv::Mat &rotMat)
{
    rotMat = cv::Mat(3, 3, CV_32FC1);
    cv::Rodrigues(rotVec, rotMat);
}

void Rotation::RotMat2RotVec
(const cv::Mat &rotMat, cv::Vec3f& rotVec)
{
    cv::Rodrigues(rotMat, rotVec);
}

void Rotation::vec2zDirMat(const cv::Vec3f &vec, cv::Mat &rotMat)
{
    cv::Vec3f uniVec = vec/cv::norm(vec);
    
    // 回転角
    float angle = acosf(uniVec[2]);
    // 回転軸
    float normAxis = sqrtf(uniVec[0]*uniVec[0] + uniVec[1]*uniVec[1]);
    cv::Vec3f axis = cv::Vec3f(uniVec[1], -uniVec[0], 0) / normAxis;
    // 回転ベクトル
    cv::Vec3f rotVec = angle * axis;
    
    RotVec2RotMat(rotVec, rotMat);
}

void Rotation::chgRotMat
(const cv::Mat &rotMat, const cv::Mat &froChgMat, cv::Mat& rotMatChg)
{
    // 回転ベクトルに変換
    cv::Vec3f rotVec;
    RotMat2RotVec(rotMat, rotVec);
    
    // 回転ベクトルを回転
    cv::Vec3f rotVecChg = cv::Vec3f(cv::Mat1f(froChgMat *
                                              cv::Mat1f(rotVec)));
    
    // 回転行列に変換
    RotVec2RotMat(rotVecChg, rotMatChg);
}

void Rotation::normalRotMat(cv::Mat &rotMat)
{
    Quaternion quat;
    RotMat2Quat(rotMat, quat);
    Quaternion::normalQuart(quat);
    Quat2RotMat(quat, rotMat);
}
