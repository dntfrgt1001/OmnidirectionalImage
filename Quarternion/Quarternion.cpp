//
//  Quarternion.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/10.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Quarternion.hpp"

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

void Quarternion::arbRotMat(float theta, cv::Vec3f& axis, cv::Mat &rotMat)
{
    float norm = cv::norm(axis);
    float nx = axis[0] / norm;
    float ny = axis[1] / norm;
    float nz = axis[2] / norm;
    
    float cost = cosf(theta);
    float sint = sinf(theta);
    float icost = 1 - cost;
    
    rotMat = cv::Mat(3, 3, CV_32FC1);

    rotMat.at<float>(0, 0) = nx*nx*icost + cost;
    rotMat.at<float>(1, 0) = nx*ny*icost - nz*sint;
    rotMat.at<float>(2, 0) = nz*nx*icost + ny*sint;
    rotMat.at<float>(0, 1) = nx*ny*icost + nz*sint;
    rotMat.at<float>(1, 1) = ny*ny*icost + cost;
    rotMat.at<float>(2, 1) = ny*nz*icost - nx*sint;
    rotMat.at<float>(0, 2) = nz*nx*icost - ny*sint;
    rotMat.at<float>(1, 2) = ny*nz*icost + nx*sint;
    rotMat.at<float>(2, 2) = nz*nz*icost + cost;
}

void Quarternion::normalizeRotMat(cv::Mat &rotMat)
{
    cv::Vec3f a, b, c;
    cv::Vec3f e1, e2, e3;
    cv::Vec3f f1, f2, f3;
    
    a = rotMat.col(0);
    b = rotMat.col(1);
    c = rotMat.col(2);
    
    f1 = a;
    e1 = 1.0/cv::norm(f1) * f1;
    
    f2 = b - b.dot(e1) * e1;
    e2 = 1.0/cv::norm(f2) * f2;
    
    f3 = c - c.dot(e1) * e1 - c.dot(e2) * e2;
    e3 = 1.0/cv::norm(f3) * f3;
    
    for (int v=0; v<3; v++) {
        rotMat.at<float>(v, 0) = e1[v];
        rotMat.at<float>(v, 1) = e2[v];
        rotMat.at<float>(v, 2) = e3[v];
    }
}






