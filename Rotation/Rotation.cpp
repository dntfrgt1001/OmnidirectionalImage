//
//  Rotation.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/07/04.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Rotation.hpp"

cv::Mat Rotation::Quat2RotMat
(const Quaternion &quat)
{
    return cv::Mat_<float>(3, 3) <<
                1 - 2*(quat.y*quat.y + quat.z*quat.z),
                2*(quat.x*quat.y + quat.t*quat.z),
                2*(quat.x*quat.z - quat.t*quat.y),
                2*(quat.x*quat.y - quat.t*quat.z),
                1 - 2*(quat.x*quat.x + quat.z*quat.z),
                2*(quat.y*quat.z + quat.t*quat.x),
                2*(quat.x*quat.z + quat.t*quat.y),
                2*(quat.y*quat.z - quat.t*quat.x),
                1 - 2*(quat.x*quat.x + quat.y*quat.y);
    
    /*
    rotMat.at<float>(0, 0) = 1 - 2*(quat.y*quat.y + quat.z*quat.z);
    rotMat.at<float>(1, 0) = 2*(quat.x*quat.y + quat.t*quat.z);
    rotMat.at<float>(2, 0) = 2*(quat.x*quat.z - quat.t*quat.y);
    rotMat.at<float>(0, 1) = 2*(quat.x*quat.y - quat.t*quat.z);
    rotMat.at<float>(1, 1) = 1 - 2*(quat.x*quat.x + quat.z*quat.z);
    rotMat.at<float>(2, 1) = 2*(quat.y*quat.z + quat.t*quat.x);
    rotMat.at<float>(0, 2) = 2*(quat.x*quat.z + quat.t*quat.y);
    rotMat.at<float>(1, 2) = 2*(quat.y*quat.z - quat.t*quat.x);
    rotMat.at<float>(2, 2) = 1 - 2*(quat.x*quat.x + quat.y*quat.y);
     */
}

Quaternion Rotation::RotMat2Quat(const cv::Mat &rotMat)
{
    float trace = (cv::trace(rotMat))(0);
    float t = sqrtf(trace + 1) / 2.0;
    return Quaternion(t,
                      (rotMat.at<float>(2,1)-rotMat.at<float>(1,2))/(4*t),
                      (rotMat.at<float>(0,2)-rotMat.at<float>(2,0))/(4*t),
                      (rotMat.at<float>(1,0)-rotMat.at<float>(0,1))/(4*t));
}

cv::Mat Rotation::RotVec2RotMat(const cv::Vec3f& rotVec)
{
    // 回転しないとき
    const float angle = cv::norm(rotVec);
    if (angle == 0.0) return cv::Mat::eye(3, 3, CV_32FC1);
    
    cv::Mat omega = (cv::Mat_<float>(3,3) << 0, -rotVec[2], rotVec[1],
                                             rotVec[2], 0, -rotVec[0],
                                             -rotVec[1], rotVec[0], 0);
    
    return cv::Mat::eye(3, 3, CV_32FC1) +
           sinf(angle)/angle * omega +
           (1 - cosf(angle))/(angle*angle) * omega*omega;
    
    /*
    cv::Mat rotMat = cv::Mat(3, 3, CV_32FC1);
    cv::Rodrigues(rotVec, rotMat);
    return rotMat;
     */
}

cv::Vec3f Rotation::RotMat2RotVec(const cv::Mat &rotMat)
{

    const float trace = (cv::trace(rotMat))[0];
    const float angle = acosf((trace - 1) /2);
    cv::Mat omega = (rotMat - rotMat.t()) / (2*sinf(angle));
    
    return cv::Vec3f(omega.at<float>(2, 1),
                     omega.at<float>(0, 2),
                     omega.at<float>(1, 0)) * angle;

    /*
    cv::Vec3f rotVec;
    cv::Rodrigues(rotMat, rotVec);
    return rotVec;
    */
}

cv::Mat Rotation::getFroChgMat(const cv::Mat &rotMat)
{
    // 単位回転ベクトルの計算
    cv::Vec3f rotVec = RotMat2RotVec(rotMat);
    cv::Vec3f rotVecUni = rotVec/cv::norm(rotVec);
    
    // 回転角
    float angle = acosf(rotVecUni[2]);
    // 回転軸
    float normAxis = sqrtf(rotVecUni[0]*rotVecUni[0] +
                           rotVecUni[1]*rotVecUni[1]);
    cv::Vec3f axis = cv::Vec3f(rotVecUni[1], -rotVecUni[0], 0) / normAxis;
    
    return RotVec2RotMat(angle * axis);
}

cv::Mat Rotation::chgRotMatCoo
(const cv::Mat &rotMat, const cv::Mat &froChgMat)
{
    // 回転ベクトルに変換
    cv::Vec3f rotVec = RotMat2RotVec(rotMat);
    
    // 回転ベクトルを回転させ行列に変換
    return RotVec2RotMat(cv::Vec3f(cv::Mat1f(froChgMat*cv::Mat1f(rotVec))));
}

void Rotation::normalRotMat(cv::Mat &rotMat)
{
    Quaternion quat = RotMat2Quat(rotMat);
    Quaternion::normalQuart(quat);
    rotMat = Quat2RotMat(quat);
}
