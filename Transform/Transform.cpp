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

void Transform::polar2equirect
(const std::vector<cv::Point2f> &polars,
 std::vector<cv::Point2f> &equirects) const
{
    points2points
    <cv::Point2f, cv::Point2f,
    &Transform::polar2equirect>(polars, equirects);
}

void Transform::equirect2polar
(const std::vector<cv::Point2f> &equirects,
 std::vector<cv::Point2f> &polars) const
{
    points2points
    <cv::Point2f, cv::Point2f,
    &Transform::equirect2polar>(equirects, polars);
}

void Transform::polar2sphere
(const std::vector<cv::Point2f> &polars,
 std::vector<cv::Point3f> &spheres) const
{
    points2points
    <cv::Point2f, cv::Point3f,
    &Transform::polar2sphere>(polars, spheres);
}

void Transform::sphere2psphere
(const std::vector<cv::Point3f> &spheres,
 std::vector<cv::Point2f> &polars) const
{
    points2points
    <cv::Point3f, cv::Point2f,
    &Transform::sphere2polar>(spheres, polars);
}

void Transform::equirect2sphere
(const std::vector<cv::Point2f> &equirects,
 std::vector<cv::Point3f> &spheres) const
{
    points2points
    <cv::Point2f, cv::Point3f,
    &Transform::equirect2sphere>(equirects, spheres);
}

void Transform::sphere2equirect
(const std::vector<cv::Point3f> &spheres,
 std::vector<cv::Point2f> &equirects) const
{
    points2points
    <cv::Point3f, cv::Point2f,
    &Transform::sphere2equirect>(spheres, equirects);
}

void Transform::sphere2normal
(const std::vector<cv::Point3f> &spheres,
 std::vector<cv::Point2f> &normals) const
{
    points2points
    <cv::Point3f, cv::Point2f,
    &Transform::sphere2normal>(spheres, normals);
}

template
<class forTp, class latTp, void (Transform::*func)(const forTp&, latTp&) const>
void Transform::points2points
(const std::vector<forTp>& forPoints, std::vector<latTp>& latPoints) const
{
    latPoints.clear();
    
    for (int i = 0; i < forPoints.size(); i++) {
        latTp latPoint;
        (this->*func)(forPoints[i], latPoint);
        latPoints.push_back(latPoint);
    }
}

void Transform::rotateSphere
(const std::vector<cv::Point3f> &spheres,
 std::vector<cv::Point3f> &spheresRot, const cv::Mat &rotMat) const
{
    for (int i = 0; i < spheres.size(); i++) {
        cv::Point3f sphereRot;
        rotateSphere(spheres[i], sphereRot, rotMat);
        spheresRot.push_back(sphereRot);
    }
}

void Transform::rotateImg
(const cv::Mat &img, cv::Mat &rotImg, const cv::Mat &rotMat) const
{
    rotImg = cv::Mat(img.size(), img.type());
    
    // 逆行列を用意
    cv::Mat invRotMat = rotMat.inv();
    
    for (int ur = 0; ur < frameSize.width; ur++) {
        for (int vr = 0; vr < frameSize.height; vr++) {
            cv::Point2f equirectRot(ur, vr), equirect;
            rotateEquirect(equirectRot, equirect, invRotMat);
            
            cv::Vec3b pixel;
            getBilinearPixel<cv::Vec3b>(img, equirect, pixel);
            rotImg.at<cv::Vec3b>(vr, ur) = pixel;
        }
    }
}

