//
//  Transform.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/01/10.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Transform.hpp"

Transform::Transform(const cv::Size& frameSize):
frameSize(frameSize)
{
}

int Transform::theta2u(double theta) const
{
    return (int)round(theta*frameSize.width/(2.0*M_PI) + frameSize.width/2.0);
}

int Transform::phi2v(double phi) const
{
    return (int)round((-1.0)*phi*frameSize.height/M_PI + frameSize.height/2.0);
}

double Transform::u2theta(int u) const
{
    return (u - frameSize.width/2.0) * 2.0*M_PI/frameSize.width;
}

double Transform::v2phi(int v) const
{
    return (-1.0) * (v - frameSize.height/2.0) * M_PI/frameSize.height;
}

void Transform::ang2orth(double theta, double phi, int &u, int &v) const
{
    u = theta2u(theta);
    v = phi2v(phi);
}

void Transform::orth2ang(int u, int v, double &theta, double &phi) const
{
    theta = u2theta(u);
    phi = v2phi(v);
}

int Transform::dtheta2u(double theta) const
{
    return (int) round(theta * frameSize.width / (2.0 * M_PI));
}

int Transform::dphi2v(double phi) const
{
    return (int) round(phi * frameSize.height / M_PI);
}

void Transform::orth2d2orth3d(const cv::Point2f &point2d, cv::Point3f &point3d)
{
    float u = point2d.x;
    float v = point2d.y;
    
    float theta = u2theta(u);
    float phi = v2phi(v);
    
    point3d.x = sinf(theta) * cosf(phi);
    point3d.y = (-1) * sinf(phi);           // 左上原点座標系
    point3d.z = cosf(theta) * cosf(phi);
}

void Transform::orth3d2orth2d(const cv::Point3f &point3d, cv::Point2f &point2d)
{
    float x = point3d.x;
    float y = point3d.y;
    float z = point3d.z;
    
    float theta = atanf(x / z);
    float phi = asinf(-1 * y);
    
    if (z < 0) {
        if (x < 0) theta = theta - M_PI;
        else       theta = theta + M_PI;
    }
    
    point2d.x = theta2u(theta);
    point2d.y = phi2v(phi);
}

void Transform::orth2d2orth2dWithRotMat
(const cv::Point2f &forPoint2d, cv::Point2f &latPoint2d, const cv::Mat &rotMat)
{
    cv::Point3f forPoint3d, latPoint3d;
    
    orth2d2orth3d(forPoint2d, forPoint3d);
    
    latPoint3d = (cv::Point3f) cv::Mat1f(rotMat * cv::Mat1f(forPoint3d));
    
    orth3d2orth2d(latPoint3d, latPoint2d);
}

void Transform::orth2d2orth2dWithRotMat
(int u, int v, int &ur, int &vr, const cv::Mat &rotMat)
{
    cv::Point2f forPoint2d;
    forPoint2d.x = u;
    forPoint2d.y = v;
    
    cv::Point2f latPoint2d;
    orth2d2orth2dWithRotMat(forPoint2d, latPoint2d, rotMat);
    
    ur = latPoint2d.x;
    vr = latPoint2d.y;
    
    if (ur < 0) ur = 0; if(frameSize.width <= ur) ur = frameSize.width-1;
    if (vr < 0) vr = 0; if(frameSize.height <= vr) vr = frameSize.height-1;
}

#include <iostream>

void Transform::rotateImgWithRotMat
(const cv::Mat &img, cv::Mat &rotImg, const cv::Mat &rotMat)
{
    rotImg = cv::Mat(frameSize, CV_8UC3);
    
    cv::Mat invRotMat = rotMat.inv();
    
    std::cout << rotMat << std::endl;
    std::cout << invRotMat << std::endl ;
    
    for (int ur=0; ur<frameSize.width; ur++) {
        for (int vr=0; vr<frameSize.height; vr++) {
            int u, v;
            
            orth2d2orth2dWithRotMat(ur, vr, u, v, rotMat);
            
            rotImg.at<cv::Vec3b>(vr, ur) = img.at<cv::Vec3b>(v, u);
        }
    }
}

double Transform::normalizeTheta(double rawTheta) const
{
    while (rawTheta < -1.0 * M_PI) rawTheta += 2.0*M_PI;
    while (M_PI <= rawTheta)       rawTheta -= 2.0*M_PI;
    
    /*
    while (rawTheta < 0)        rawTheta += 2.0*M_PI;
    while (2*M_PI <= rawTheta)  rawTheta -= 2.0*M_PI;
    */
    return rawTheta;
}

int Transform::normalizeU(int rawU) const
{
    while (rawU < 0)                rawU += frameSize.width;
    while (frameSize.width <= rawU) rawU -= frameSize.width;
    
    return rawU;
}

double Transform::normalizePhi(double rawPhi) const
{
    while (rawPhi < -1.0 * M_PI / 2.0) rawPhi += M_PI / 2.0;
    while (M_PI / 2.0 <= rawPhi)       rawPhi -= M_PI / 2.0;
    
    return rawPhi;
}

int Transform::normalizeV(int rawV) const
{
    while (rawV < 0)                 rawV += frameSize.height;
    while (frameSize.height <= rawV) rawV -= frameSize.height;

    return rawV;
}

