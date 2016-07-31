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

class Transform{
public:
    Transform(const cv::Size& frameSize);
    
    // 画像の大きさを変換
    void resizeImg(const cv::Mat& img, cv::Mat& resImg) const {
        cv::resize(img, resImg, frameSize);
    }
    
    // 画像座標->球面極座標
    void psphere2equirect
    (const cv::Point2f& psphere, cv::Point2f& equirect) const {
        equirect.x = psphere.x*frameSize.width/(2.0*M_PI) + frameSize.width/2.0;
        equirect.y = -psphere.y*frameSize.height/M_PI + frameSize.height/2.0;
        correctequirect(equirect);
    }
    void psphere2equirect
    (const std::vector<cv::Point2f>& pspheres,
     std::vector<cv::Point2f>& equirects) const;
    
    // 球面極座標->画像座標
    void equirect2psphere
    (const cv::Point2f& equirect, cv::Point2f& psphere) const {
        psphere.x = (equirect.x-frameSize.width/2.0)*2.0*M_PI/frameSize.width;
        psphere.y = - (equirect.y-frameSize.height/2.0)*M_PI/frameSize.height;
        correctpsphere(psphere);
    }
    void equirect2psphere
    (const std::vector<cv::Point2f>& equirects,
     std::vector<cv::Point2f>& pspheres) const;
    // 画像座標と極座標の長さの変換
    float dtheta2u(float theta) const {
        return theta * frameSize.width / (2.0 * M_PI);
    }
    float dphi2v(float phi) const {
        return phi * frameSize.height / M_PI;
    }
    
    // 球面極座標->球面座標
    void psphere2sphere
    (const cv::Point2f& psphere, cv::Point3f& sphere) const {
        sphere.x = sinf(psphere.x) * cosf(psphere.y);
        sphere.y = - sinf(psphere.y);
        sphere.z = cosf(psphere.x) * cosf(psphere.y);
        correctsphere(sphere);
    }
    void psphere2sphere
    (const std::vector<cv::Point2f>& pspheres,
     std::vector<cv::Point3f>& spheres) const;
    
    // 球面座標->球面極座標
    void sphere2psphere
    (const cv::Point3f& sphere, cv::Point2f& psphere) const {
        float theta = (sphere.z!=0)? atanf(sphere.x/sphere.z):
                      ((sphere.x>0)? M_PI/2.0: -M_PI/2.0);
        if (sphere.z<0) { if (sphere.x < 0) theta -= M_PI; else theta += M_PI; }
        float phi = sphere.y<-1.0? asinf(-1.0):
                    (1.0<sphere.y? asinf(1.0): asinf(-sphere.y));
        psphere.x = theta; psphere.y = phi;
        correctpsphere(psphere);
    }
    void sphere2psphere
    (const std::vector<cv::Point3f>& spheres,
     std::vector<cv::Point2f>& pspheres) const;
    
    // 画像座標->球面座標
    void equirect2sphere
    (const cv::Point2f& equirect, cv::Point3f& sphere) const {
        cv::Point2f psphere;
        equirect2psphere(equirect, psphere);
        psphere2sphere(psphere, sphere);
    }
    void equirect2sphere
    (const std::vector<cv::Point2f>& equirects,
     std::vector<cv::Point3f>& spheres) const;
    // 球面座標->画像座標
    void sphere2equirect
    (const cv::Point3f& sphere, cv::Point2f& equirect) const {
        cv::Point2f psphere;
        sphere2psphere(sphere, psphere);
        psphere2equirect(psphere, equirect);
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
    
    // 点群変換のテンプレート
    template
    <class forTp, class latTp,
    void (Transform::*func)(const forTp&, latTp&) const>
    void points2points
    (const std::vector<forTp>& forPoints, std::vector<latTp>& latPoints) const;
    
    // 計算誤差を補正
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
    
    // 回転行列で球面座標を変換
    void sphereWithRotMat
    (const cv::Point3f& forsphere, cv::Point3f& latsphere,
     const cv::Mat& rotMat) const;
    // 回転行列で画像座標を変換
    void equirectWithRotMat
    (const cv::Point2f& forequirect, cv::Point2f& latequirect,
     const cv::Mat& rotMat) const;
    // 回転行列で画像を変換　逆向きの回転行列を使うことに注意
    void rotateImgWithRotMat
    (const cv::Mat& img, cv::Mat& rotImg, const cv::Mat& rotMat) const;
    // バイリニア補間による画素値の決定
    void getDotBilinear
    (const cv::Mat& img, const cv::Point2f& equirect, cv::Vec3b& dot) const;
    
    // 画像をX軸回り(縦方向)に回転
    void rotateVerticalImgRect
    (float chi, const cv::Mat& img,
     const cv::Rect& rect, cv::Mat& rotImg)  const;
    void rotateVerticalequirect
    (float chi, const cv::Point2f& forequirect,
     cv::Point2f& latequirect) const;
    void rotateVerticalpsphere
    (float chi, const cv::Point2f& forpsphere,
     cv::Point2f& latpsphere) const;
    
    // 片方のレンズ正面の画像を正規化画像座標に
    void normalCoord(const cv::Mat& img, float trange, float prange);
    
private:
    const cv::Size& frameSize;
};
#endif /* Transform_hpp */
