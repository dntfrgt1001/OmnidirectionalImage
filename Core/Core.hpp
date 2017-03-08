//
//  Core.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Core_hpp
#define Core_hpp

#include <stdio.h>

#include <opencv2/opencv.hpp>

class Equirect : public cv::Point2f
{
public:
    Equirect(): cv::Point2f() {};
    Equirect(float u, float v): cv::Point2f(u, v){};
    Equirect(const cv::Point2f& point2f): cv::Point2f(point2f){};
};

class Polar : public cv::Point2f
{
public:
    Polar(): cv::Point2f() {};
    Polar(float theta, float phi): cv::Point2f(theta, phi){};
    Polar(const cv::Point2f& point2f): cv::Point2f(point2f){};
};

class Sphere : public cv::Point3f
{
public:
    Sphere(): cv::Point3f() {};
    Sphere(float x, float y, float z): cv::Point3f(x, y, z){};
    Sphere(const cv::Point3f& point3f): cv::Point3f(point3f){};
};

class Normal : public cv::Point2f
{
public:
    Normal(): cv::Point2f() {};
    Normal(float xn, float yn): cv::Point2f(xn, yn){};
    Normal(const cv::Point2f& point2f): cv::Point2f(point2f) {};
};

class Pers : public cv::Point2f
{
public:
    Pers(): cv::Point2f() {};
    Pers(float xp, float yp): cv::Point2f(xp, yp){};
    Pers(const cv::Point2f& point2f): cv::Point2f(point2f){};
};


class Map {
public:
    Map();
    
    // 画像をリサイズ
    static void resizeImg(const cv::Mat& img, cv::Mat& resImg) {
        cv::resize(img, resImg, fs);
    }
    
    // それぞれの座標系の定義域に正規化
    static float normalDomain
    (const float value, const float lower, const float upper) {
        return (value < lower)? lower: (upper < value)? upper: value;
    }
    
    // *****************************************************************
    // 経緯度 → 画像座標
    static Equirect polar2equirect(const Polar& polar)
    {
        return Equirect(normalDomain
                        (polar.x*fs.width/(2.0*M_PI) + fs.width/2.0,
                         0.0, fs.width - 1),
                        normalDomain
                        (-polar.y*fs.height/M_PI + fs.height/2.0,
                         0.0, fs.height - 1));
    }
    static void polar2equirect
    (const std::vector<Polar>& polars, std::vector<Equirect>& equirects);
    
    // 画像座標 → 経緯度
    static Polar equirect2polar(const Equirect& equirect)
    {
        return Polar(normalDomain
                     ((equirect.x-fs.width/2.0) * 2.0*M_PI/fs.width,
                      -M_PI, M_PI),
                     normalDomain
                     (- (equirect.y-fs.height/2.0) * M_PI/fs.height,
                      -M_PI_2, M_PI_2));
    }
    static void equirect2polar
    (const std::vector<Equirect>& equirects, std::vector<Polar>& polars);

    // 長さの変換
    static float dtheta2u(const float theta) {
        return theta * fs.width / (2.0 * M_PI);
    }
    static float dphi2v(const float phi) {
        return phi * fs.height / M_PI;
    }
    
    // *****************************************************************
    // 経緯度 → カメラ座標(球面上)
    static Sphere polar2sphere(const Polar& polar)
    {
        return Sphere(sinf(polar.x) * cosf(polar.y),
                      - sinf(polar.y),
                      cosf(polar.x) * cosf(polar.y));
    }
    static void polar2sphere
    (const std::vector<Polar>& polars, std::vector<Sphere>& spheres);
    
    // カメラ座標(球面上) → 経緯度
    static Polar sphere2polar(const Sphere& sphere)
    {
//        float den = sqrtf(sphere.x* sphere.x + sphere.z*sphere.z);
//        float theta = (den==0)? 0: acosf(normalDomain(sphere.z/den,-1,1)) *
//                                   (sphere.x > 0? 1: -1);

        float theta = atan2f(sphere.x, sphere.z);
        float phi = asinf(normalDomain(-sphere.y, -1, 1));
        
        return Polar(theta, phi);
    }
    static void sphere2polar
    (const std::vector<Sphere>& spheres, std::vector<Polar>& polars);
    
    // *****************************************************************
    // 画像座標 → カメラ座標(球面上)
    static Sphere equirect2sphere(const Equirect& equirect)
    {
        return polar2sphere(equirect2polar(equirect));
    }
    static void equirect2sphere
    (const std::vector<Equirect>& equirects, std::vector<Sphere>& spheres);
    
    // カメラ座標(球面上) → 画像座標
    static Equirect sphere2equirect(const Sphere& sphere)
    {
        return polar2equirect(sphere2polar(sphere));
    }
    static void sphere2equirect
    (const std::vector<Sphere>& spheres,
     std::vector<Equirect>& equirects);
    
    // *****************************************************************
    // カメラ座標(球面上) → 正規化画像座標
    static Normal sphere2normal(const Sphere& sphere)
    {
        return Normal(sphere.x/sphere.z, sphere.y / sphere.z);
    }
    static void sphere2normal
    (const std::vector<Sphere>& spheres,
     std::vector<Normal>& normals);
    
    // 正規化画像座標 → カメラ座標(球面上)
    static Sphere normal2sphere(const Normal& normal, const bool isFront)
    {
        float denomi = sqrtf(normal.x*normal.x + normal.y*normal.y + 1.0);
        float x = normal.x / denomi;
        float y = normal.y / denomi;
        float z = 1 / denomi;
        return isFront? Sphere(x, y, z): Sphere(-x, -y, -z);
    }
    
