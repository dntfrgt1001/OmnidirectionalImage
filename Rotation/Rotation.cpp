//
//  Rotation.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/07/04.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Rotation.hpp"

void Rotation::Quart2RotMat
(const Quarternion &quart, cv::Mat &rotMat)
{
    rotMat = cv::Mat(3, 3, CV_32FC1);
    
    rotMat.at<float>(0, 0) = 1 - 2*(quart.y*quart.y + quart.z*quart.z);
    rotMat.at<float>(1, 0) = 2*(quart.x*quart.y + quart.t*quart.z);
    rotMat.at<float>(2, 0) = 2*(quart.x*quart.z - quart.t*quart.y);
    rotMat.at<float>(0, 1) = 2*(quart.x*quart.y - quart.t*quart.z);
    rotMat.at<float>(1, 1) = 1 - 2*(quart.x*quart.x + quart.z*quart.z);
    rotMat.at<float>(2, 1) = 2*(quart.y*quart.z + quart.t*quart.x);
    rotMat.at<float>(0, 2) = 2*(quart.x*quart.z + quart.t*quart.y);
    rotMat.at<float>(1, 2) = 2*(quart.y*quart.z - quart.t*quart.x);
    rotMat.at<float>(2, 2) = 1 - 2*(quart.x*quart.x + quart.y*quart.y);
}

void Rotation::RotMat2Quart(const cv::Mat &rotMat, Quarternion &quart)
{
    float trace = (cv::trace(rotMat))(0);
    
    quart.t = sqrtf(trace + 1) / 2.0;
    quart.x = (rotMat.at<float>(2, 1) - rotMat.at<float>(1, 2)) / (4*quart.t);
    quart.y = (rotMat.at<float>(0, 2) - rotMat.at<float>(2, 0)) / (4*quart.t);
    quart.z = (rotMat.at<float>(1, 0) - rotMat.at<float>(0, 1)) / (4*quart.t);
}

void Rotation::RotVec2RotMat
(const cv::Vec3f& rotVec, cv::Mat &rotMat)
{
    rotMat = cv::Mat(3, 3, CV_32FC1);
    cv::Rodrigues(rotVec, rotMat);
    /*
    float angle = cv::norm(rotVec);
    float nx = rotVec[0] / angle;
    float ny = rotVec[1] / angle;
    float nz = rotVec[2] / angle;
    
    float cost = cosf(angle);
    float sint = sinf(angle);
    float icost = 1 - cost;
    
    rotMat = cv::Mat(3, 3, CV_32FC1);
    
    rotMat.at<float>(0, 0) = nx*nx*icost + cost;
    rotMat.at<float>(0, 1) = nx*ny*icost - nz*sint;
    rotMat.at<float>(0, 2) = nz*nx*icost + ny*sint;
    rotMat.at<float>(1, 0) = nx*ny*icost + nz*sint;
    rotMat.at<float>(1, 1) = ny*ny*icost + cost;
    rotMat.at<float>(1, 2) = ny*nz*icost - nx*sint;
    rotMat.at<float>(2, 0) = nz*nx*icost - ny*sint;
    rotMat.at<float>(2, 1) = ny*nz*icost + nx*sint;
    rotMat.at<float>(2, 2) = nz*nz*icost + cost;
     */
}

void Rotation::RotMat2RotVec
(const cv::Mat &rotMat, cv::Vec3f& rotVec)
{
    cv::Rodrigues(rotMat, rotVec);
    
    /*
    Quarternion quart;
    RotMat2Quart(rotMat, quart);
    
    float halfAngle = acosf(quart.t);
    // 回転角がゼロの場合
    if (halfAngle == 0.0) {
        angle = 0.0;
        axis[0] = axis[1] = axis[2] = 1.0;
    } else {
        angle = halfAngle * 2.0;
        axis[0] = quart.x / sinf(halfAngle);
        axis[1] = quart.y / sinf(halfAngle);
        axis[2] = quart.z / sinf(halfAngle);
    }
     */
}

void Rotation::normalRotMat(cv::Mat &rotMat)
{
    Quarternion quart;
    RotMat2Quart(rotMat, quart);
    Quarternion::normalQuart(quart);
    Quart2RotMat(quart, rotMat);
}