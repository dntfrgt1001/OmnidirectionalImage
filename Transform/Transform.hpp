//
//  Transform.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/01/10.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Transform_hpp
#define Transform_hpp

#include <stdio.h>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "Core.hpp"

//#include <libiomp/omp.h>

class Transform{
public:
    Transform(const cv::Size& frameSize);
    
    
    // 画像の大きさを変換
    void resizeImg(const cv::Mat& img, cv::Mat& resImg) const {
        cv::resize(img, resImg, fs);
    }
    
    static float revValueDom
    (const float value, const float dlim, const float ulim) {
        return (value < dlim) ? dlim : (ulim < value) ? ulim: value;
    }

    // 画像座標->球面極座標
    /*
    void polar2equirect
    (const Polar& polar, Equirect& equirect) const {
        equirect.x = polar.x*fs.width/(2.0*M_PI) + fs.width/2.0;
        equirect.y = -polar.y*fs.height/M_PI + fs.height/2.0;
        correctequirect(equirect);
    }
     */
    Equirect polar2equirect(const Polar& polar) const
    {
        return Equirect(revValueDom
                        (polar.x*fs.width/(2.0*M_PI) + fs.width/2.0,
                        0.0, fs.width - 1),
                        revValueDom
                        (-polar.y*fs.height/M_PI + fs.height/2.0,
                         0.0, fs.height - 1));
    }
    void polar2equirect
    (const std::vector<Polar>& polars,
     std::vector<Equirect>& equirects) const;
    
    // 球面極座標->画像座標
    /*
    void equirect2polar
    (const Equirect& equirect, Polar& polar) const {
        polar.x = (equirect.x-fs.width/2.0) *
                    2.0*M_PI/fs.width;
        polar.y = - (equirect.y-fs.height/2.0) *
                    M_PI/fs.height;
        correctpolar(polar);
    }
     */
    Polar equirect2polar(const Equirect& equirect) const
    {
        return Polar(revValueDom
                     ((equirect.x-fs.width/2.0) * 2.0*M_PI/fs.width,
                      -M_PI, M_PI),
                     revValueDom
                     (- (equirect.y-fs.height/2.0) * M_PI/fs.height,
                      -M_PI_2, M_PI_2));
    }
    void equirect2polar
    (const std::vector<Equirect>& equirects,
     std::vector<Polar>& polars) const;
    
    // 画像座標と極座標の長さの変換
    float dtheta2u(float theta) const {
        return theta * fs.width / (2.0 * M_PI);
    }
    float dphi2v(float phi) const {
        return phi * fs.height / M_PI;
    }
    
    // 球面極座標->球面座標
    /*
    void polar2sphere
    (const Polar& polar, Sphere& sphere) const {
        sphere.x = sinf(polar.x) * cosf(polar.y);
        sphere.y = - sinf(polar.y);
        sphere.z = cosf(polar.x) * cosf(polar.y);
        correctsphere(sphere);
    }*/
    Sphere polar2sphere(const Polar& polar) const
    {
        return Sphere(sinf(polar.x) * cosf(polar.y),
                      - sinf(polar.y),
                      cosf(polar.x) * cosf(polar.y));
    }
    void polar2sphere
    (const std::vector<Polar>& polars,
     std::vector<Sphere>& spheres) const;
    
    // 球面座標->球面極座標
    /*
    void sphere2polar
    (const Sphere& sphere, Polar& polar) const {
        float theta = (sphere.z!=0)? atanf(sphere.x/sphere.z):
                      ((sphere.x>0)? M_PI/2.0: -M_PI/2.0);
        if (sphere.z<0) { if (sphere.x < 0) theta -= M_PI; else theta += M_PI; }
        float phi = sphere.y<-1.0? asinf(-1.0):
                    (1.0<sphere.y? asinf(1.0): asinf(-sphere.y));
        polar.x = theta; polar.y = phi;
        correctpolar(polar);
    }
     */
    Polar sphere2polar(const Sphere& sphere) const
    {
        float den = sqrtf(sphere.x* sphere.x + sphere.z*sphere.z);
        float theta = (den==0)? 0: acosf(revValueDom(sphere.z/den,-1,1)) *
                                   (sphere.x > 0? 1: -1);

        float phi = asinf(revValueDom(-sphere.y, -1, 1));
        return Polar(revValueDom(theta, -M_PI, M_PI),
                     revValueDom(phi, -M_PI_2, M_PI_2));
    }
    void sphere2polar
    (const std::vector<Sphere>& spheres,
     std::vector<Polar>& pspheres) const;
    
