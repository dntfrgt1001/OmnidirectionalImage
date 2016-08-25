//
//  Quarternion.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/10.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Quarternion.hpp"

Quarternion::Quarternion()
{
    float theta = 0.0;
    cv::Vec3f axis(1.0, 1.0, 1.0);
    Quarternion(theta, axis);
}

Quarternion::Quarternion(float theta, cv::Vec3f& axis)
{
    float norm = cv::norm(axis);
    
    t = cosf(theta/2.0);
    x = axis[0]/norm * sinf(theta/2.0);
    y = axis[1]/norm * sinf(theta/2.0);
    z = axis[2]/norm * sinf(theta/2.0);
}

Quarternion::~Quarternion()
{
    
}

float Quarternion::norm(Quarternion &quart)
{
    return sqrtf(quart.t*quart.t + quart.x*quart.x +
                 quart.y*quart.y + quart.z*quart.z);
}

void Quarternion::transCounterRot()
{
    x = -x;
    y = -y;
    z = -z;
}

void Quarternion::Quart2RotMat
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

void Quarternion::RotMat2Quart(const cv::Mat &rotMat, Quarternion &quart)
{
    float trace = (cv::trace(rotMat))(0);
    
    quart.t = sqrtf(trace + 1) / 2.0;
    
    quart.x = (rotMat.at<float>(2, 1) - rotMat.at<float>(1, 2)) / (4*quart.t);
    quart.y = (rotMat.at<float>(0, 2) - rotMat.at<float>(2, 0)) / (4*quart.t);
    quart.z = (rotMat.at<float>(1, 0) - rotMat.at<float>(0, 1)) / (4*quart.t);
}

void Quarternion::Rodrigues2RotMat
(const float angle, const cv::Vec3f& axis, cv::Mat &rotMat)
{
    float norm = cv::norm(axis);
    float nx = axis[0] / norm;
    float ny = axis[1] / norm;
    float nz = axis[2] / norm;
    
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
}

void Quarternion::RotMat2Rodrigues
(const cv::Mat &rotMat, float &angle, cv::Vec3f &axis)
{
    Quarternion quart;
    RotMat2Quart(rotMat, quart);
    
    float halfAngle = acosf(quart.t);
    // 回転角がゼロの場合，回転軸は不定
    if (halfAngle == 0.0) {
        angle = 0.0;
        axis[0] = axis[1] = axis[2] = 0.0;
    } else {
        angle = halfAngle * 2.0;
        axis[0] = quart.x / sinf(halfAngle);
        axis[1] = quart.y / sinf(halfAngle);
        axis[2] = quart.z / sinf(halfAngle);
    }
}

void Quarternion::normalQuart(Quarternion &quart)
{
    float qnorm = norm(quart);
    
    quart.t /= qnorm; quart.x /= qnorm; quart.y /= qnorm; quart.z /= qnorm;
}

void Quarternion::normalRotMat(cv::Mat &rotMat)
{
    Quarternion quart;
    RotMat2Quart(rotMat, quart);
    normalQuart(quart);
    Quart2RotMat(quart, rotMat);
}

std::ostream& operator<<(std::ostream& os, const Quarternion& quart)
{
    return (os << "[" << quart.t << "; " << quart.x << ", "
            << quart.y << ", " << quart.z << "]");
}


