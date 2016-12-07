//
//  Transform.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/01/10.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Transform.hpp"

Transform::Transform(const cv::Size& fs): fs(fs)
{
}

void Transform::polar2equirect
(const std::vector<Polar> &polars,
 std::vector<Equirect> &equirects) const
{
    points2points
    <Polar, Equirect, &Transform::polar2equirect>(polars, equirects);
}

void Transform::equirect2polar
(const std::vector<Equirect> &equirects,
 std::vector<Polar> &polars) const
{
    points2points
    <Equirect, Polar, &Transform::equirect2polar>(equirects, polars);
}

void Transform::polar2sphere
(const std::vector<Polar> &polars,
 std::vector<Sphere> &spheres) const
{
    points2points
    <Polar, Sphere, &Transform::polar2sphere>(polars, spheres);
}

void Transform::sphere2polar
(const std::vector<Sphere> &spheres,
 std::vector<Polar> &polars) const
{
    points2points
    <Sphere, Polar, &Transform::sphere2polar>(spheres, polars);
}

void Transform::equirect2sphere
(const std::vector<Equirect> &equirects,
 std::vector<Sphere> &spheres) const
{
    points2points
    <Equirect, Sphere,
    &Transform::equirect2sphere>(equirects, spheres);
}

void Transform::sphere2equirect
(const std::vector<Sphere> &spheres,
 std::vector<Equirect> &equirects) const
{
    points2points
    <Sphere, Equirect,
    &Transform::sphere2equirect>(spheres, equirects);
}

void Transform::sphere2normal
(const std::vector<Sphere> &spheres,
 std::vector<Normal> &normals) const
{
    points2points
    <Sphere, Normal, &Transform::sphere2normal>(spheres, normals);
}

void Transform::pers2normal
(const std::vector<Pers> &perss, std::vector<Normal> &normals,
 const cv::Mat& inParaMat) const
{
    points2points
    <Pers, Normal, &Transform::pers2normal>(perss, normals, inParaMat);
}

void Transform::normal2pers
(const std::vector<Normal> &normals, std::vector<Pers> &perss,
 const cv::Mat &inParaMat) const
{
    points2points
    <Normal, Pers, &Transform::normal2pers>(normals, perss, inParaMat);
}

template
<class forTp, class latTp, latTp (Transform::*func)(const forTp&) const>
void Transform::points2points
(const std::vector<forTp>& forPoints, std::vector<latTp>& latPoints) const
{
    latPoints = std::vector<latTp>(forPoints.size());
    
    for (int i = 0; i < forPoints.size(); i++) {
        latPoints[i] = (this->*func)(forPoints[i]);
    }
}

template
<class forTp, class latTp,
latTp (Transform::*func)(const forTp&, const cv::Mat&) const>
void Transform::points2points
(const std::vector<forTp>& forPoints, std::vector<latTp>& latPoints,
 const cv::Mat& mat) const
{
    latPoints = std::vector<latTp>(forPoints.size());
    
    for (int i = 0; i < forPoints.size(); i++) {
        latPoints[i] = (this->*func)(forPoints[i], mat);
    }
}


void Transform::rotateSphere
(const std::vector<Sphere> &spheres,
 std::vector<Sphere> &spheresRot, const cv::Mat &rotMat) const
{
    points2points
    <Sphere, Sphere, &Transform::rotateSphere>(spheres, spheresRot, rotMat);
}

void Transform::rotateImg
(const cv::Mat &img, cv::Mat &rotImg, const cv::Mat &rotMat) const
{
    rotImg = cv::Mat(img.size(), img.type());
    
    std::cout << rotMat << std::endl;
    
    // 逆行列を用意
    cv::Mat invRotMat = rotMat.inv();
    
    for (int vr = 0; vr < fs.height; vr++) {
        cv::Vec3b* row = rotImg.ptr<cv::Vec3b>(vr);
        
        for (int ur = 0; ur < fs.width; ur++) {
            Equirect equirect = rotateEquirect(Equirect(ur, vr), invRotMat);
            
            row[ur] = getBiliPixel<cv::Vec3b>(img, equirect);
        }
    }
}

/*
template<class Tp>
Tp Transform::getBiliPixel(const cv::Mat &img, const Equirect &equirect) const
{
    float u = equirect.x, v = equirect.y;
    
    // 天井関数と床関数を同時に使うと整数の座標でエラー
    int uf = (int) floorf(u), uc = uf + 1;
    int vf = (int) floorf(v), vc = vf + 1;
    
    float ulow = u - uf, uup = uc - u;
    float vlow = v - vf, vup = vc - v;
    
    if (uc == fs.width) uc = 0;
    if (vc == fs.height) vc = 0;
    
    return uup * vup * img.at<Tp>(vf, uf) + uup * vlow * img.at<Tp>(vc, uf) +
           ulow * vup * img.at<Tp>(vf, uc) + ulow * vlow * img.at<Tp>(vc, uc);
}*/

void Transform::rotateImgVertRect
(const float angle, const cv::Mat &img, const cv::Rect& rect,
 cv::Mat &rotImg) const
{
    rotImg = cv::Mat(img.size(), img.type());
    
    /*
    for (int ur = rect.x; ur < rect.x + rect.width; ur++) {
        for (int vr = rect.y; vr < rect.y + rect.height; vr++) {
            Equirect equirect = rotateEquirectVert(-angle, Equirect(ur,vr));
            
            rotImg.at<uchar>(vr, ur) = getBiliPixel<uchar>(img, equirect);
        }
    }*/

    const float vheight = rect.y + rect.height;
    const float vwidth = rect.x + rect.width;
    
    for (int vr = rect.y; vr < vheight ; vr++) {
        uchar* row = rotImg.ptr<uchar>(vr);
        
        for (int ur = rect.x; ur < vwidth; ur++) {
            Equirect equirect = rotateEquirectVert(-angle, Equirect(ur,vr));
            row[ur] = getBiliPixel<uchar>(img, equirect);
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

void Transform::normal2point
(const std::vector<Normal> &normals, std::vector<cv::Point2f> &points)
{
    points = std::vector<cv::Point2f>(normals.size());
    
    for (int i = 0; i < normals.size(); i++) {
        points[i] = normals[i];
    }
}

void Transform::point2normal
(const std::vector<cv::Point2f> &points, std::vector<Normal> &normals)
{
    normals = std::vector<Normal>(points.size());
    
    for (int i = 0; i < points.size(); i++) {
        normals[i] = Normal(points[i]);
    }
}

void Transform::changeChannel(const cv::Mat &img, cv::Mat &outImg)
{
    assert(img.channels() == 1);
    
    outImg = cv::Mat(img.size(), CV_8UC3);
    
    for (int v = 0; v < img.rows; v++) {
        const uchar* inRow = img.ptr<uchar>(v);
        cv::Vec3b* outRow = outImg.ptr<cv::Vec3b>(v);
        
        for (int u = 0; u < img.cols; u++) {
            outRow[u] = cv::Vec3b(inRow[u], inRow[u], inRow[u]);
        }
    }
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