    // 画像座標->球面座標
    /*
    void equirect2sphere
    (const Equirect& equirect, Sphere& sphere) const {
        Polar polar;
        equirect2polar(equirect, polar);
        polar2sphere(polar, sphere);
    }*/
    Sphere equirect2sphere(const Equirect& equirect) const
    {
        return polar2sphere(equirect2polar(equirect));
    }
    void equirect2sphere
    (const std::vector<Equirect>& equirects,
     std::vector<Sphere>& spheres) const;
    
    // 球面座標->画像座標
    /*
    void sphere2equirect
    (const Sphere& sphere, Equirect& equirect) const {
        Polar polar;
        sphere2polar(sphere, polar);
        polar2equirect(polar, equirect);
    }*/
    Equirect sphere2equirect(const Sphere& sphere) const
    {
        return polar2equirect(sphere2polar(sphere));
    }
    void sphere2equirect
    (const std::vector<Sphere>& spheres,
     std::vector<Equirect>& equirects) const;
    
    // 球面座標->正規化画像座標
    /*
    void sphere2normal
    (const Sphere& sphere, Normal& normal) const {
        normal.x = sphere.x / sphere.z;
        normal.y = sphere.y / sphere.z;
    }*/
    Normal sphere2normal(const Sphere& sphere) const
    {
        return Normal(sphere.x/sphere.z, sphere.y / sphere.z);
    }
    void sphere2normal
    (const std::vector<Sphere>& spheres,
     std::vector<Normal>& normals) const;
    
    // 正規化画像座標->透視投影画像座標
    /*
    void normal2pers
    (const Normal& normal, Pers& pers, const cv::Mat& inParaMat) const {
        pers.x = inParaMat.at<float>(0,0)*normal.x +
                 inParaMat.at<float>(0,2);
        pers.y = inParaMat.at<float>(1,1)*normal.y +
                 inParaMat.at<float>(1,2);
    }*/
    Pers normal2pers(const Normal& normal, const cv::Mat& inParaMat) const
    {
        return Pers(inParaMat.at<float>(0,0)*normal.x +
                    inParaMat.at<float>(0,2),
                    inParaMat.at<float>(1,1)*normal.y +
                    inParaMat.at<float>(1,2));
    }
    void normal2pers
    (const std::vector<Normal>& normals, std::vector<Pers>& perss,
     const cv::Mat& inParaMat) const;
    
    // 透視投影画像座標->正規化画像座標
    /*
    void pers2normal
    (const Pers& pers, Normal& normal, const cv::Mat& inParaMat) const {
        normal.x = (pers.x - inParaMat.at<float>(0,2)) /
                    inParaMat.at<float>(0,0);
        normal.y = (pers.y - inParaMat.at<float>(1,2)) /
                    inParaMat.at<float>(1,1);
    }*/
    Normal pers2normal(const Pers& pers, const cv::Mat& inParaMat) const
    {
        return Normal((pers.x - inParaMat.at<float>(0,2)) /
                      inParaMat.at<float>(0,0),
                      (pers.y - inParaMat.at<float>(1,2)) /
                      inParaMat.at<float>(1,1));
    }
    void pers2normal
    (const std::vector<Pers>& perss, std::vector<Normal>& normals,
     const cv::Mat& inParaMat) const;
    
    // 正規化画像座標->球面座標
    /*
    void normal2sphere
    (const Normal& normal, Sphere& sphere, const bool isFront) const {
        float denomi = sqrtf(normal.x*normal.x + normal.y*normal.y + 1.0);
        sphere.x = normal.x / denomi;
        sphere.y = normal.y / denomi;
        sphere.z = 1 / denomi;
        
        if (!isFront) sphere *= -1;
    }*/
    Sphere normal2sphere(const Normal& normal, const bool isFront) const
    {
        float denomi = sqrtf(normal.x*normal.x + normal.y*normal.y + 1.0);
        float x = normal.x / denomi;
        float y = normal.y / denomi;
        float z = 1 / denomi;
        return isFront? Sphere(x, y, z): Sphere(-x, -y, -z);
    }
    
