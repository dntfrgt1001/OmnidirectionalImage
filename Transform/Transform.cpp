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

int Transform::theta2u(float theta) const
{
    return (int)round(theta*frameSize.width/(2.0*M_PI) + frameSize.width/2.0);
}

int Transform::phi2v(float phi) const
{
    return (int)round((-1.0)*phi*frameSize.height/M_PI + frameSize.height/2.0);
}

float Transform::u2theta(int u) const
{
    return (u - frameSize.width/2.0) * 2.0*M_PI/frameSize.width;
}

float Transform::v2phi(int v) const
{
    return (-1.0) * (v - frameSize.height/2.0) * M_PI/frameSize.height;
}

void Transform::ang2orth(float theta, float phi, int &u, int &v) const
{
    u = theta2u(theta);
    v = phi2v(phi);
}

void Transform::orth2ang(int u, int v, float &theta, float &phi) const
{
    theta = u2theta(u);
    phi = v2phi(v);
}

int Transform::dtheta2u(float theta) const
{
    return (int) round(theta * frameSize.width / (2.0 * M_PI));
}

int Transform::dphi2v(float phi) const
{
    return (int) round(phi * frameSize.height / M_PI);
}

void Transform::orth2d2orth3d
(const std::vector<cv::Point2f> &points2d, std::vector<cv::Point3f> &points3d){
    for (int i=0; i<points2d.size(); i++) {
        cv::Point3f point3d;
        orth2d2orth3d(points2d[i], point3d);
        points3d.push_back(point3d);
    }
}

void Transform::orth2d2orth3d
(const cv::Point2f &point2d, cv::Point3f &point3d) const
{
    float u = point2d.x;
    float v = point2d.y;
    
    float theta = u2theta(u);
    float phi = v2phi(v);
    
    point3d.x = sinf(theta) * cosf(phi);
    point3d.y = (-1) * sinf(phi);           // 左上原点座標系
    point3d.z = cosf(theta) * cosf(phi);
}

void Transform::orth3d2orth2d
(const std::vector<cv::Point3f> &points3d, std::vector<cv::Point2f> &points2d)
{
    for (int i=0; i<points3d.size(); i++) {
        cv::Point2f point2d;
        orth3d2orth2d(points3d[i], point2d);
        points2d.push_back(point2d);
    }
}

void Transform::orth3d2orth2d
(const cv::Point3f &point3d, cv::Point2f &point2d) const
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

void Transform::orth3d2orth3dWithRotMat
(const cv::Point3f &forPoint3d, cv::Point3f &latPoint3d, const cv::Mat &rotMat)
const
{
    latPoint3d = (cv::Point3f) cv::Mat1f(rotMat * cv::Mat1f(forPoint3d));
}

void Transform::orth2d2orth2dWithRotMat
(const cv::Point2f &forPoint2d, cv::Point2f &latPoint2d, const cv::Mat &rotMat)
const
{
    cv::Point3f forPoint3d, latPoint3d;
    
    orth2d2orth3d(forPoint2d, forPoint3d);
  
    orth3d2orth3dWithRotMat(forPoint3d, latPoint3d, rotMat);
    
    orth3d2orth2d(latPoint3d, latPoint2d);
}

void Transform::orth2d2orth2dWithRotMat
(int u, int v, int &ur, int &vr, const cv::Mat &rotMat) const
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

void Transform::rotateImgWithRotMat
(const cv::Mat &img, cv::Mat &rotImg, const cv::Mat &rotMat) const
{
    rotImg = cv::Mat(frameSize, CV_8UC3);
    
    cv::Mat invRotMat = rotMat.inv();
    
    for (int ur=0; ur<frameSize.width; ur++) {
        for (int vr=0; vr<frameSize.height; vr++) {
            int u, v;
            
            // 逆の回転行列によって回転後の画素が回転前のどの画素に対応するか求める
            orth2d2orth2dWithRotMat(ur, vr, u, v, rotMat.inv());
            
            rotImg.at<cv::Vec3b>(vr, ur) = img.at<cv::Vec3b>(v, u);
        }
    }
}

void Transform::rotateVerticalImg
(float chi, const cv::Mat &img, cv::Mat &rotImg) const
{
    rotImg = cv::Mat(frameSize.height, frameSize.width, CV_8UC3);
    
    for (int ur=0; ur<frameSize.width; ur++) {
        for (int vr=0; vr<frameSize.height; vr++) {
            int u, v;
            
            rotateVerticalOrthDot(-1.0 * chi, ur, vr, u, v);
            rotImg.at<cv::Vec3b>(vr, ur) = img.at<cv::Vec3b>(v, u);
        }
    }
    
    int halfWidth = frameSize.width / 2;
    // 幅が偶数の場合の幅中心の画素の補間
    if (frameSize.width % 2 == 0) {
        for (int vr=0; vr<frameSize.height; vr++) {
            rotImg.at<cv::Vec3b>(vr, halfWidth-1) =
                2.0/3.0 * rotImg.at<cv::Vec3b>(vr, halfWidth-2) +
                1.0/3.0 * rotImg.at<cv::Vec3b>(vr, halfWidth+1);
            rotImg.at<cv::Vec3b>(vr, halfWidth) =
                1.0/3.0 * rotImg.at<cv::Vec3b>(vr, halfWidth-2) +
                2.0/3.0 * rotImg.at<cv::Vec3b>(vr, halfWidth+1);
        }
    // 幅が奇数の場合の幅中心の画素の補間
    } else {
        for (int vr=0; vr<frameSize.height; vr++) {
            rotImg.at<cv::Vec3b>(vr, halfWidth) =
            1.0/2.0 * rotImg.at<cv::Vec3b>(vr, halfWidth-1) +
            1.0/2.0 * rotImg.at<cv::Vec3b>(vr, halfWidth+1);
        }
    }
}

void Transform::rotateVerticalOrthDot(float chi, int u, int v, int &ur, int &vr)
const
{
    float theta, phi;
    float thetar, phir;
    
    orth2ang(u, v, theta, phi);
    rotateVerticalAngDot(chi, theta, phi, thetar, phir);
    ang2orth(thetar, phir, ur, vr);
    
    if (ur < 0) ur = 0;
    if (frameSize.width-1 < ur) ur = frameSize.width-1;
    if (vr < 0) vr = 0;
    if (frameSize.height-1 < vr) vr = frameSize.height-1;
}

void Transform::rotateVerticalAngDot
(float chi, float theta, float phi, float &thetar, float &phir) const
{
    float thetarDenomi = -sinf(chi)*sinf(phi) + cosf(chi)*cosf(theta)*cosf(phi);
    if (thetarDenomi == 0.0) thetar = M_PI * 1.0/2.0;
    else thetar = atanf(sinf(theta)*cosf(phi) / thetarDenomi);
    
    // thetaの定義域はarctanの定義の定義域よりも広いので場合分け
    if (0 <= theta && thetar < 0) thetar += M_PI;
    if (theta < 0 && 0 <= thetar) thetar -= M_PI;
    
    float AsinInput = cosf(chi)*sinf(phi) + sinf(chi)*cosf(theta)*cosf(phi);
    if (AsinInput < -1)      phir = -1.0 * M_PI * 1.0/ 2.0;
    else if (1 <= AsinInput) phir = M_PI * 1.0/2.0;
    else                     phir = asinf(AsinInput);
}

double Transform::normalizeTheta(float rawTheta) const
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

double Transform::normalizePhi(float rawPhi) const
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