template<class T>
void Transform::getBilinearPixel
(const cv::Mat &img, const cv::Point2f &equirect, T &pixel) const
{
    float u = equirect.x, v = equirect.y;
    
    // 天井関数と床関数を同時に使うと整数の座標でエラー
    int uf = (int) floorf(u), uc = uf + 1;
    int vf = (int) floorf(v), vc = vf + 1;
    
    float ulow = u - uf, uup = uc - u;
    float vlow = v - vf, vup = vc - v;
    
    if (uc == frameSize.width) uc = 0;
    if (vc == frameSize.height) vc = 0;
    
    pixel =
        uup * vup * img.at<T>(vf, uf) + uup * vlow * img.at<T>(vc, uf) +
        ulow * vup * img.at<T>(vf, uc) + ulow * vlow * img.at<T>(vc, uc);
}
 
void Transform::rotateImgVertRect
(const float angle, const cv::Mat &img, const cv::Rect& rect,
 cv::Mat &rotImg) const
{
    rotImg = cv::Mat(img.size(), img.type());
    
    for (int ur = rect.x; ur < rect.x + rect.width; ur++) {
        for (int vr = rect.y; vr < rect.y + rect.height; vr++) {
            cv::Point2f equirectRot(ur, vr), equirect;
            rotateEquirectVert(-angle, equirectRot, equirect);
            
            uchar pixel;
            getBilinearPixel<uchar>(img, equirect, pixel);
            rotImg.at<uchar>(vr, ur) = pixel;
        }
    }
    
    /*
    int halfWidth = frameSize.width / 2;
    // 幅が偶数の場合の幅中心の画素の補間
    if (frameSize.width % 2 == 0) {
        for (int vr=rect.y; vr<rect.y + rect.height; vr++) {
            rotImg.at<cv::Vec3b>(vr, halfWidth-1) =
                2.0/3.0 * rotImg.at<cv::Vec3b>(vr, halfWidth-2) +
                1.0/3.0 * rotImg.at<cv::Vec3b>(vr, halfWidth+1);
            rotImg.at<cv::Vec3b>(vr, halfWidth) =
                1.0/3.0 * rotImg.at<cv::Vec3b>(vr, halfWidth-2) +
                2.0/3.0 * rotImg.at<cv::Vec3b>(vr, halfWidth+1);
        }
    // 幅が奇数の場合の幅中心の画素の補間
    } else {
        for (int vr=rect.y; vr<rect.y + rect.height; vr++) {
            rotImg.at<cv::Vec3b>(vr, halfWidth) =
            1.0/2.0 * rotImg.at<cv::Vec3b>(vr, halfWidth-1) +
            1.0/2.0 * rotImg.at<cv::Vec3b>(vr, halfWidth+1);
        }
    }
    */
}

/*
void Transform::rotateEquirectVert
(const float angle, const cv::Point2f &equirect,
 cv::Point2f &equirectRot) const
{
    cv::Point2f forpsphere, latpsphere;
    equirect2psphere(equirect, forpsphere);
    rotatePsphereVert(angle, forpsphere, latpsphere);
    psphere2equirect(latpsphere, equirectRot);
}
*/
/*
void Transform::rotatePsphereVert
(const float angle, const cv::Point2f &psphere,
 cv::Point2f &psphereRot) const
{
    float theta = psphere.x;
    float phi = psphere.y;
    float thetar, phir;
    
    // 分母が0のときπ/2
    float trdeno = -sinf(angle)*sinf(phi) + cosf(angle)*cosf(theta)*cosf(phi);
    thetar = (trdeno==0.0)? M_PI/2.0: atanf(sinf(theta)*cosf(phi)/trdeno);
    
    // thetaの定義域はarctanの定義域よりも広いので場合分け
    if (0 <= theta && thetar < 0) thetar += M_PI;
    if (theta < 0 && 0 <= thetar) thetar -= M_PI;
    
    // 誤差によりasinの定義域に厳密に収まらない場合あり
    float asinin = cosf(angle)*sinf(phi) + sinf(angle)*cosf(theta)*cosf(phi);
    phir = (asinin<-1)? -1.0*M_PI/2.0: (1<=asinin? M_PI/2.0: asinf(asinin));
    
    psphereRot.x = thetar;
    psphereRot.y = phir;
}
*/