    // 点群変換のテンプレート
    template
    <class forTp, class latTp, latTp (Transform::*func)(const forTp&) const>
    void points2points
    (const std::vector<forTp>& forPoints, std::vector<latTp>& latPoints) const;
    
    template
    <class forTp, class latTp,
    latTp (Transform::*func)(const forTp&, const cv::Mat&) const>
    void points2points
    (const std::vector<forTp>& forPoints, std::vector<latTp>& latPoints,
     const cv::Mat& mat) const;
    
    // 回転行列で画像座標を回転
    Equirect rotateEquirect
    (const Equirect& equirect, const cv::Mat& rotMat) const
    {
        return sphere2equirect(rotateSphere(equirect2sphere(equirect),
                                            rotMat));
    }
    
    // 回転行列で球面座標を回転
    Sphere rotateSphere(const Sphere& sphere, const cv::Mat& rotMat) const
    {
        const float* row0 = rotMat.ptr<float>(0);
        const float* row1 = rotMat.ptr<float>(1);
        const float* row2 = rotMat.ptr<float>(2);
        
        return Sphere(row0[0]*sphere.x+row0[1]*sphere.y+row0[2]*sphere.z,
                      row1[0]*sphere.x+row1[1]*sphere.y+row1[2]*sphere.z,
                      row2[0]*sphere.x+row2[1]*sphere.y+row2[2]*sphere.z);
    }
    
    void rotateSphere
    (const std::vector<Sphere>& spheres, std::vector<Sphere>& spheresRot,
     const cv::Mat& rotMat) const;
    
    // 回転行列で画像を変換（逆向きの回転行列を使うことに注意）
    void rotateImg
    (const cv::Mat& img, cv::Mat& rotImg, const cv::Mat& rotMat) const;
    
    // バイリニア補間による画素値の決定
    template<class Tp>
    Tp getBiliPixel(const cv::Mat& img, const Equirect& equirect) const;
    
    // 画像をX軸まわりに回転（グレースケール）
    void rotateImgVertRect
    (const cv::Mat& img, const float angle,
     const cv::Rect& rect, cv::Mat& rotImg) const;
    
    // 画像座標をX軸まわりに回転
    Equirect rotateEquirectVert
    (const Equirect& equirect, const float angle) const
    {
        return sphere2equirect(rotateSphereVert
                               (equirect2sphere(equirect), angle));
    }
    // 球面座標をX軸まわりに回転
    Sphere rotateSphereVert(const Sphere& sphere, const float angle) const
    {
        float cosa = cosf(angle), sina = sinf(angle);
        return Sphere(sphere.x,
                      cosa*sphere.y - sina*sphere.z,
                      sina*sphere.y + cosa*sphere.z);
    }
    
    static void normal2point
    (const std::vector<Normal>& normals, std::vector<cv::Point2f>& points);
    static void point2normal
    (const std::vector<cv::Point2f>& points, std::vector<Normal>& normals);
    
    // チャンネル数の変換
    static void changeChannel(const cv::Mat& img, cv::Mat& outImg);
    
//private:
    const cv::Size& fs;
};

template<class Tp>
inline Tp Transform::getBiliPixel
(const cv::Mat &img, const Equirect &equirect) const
{
    int uf = floor(equirect.x), uc = uf + 1;
    int vf = floor(equirect.y), vc = vf + 1;
    
    float ulow = equirect.x - uf, uup = uc - equirect.x;
    float vlow = equirect.y - vf, vup = vc - equirect.y;
    
    if (uc == fs.width) uc = 0;
    if (vc == fs.height) vc = 0;
    
    return uup * vup * (img.ptr<Tp>(vf))[uf] +
           uup * vlow * (img.ptr<Tp>(vc))[uf] +
           ulow * vup * (img.ptr<Tp>(vf))[uc] +
           ulow * vlow * (img.ptr<Tp>(vc))[uc];
}

#endif /* Transform_hpp */
