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
        if (value < dlim) return dlim;
        else if (ulim < value) return ulim;
        else return value;
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
        float u = polar.x*fs.width/(2.0*M_PI) + fs.width/2.0;
        float v = -polar.y*fs.height/M_PI + fs.height/2.0;
        return Equirect(revValueDom(u, 0.0, fs.width-1),
                        revValueDom(v, 0.0, fs.height-1));
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
        float theta = (equirect.x-fs.width/2.0) * 2.0*M_PI/fs.width;
        float phi = - (equirect.y-fs.height/2.0) * M_PI/fs.height;
        return Polar(revValueDom(theta, -M_PI, M_PI),
                     revValueDom(phi, -M_PI_2, M_PI_2));
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
    Sphere polar2Sphere(const Polar& polar) const
    {
        float x = sinf(polar.x) * cosf(polar.y);
        float y = - sinf(polar.y);
        float z = cosf(polar.x) * cosf(polar.y);
        // 補正の必要はあるか
        return Sphere(x, y, z);
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
        float theta = (sphere.z!=0)? atanf(sphere.x/sphere.z):
                                     ((sphere.x>0)? M_PI_2:
                                                    -M_PI_2);
        if (sphere.z<0) { if (sphere.x < 0) theta -= M_PI;
                          else theta += M_PI; }
        float phi = asinf(revValueDom(sphere.y, -1, 1));
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
        return polar2Sphere(equirect2polar(equirect));
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
    <class forTp, class latTp,
    void (Transform::*func)(const forTp&, latTp&) const>
    void points2points
    (const std::vector<forTp>& forPoints, std::vector<latTp>& latPoints) const;
    
    // 計算誤差による定義域外の値を補正
    static void correctDomain(float& rawValue, float dlimit, float ulimit) {
        if (rawValue < dlimit) rawValue = dlimit;
        else if (ulimit < rawValue) rawValue = ulimit;
    }
    void correctequirect(Equirect& equirect) const {
        correctDomain(equirect.x, 0.0, fs.width-1);
        correctDomain(equirect.y, 0.0, fs.height-1);
    }
    static void correctpolar(Polar& polar) {
        correctDomain(polar.x, -M_PI, M_PI);
        correctDomain(polar.y, -M_PI/2.0, M_PI/2.0);
    }
    static void correctsphere(Sphere& sphere) {
        correctDomain(sphere.x, -1, 1);
        correctDomain(sphere.y, -1, 1);
        correctDomain(sphere.z, -1, 1);
    }
    
    // 回転行列で画像座標を回転
    void rotateEquirect
    (const Equirect& equirect, Equirect& equirectRot,
     const cv::Mat& rotMat) const {
        Sphere sphere, sphereRot;
        equirect2sphere(equirect, sphere);
        rotateSphere(sphere, sphereRot, rotMat);
        sphere2equirect(sphereRot, equirectRot);
    }
    // 回転行列で球面座標を回転
    void rotateSphere
    (const Sphere& sphere, Sphere& sphereRot, const cv::Mat& rotMat) const {
        const float* row0 = rotMat.ptr<float>(0);
        const float* row1 = rotMat.ptr<float>(1);
        const float* row2 = rotMat.ptr<float>(2);

        sphereRot.x = row0[0]*sphere.x+row0[1]*sphere.y+row0[2]*sphere.z;
        sphereRot.y = row1[0]*sphere.x+row1[1]*sphere.y+row1[2]*sphere.z;
        sphereRot.z = row2[0]*sphere.x+row2[1]*sphere.y+row2[2]*sphere.z;
    }
    
    void rotateSphere
    (const std::vector<Sphere>& spheres, std::vector<Sphere>& spheresRot,
     const cv::Mat& rotMat) const;
    
    // 回転行列で画像を変換（逆向きの回転行列を使うことに注意）
    void rotateImg
    (const cv::Mat& img, cv::Mat& rotImg, const cv::Mat& rotMat) const;
    
    // バイリニア補間による画素値の決定
    template<class T>
    void getBilinearPixel
    (const cv::Mat& img, const cv::Point2f& equirect, T& pixel) const;
    
    // 画像をX軸まわりに回転（グレースケール）
    void rotateImgVertRect
    (const float angle, const cv::Mat& img,
     const cv::Rect& rect, cv::Mat& rotImg)  const;
    // 画像座標をX軸まわりに回転
    void rotateEquirectVert
    (const float angle, const Equirect& equirect,
     Equirect& equirectRot) const {
        Sphere sphere, sphereRot;
        equirect2sphere(equirect, sphere);
        rotateSphereVert(angle, sphere, sphereRot);
        sphere2equirect(sphereRot, equirectRot);
    }
    // 球面座標をX軸まわりに回転
    void rotateSphereVert
    (const float angle, const cv::Point3f& sphere,
     cv::Point3f& sphereRot) const {
        float cosa = cosf(angle), sina = sinf(angle);
        sphereRot.x = sphere.x;
        sphereRot.y = cosa*sphere.y - sina*sphere.z;
        sphereRot.z = sina*sphere.y + cosa*sphere.z;
    }
    
    /*
    void rotatePsphereVert
    (const float angle, const cv::Point2f& psphere,
     cv::Point2f& psphereRot) const;
    */

    static void normal2point
    (const std::vector<Normal>& normals, std::vector<cv::Point2f>& points);
    static void point2normal
    (const std::vector<cv::Point2f>& points, std::vector<Normal>& normals);
    
    
private:
    const cv::Size& fs;
};
#endif /* Transform_hpp */
