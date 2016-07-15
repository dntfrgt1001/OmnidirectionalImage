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

void Transform::orth2D2orth3D
(const std::vector<cv::Point2f> &points2d, std::vector<cv::Point3f> &points3d)
const
{
    points3d.clear();
    
    for (int i=0; i<points2d.size(); i++) {
        cv::Point3f point3d;
        orth2D2orth3D(points2d[i], point3d);
        points3d.push_back(point3d);
    }
}

void Transform::orth2D2orth3D
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

void Transform::orth3D2orth2D
(const std::vector<cv::Point3f> &points3d, std::vector<cv::Point2f> &points2d)
const
{
    points2d.clear();
    
    for (int i=0; i<points3d.size(); i++) {
        cv::Point2f point2d;
        orth3D2orth2D(points3d[i], point2d);
        points2d.push_back(point2d);
    }
}

void Transform::orth3D2orth2D
(const cv::Point3f &point3d, cv::Point2f &point2d) const
{
    float x = point3d.x;
    float y = point3d.y;
    float z = point3d.z;
    
    // [z = 0]の場合はxの値によって直接θを代入
    float theta = (z != 0)? atanf(x / z): ((x > 0)? M_PI/2.0: -M_PI/2.0);
    // 逆三角関数の定義域の範囲外を防ぐ
    float phi = y<-1.0? asinf(-1.0): (1.0<y? asinf(1.0): asinf(-1 * y));
    
    if (z < 0) {
        if (x < 0) theta = theta - M_PI;
        else theta = theta + M_PI;
    }
    
    point2d.x = theta2u(theta);
    point2d.y = phi2v(phi);
}

void Transform::orth3D2orth3DWithRotMat
(const cv::Point3f &forPoint3d, cv::Point3f &latPoint3d, const cv::Mat &rotMat)
const
{
    latPoint3d = (cv::Point3f) cv::Mat1f(rotMat * cv::Mat1f(forPoint3d));
}

void Transform::orth2D2orth2DWithRotMat
(const cv::Point2f &forPoint2d, cv::Point2f &latPoint2d, const cv::Mat &rotMat)
const
{
    cv::Point3f forPoint3d, latPoint3d;
    
    orth2D2orth3D(forPoint2d, forPoint3d);
    orth3D2orth3DWithRotMat(forPoint3d, latPoint3d, rotMat);
    orth3D2orth2D(latPoint3d, latPoint2d);
    
    assert(!isnan(latPoint2d.y));
}

void Transform::orth2D2orth2DWithRotMat
(float u, float v, float &ur, float &vr, const cv::Mat &rotMat) const
{
    cv::Point2f forPoint2d;
    forPoint2d.x = u;   forPoint2d.y = v;
    
    cv::Point2f latPoint2d;
    orth2D2orth2DWithRotMat(forPoint2d, latPoint2d, rotMat);
    
    ur = latPoint2d.x;  vr = latPoint2d.y;
    
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
            float u, v;
            
            // 逆の回転行列によって回転後の画素が回転前のどの画素に対応するか求める
            orth2D2orth2DWithRotMat(ur, vr, u, v, invRotMat);
            
            getDotBilinear(img, u, v, rotImg.at<cv::Vec3b>(vr, ur));
            //rotImg.at<cv::Vec3b>(vr, ur) = img.at<cv::Vec3b>((int)v, (int)u);
        }
    }
}

void Transform::getDotBilinear
(const cv::Mat &img, float u, float v, cv::Vec3b &dot) const
{
    // 天井関数と床関数を同時に使うと整数の座標でエラー
    int uf = (int) floorf(u);
    int uc = uf + 1;
    int vf = (int) floorf(v);
    int vc = vf + 1;
    
    float ulow = u - uf;
    float uup = uc - u;
    float vlow = v - vf;
    float vup = vc - v;

    if (uc == frameSize.width) uc = 0;
    if (vc == frameSize.height) vc = 0;
    
    dot = uup * vup * img.at<cv::Vec3b>(vf, uf)
        + uup * vlow * img.at<cv::Vec3b>(vc, uf)
        + ulow * vup * img.at<cv::Vec3b>(vf, uc)
        + ulow * vlow * img.at<cv::Vec3b>(vc, uc);
}

void Transform::rotateVerticalImg
(float chi, const cv::Mat &img, cv::Mat &rotImg) const
{
    // X軸周りの回転を表す行列で画像を回転
    
    cv::Mat rotMat = (cv::Mat_<float>(3, 3) <<
                      1.0,    0.0,       0.0,
                      0.0, cosf(chi), -sinf(chi),
                      0.0, sinf(chi), cosf(chi));

    //rotateImgWithRotMat(img, rotImg, rotMat);
    
    
    for (int ur=0; ur<frameSize.width; ur++) {
        for (int vr=0; vr<frameSize.height; vr++) {
            float u, v;
            
            rotateVerticalOrthDot(-chi, ur, vr, u, v);
            getDotBilinear(img, u, v, rotImg.at<cv::Vec3b>(vr, ur));
            //rotImg.at<cv::Vec3b>(vr, ur) = img.at<cv::Vec3b>((int)v, (int)u);
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

void Transform::rotateVerticalOrthDot
(float chi, float u, float v, float &ur, float &vr)
const
{
    float theta, phi;
    float thetar, phir;
    
    orth2ang(u, v, theta, phi);
    rotateVerticalAngDot(chi, theta, phi, thetar, phir);
    ang2orth(thetar, phir, ur, vr);
    
    if (ur < 0) ur = 0; if (frameSize.width-1 < ur) ur = frameSize.width-1;
    if (vr < 0) vr = 0; if (frameSize.height-1 < vr) vr = frameSize.height-1;
}

void Transform::rotateVerticalAngDot
(float chi, float theta, float phi, float &thetar, float &phir) const
{
    // 分母が0のときpi/2
    float trdeno = -sinf(chi)*sinf(phi) + cosf(chi)*cosf(theta)*cosf(phi);
    thetar = (trdeno==0.0)? M_PI/2.0: atanf(sinf(theta)*cosf(phi)/trdeno);
    
    // thetaの定義域はarctanの定義の定義域よりも広いので場合分け
    if (0 <= theta && thetar < 0) thetar += M_PI;
    if (theta < 0 && 0 <= thetar) thetar -= M_PI;
    
    // 誤差によりasinの定義域に厳密に収まらない場合あり
    float asinin = cosf(chi)*sinf(phi) + sinf(chi)*cosf(theta)*cosf(phi);
    phir = (asinin<-1)? -1.0*M_PI/2.0: (1<=asinin? M_PI/2.0: asinf(asinin));
}

double Transform::normalizeTheta(float rawTheta) const
{
    while (rawTheta < -1.0 * M_PI) rawTheta += 2.0*M_PI;
    while (M_PI <= rawTheta)       rawTheta -= 2.0*M_PI;
    
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

