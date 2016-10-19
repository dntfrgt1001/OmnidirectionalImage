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

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//#include <libiomp/omp.h>

class Transform{
public:
    Transform(const cv::Size& frameSize);
    
    // 画像の大きさを変換
    void resizeImg(const cv::Mat& img, cv::Mat& resImg) const {
        cv::resize(img, resImg, frameSize);
    }
    
    // 画像座標->球面極座標
    void polar2equirect
    (const cv::Point2f& polar, cv::Point2f& equirect) const {
        equirect.x = polar.x*frameSize.width/(2.0*M_PI) +
                     frameSize.width/2.0;
        equirect.y = -polar.y*frameSize.height/M_PI +
                     frameSize.height/2.0;
        correctequirect(equirect);
    }
    void polar2equirect
    (const std::vector<cv::Point2f>& polars,
     std::vector<cv::Point2f>& equirects) const;
    
    // 球面極座標->画像座標
    void equirect2polar
    (const cv::Point2f& equirect, cv::Point2f& polar) const {
        polar.x = (equirect.x-frameSize.width/2.0) *
                    2.0*M_PI/frameSize.width;
        polar.y = - (equirect.y-frameSize.height/2.0) *
                    M_PI/frameSize.height;
        correctpsphere(polar);
    }
    void equirect2polar
    (const std::vector<cv::Point2f>& equirects,
     std::vector<cv::Point2f>& polars) const;
    
    // 画像座標と極座標の長さの変換
    float dtheta2u(float theta) const {
        return theta * frameSize.width / (2.0 * M_PI);
    }
    float dphi2v(float phi) const {
        return phi * frameSize.height / M_PI;
    }
    
    // 球面極座標->球面座標
    void polar2sphere
    (const cv::Point2f& polar, cv::Point3f& sphere) const {
        sphere.x = sinf(polar.x) * cosf(polar.y);
        sphere.y = - sinf(polar.y);
        sphere.z = cosf(polar.x) * cosf(polar.y);
        correctsphere(sphere);
    }
    void polar2sphere
    (const std::vector<cv::Point2f>& polars,
     std::vector<cv::Point3f>& spheres) const;
    
    // 球面座標->球面極座標
    void sphere2polar
    (const cv::Point3f& sphere, cv::Point2f& polar) const {
        float theta = (sphere.z!=0)? atanf(sphere.x/sphere.z):
                      ((sphere.x>0)? M_PI/2.0: -M_PI/2.0);
        if (sphere.z<0) { if (sphere.x < 0) theta -= M_PI; else theta += M_PI; }
        float phi = sphere.y<-1.0? asinf(-1.0):
                    (1.0<sphere.y? asinf(1.0): asinf(-sphere.y));
        polar.x = theta; polar.y = phi;
        correctpsphere(polar);
    }
    void sphere2psphere
    (const std::vector<cv::Point3f>& spheres,
     std::vector<cv::Point2f>& pspheres) const;
    
    // 画像座標->球面座標
    void equirect2sphere
    (const cv::Point2f& equirect, cv::Point3f& sphere) const {
        cv::Point2f polar;
        equirect2polar(equirect, polar);
        polar2sphere(polar, sphere);
    }
    void equirect2sphere
    (const std::vector<cv::Point2f>& equirects,
     std::vector<cv::Point3f>& spheres) const;
    // 球面座標->画像座標
    void sphere2equirect
    (const cv::Point3f& sphere, cv::Point2f& equirect) const {
        cv::Point2f polar;
        sphere2polar(sphere, polar);
        polar2equirect(polar, equirect);
    }
    void sphere2equirect
    (const std::vector<cv::Point3f>& spheres,
     std::vector<cv::Point2f>& equirects) const;
    
    // 球面座標->正規化画像座標
    void sphere2normal
    (const cv::Point3f& sphere, cv::Point2f& normal) const {
        normal.x = sphere.x / sphere.z;
        normal.y = sphere.y / sphere.z;
    }
    void sphere2normal
    (const std::vector<cv::Point3f>& spheres,
     std::vector<cv::Point2f>& normals) const;
    
    // 正規化画像座標->透視投影画像座標
    void normal2pers
    (const cv::Point2f& normal, cv::Point2f& pers,
     const cv::Mat& inParaMat) const {
        pers.x = inParaMat.at<float>(0,0)*normal.x+inParaMat.at<float>(0,2);
        pers.y = inParaMat.at<float>(1,1)*normal.y+inParaMat.at<float>(1,2);
    }
    
    // 透視投影画像座標->正規化画像座標
    void pers2normal
    (const cv::Point2f& pers, cv::Point2f& normal,
     const cv::Mat& inParaMat) const {
        normal.x = (pers.x - inParaMat.at<float>(0,2)) /
                    inParaMat.at<float>(0,0);
        normal.y = (pers.y - inParaMat.at<float>(1,2)) /
                    inParaMat.at<float>(1,1);
    }
    
    // 正規化画像座標->球面座標
    void normal2sphere
    (const cv::Point2f& normal, cv::Point3f& sphere,
     const bool isFront) const {
        float denomi = sqrtf(normal.x*normal.x + normal.y*normal.y + 1.0);
        sphere.x = normal.x / denomi;
        sphere.y = normal.y / denomi;
        sphere.z = 1 / denomi;
        if (!isFront) sphere = -1 * sphere;
        
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
    void correctequirect(cv::Point2f& equirect) const {
        correctDomain(equirect.x, 0.0, frameSize.width-1);
        correctDomain(equirect.y, 0.0, frameSize.height-1);
    }
    static void correctpsphere(cv::Point2f& psphere) {
        correctDomain(psphere.x, -M_PI, M_PI);
        correctDomain(psphere.y, -M_PI/2.0, M_PI/2.0);
    }
    static void correctsphere(cv::Point3f& sphere) {
        correctDomain(sphere.x, -1, 1);
        correctDomain(sphere.y, -1, 1);
        correctDomain(sphere.z, -1, 1);
    }
    
    // 回転行列で画像座標を回転
    void rotateEquirect
    (const cv::Point2f& equirect, cv::Point2f& equirectRot,
     const cv::Mat& rotMat) const {
        cv::Point3f sphere, sphereRot;
        equirect2sphere(equirect, sphere);
        rotateSphere(sphere, sphereRot, rotMat);
        sphere2equirect(sphereRot, equirectRot);
    }
    // 回転行列で球面座標を回転
    void rotateSphere
    (const cv::Point3f& sphere, cv::Point3f& sphereRot,
     const cv::Mat& rotMat) const {
        sphereRot = (cv::Point3f) cv::Mat1f(rotMat * cv::Mat1f(sphere));
    }
    void rotateSphere
    (const std::vector<cv::Point3f>& spheres,
     std::vector<cv::Point3f>& spheresRot, const cv::Mat& rotMat) const;
    
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
    (const float angle, const cv::Point2f& equirect,
     cv::Point2f& equirectRot) const {
        cv::Point3f sphere, sphereRot;
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
    
    // 片方のレンズ正面の画像を正規化画像座標に
    void normalCoord(const cv::Mat& img, float trange, float prange);
    
private:
    const cv::Size& frameSize;
};
#endif /* Transform_hpp */