    // *****************************************************************
    // 正規化画像座標 → 透視投影画像座標
    static Pers normal2pers(const Normal& normal, const cv::Mat& inParaMat)
    {
        return Pers(inParaMat.at<float>(0,0)*normal.x +
                    inParaMat.at<float>(0,2),
                    inParaMat.at<float>(1,1)*normal.y +
                    inParaMat.at<float>(1,2));
    }
    static void normal2pers
    (const std::vector<Normal>& normals, std::vector<Pers>& perss,
     const cv::Mat& inParaMat);

    // 透視投影画像座標 → 正規化画像座標
    static Normal pers2normal(const Pers& pers, const cv::Mat& inParaMat)
    {
        return Normal((pers.x - inParaMat.at<float>(0,2)) /
                      inParaMat.at<float>(0,0),
                      (pers.y - inParaMat.at<float>(1,2)) /
                      inParaMat.at<float>(1,1));
    }
    static void pers2normal
    (const std::vector<Pers>& perss, std::vector<Normal>& normals,
     const cv::Mat& inParaMat);
    
    // *****************************************************************
    // OpenCVのクラス
    static cv::Point2f normal2point(const Normal& normal)
    {
        return normal;
    }
    static void normal2point
    (const std::vector<Normal>& normals, std::vector<cv::Point2f>& points);
    
    // *****************************************************************
    // 点群変換のテンプレート
    template
    <class forTp, class latTp, static latTp func(const forTp&)>
    static void points2points
    (const std::vector<forTp>& forPoints, std::vector<latTp>& latPoints);
    
    template
    <class forTp, class latTp, static latTp func(const forTp&, const cv::Mat&)>
    static void points2points
    (const std::vector<forTp>& forPoints, std::vector<latTp>& latPoints,
     const cv::Mat& mat);
    
    // *****************************************************************
    // 画像座標を回転
    static Equirect rotateEquirect
    (const Equirect& equirect, const cv::Mat& rotMat)
    {
        return sphere2equirect(rotateSphere(equirect2sphere(equirect),
                                            rotMat));
    }

    // カメラ座標を回転
    static Sphere rotateSphere(const Sphere& sphere, const cv::Mat& rotMat)
    {
        const float* row0 = rotMat.ptr<float>(0);
        const float* row1 = rotMat.ptr<float>(1);
        const float* row2 = rotMat.ptr<float>(2);
        
        return Sphere(row0[0]*sphere.x+row0[1]*sphere.y+row0[2]*sphere.z,
                      row1[0]*sphere.x+row1[1]*sphere.y+row1[2]*sphere.z,
                      row2[0]*sphere.x+row2[1]*sphere.y+row2[2]*sphere.z);
    }
    static void rotateSphere
    (const std::vector<Sphere>& spheres, std::vector<Sphere>& spheresRot,
     const cv::Mat& rotMat);
    
    // 画像を回転(テンプレート)
    template <class Dot>
    static void rotateImg
    (const cv::Mat& img, const cv::Mat& rotMat, cv::Mat& rotImg);
    // 画像を回転
    static void rotateImg
    (const cv::Mat& img, const cv::Mat& rotMat, cv::Mat& rotImg);
    
    
    // *****************************************************************
    // 画像座標をX軸まわりに回転
    static Equirect rotateEquirectVert
    (const Equirect& equirect, const float angle)
    {
        return sphere2equirect(rotateSphereVert
                               (equirect2sphere(equirect), angle));
    }

    // カメラ座標をX軸まわりに回転
    static Sphere rotateSphereVert(const Sphere& sphere, const float angle)
    {
        const float cosa = cosf(angle);
        const float sina = sinf(angle);
        return Sphere(sphere.x,
                      cosa*sphere.y - sina*sphere.z,
                      sina*sphere.y + cosa*sphere.z);
    }

    // 画像をX軸まわりに回転(テンプレート)
    template <class Dot>
    static void rotateImgVert
    (const cv::Mat& img, const float angle,
     const cv::Rect& rect, cv::Mat& rotImg);
    // 画像をX軸まわりに回転
    static void rotateImgVert
    (const cv::Mat& img, const float angle,
     const cv::Rect& rect, cv::Mat& rotImg);
    
    // *****************************************************************
    // バイリニア補間による画素値の決定
    template<class Dot>
    static Dot getBiliPixel(const cv::Mat& img, const cv::Point2f& point);
    
    // フレームサイズの切り替え
    static void setProcSize() { fs = fsProc; }
    static void setOutSize()  { fs = fsOut; }

    // フレームサイズ
    static cv::Size fs;      // 現在のフレームサイズ
    static cv::Size fsProc;  // 画像処理用のフレームサイズ
    static cv::Size fsOut;   // 出力用のフレームサイズ
};

template<class Dot>
inline Dot Map::getBiliPixel
(const cv::Mat &img, const cv::Point2f &point)
{
    int uf = floor(point.x), uc = uf + 1;
    int vf = floor(point.y), vc = vf + 1;
    
    double ulow = point.x - uf, uup = uc - point.x;
    double vlow = point.y - vf, vup = vc - point.y;
    
    if (uc == img.cols) uc = 0;
    if (vc == img.rows) vc = 0;
    
    return uup * vup * (img.ptr<Dot>(vf))[uf] +
           uup * vlow * (img.ptr<Dot>(vc))[uf] +
           ulow * vup * (img.ptr<Dot>(vf))[uc] +
           ulow * vlow * (img.ptr<Dot>(vc))[uc];
}

#endif /* Core_hpp */
