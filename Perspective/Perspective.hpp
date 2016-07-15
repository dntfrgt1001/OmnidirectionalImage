//
//  Perspective.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/07/13.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Perspective_hpp
#define Perspective_hpp

#include <stdio.h>
#include <iostream>

#include <opencv2/core.hpp>

#include "Transform.hpp"

class Perspective
{
public:
    Perspective(const Transform& transform, cv::Mat& cameraMat):
    transform(transform), cameraMat(cameraMat){};
    
    // 球面正規化画像座標から正規化画像座標に
    static float pregular2x(float theta, float phi = 0.0) {
        return tanf(theta);
    }
    static float pregular2y(float theta, float phi) {
        return - tanf(phi)/cosf(theta);
    }
    static void pregular2regular(float theta, float phi, float& x, float& y) {
        x = pregular2x(theta, phi); y = pregular2y(theta, phi);
    }
    
    // 正規化画像座標から球面正規化画像座標に変換 立体射影に修正
    static float regular2theta(float x, float y = 0.0) {
        return atanf(x);
        /*
        float xp = (4*x) / (x*x + y*y + 4);
        float yp = (4*y) / (x*x + y*y + 4);
        float zp = (-x*x - y*y + 4) / (x*x + y*y + 4);
        return atanf(xp / zp);
         */
    }
    static float regular2phi(float x, float y) {
        return - atanf(y / sqrtf(1 + x*x));
        /*
        float xp = (4*x) / (x*x + y*y + 4);
        float yp = (4*y) / (x*x + y*y + 4);
        float zp = (-x*x - y*y + 4) / (x*x + y*y + 4);
        return yp<-1.0? asinf(-1.0): (1.0<yp? asinf(1.0): asinf(-yp));
         */
    }
    static void regular2pregular(float x, float y, float& theta, float& phi) {
        theta = regular2theta(x, y); phi = regular2phi(x, y);
    }
    
    // 正規化画像座標から透視投影画像座標に変換
    float x2ud(float x) const {
        return cameraMat.at<float>(0, 0) * x + cameraMat.at<float>(0, 2);
    }
    float y2vd(float y) const {
        return cameraMat.at<float>(1, 1) * y + cameraMat.at<float>(1, 2);
    }
    void regular2pers(float x, float y, float& ud, float& vd) const {
        ud = x2ud(x); vd = y2vd(y);
    }
    // 透視投影画像座標から正規化画像座標に変換
    float ud2x(float ud) const {
        return (ud - cameraMat.at<float>(0, 2)) / cameraMat.at<float>(0, 0);
    }
    float vd2y(float vd) const {
        return (vd - cameraMat.at<float>(1, 2)) / cameraMat.at<float>(1, 1);
    }
    void pers2regular(float ud, float vd, float& x, float& y) const {
        x = ud2x(ud); y = vd2y(vd);
    }
    // 指定したθとφの範囲の透視投影画像を得る
    void persProjImg
    (const cv::Mat& img, float rangeTheta, float rangePhi,
     cv::Mat& persedImg) ;
    
    void setPersCenter(float cud, float cvd) {
        cameraMat.at<float>(0, 2) = cud;
        cameraMat.at<float>(1, 2) = cvd;
    }
    // 透視投影画像の画像中心を計算する
    void calcPersLength
    (float rangeTheta, float rangePhi, float& dud, float& dvd) const {
        float dx, dy;
        dpregular2dregular(rangeTheta, rangePhi, dx, dy);
        dregular2dpers(dx, dy, dud, dvd);
    };
    // 球面正規化画像座標の距離を正規化画像座標の距離に変換
    // 線形変換ではないので，対称的な場合のみ 立体射影に修正
    float dpregular2dx(float dtheta, float dphi) const {
        return 2 * tanf(dtheta / 2.0);
        /*
        float theta = dtheta / 2.0;
        float phi = dphi / 2.0;
        float xp = sinf(theta) * cosf(phi);
        float yp = -1 * sinf(phi);
        float zp = cosf(theta) * cosf(phi);
        
        return 2 * (2*xp / (1+zp));
         */
    }
    float dpregular2dy(float dtheta, float dphi) const {
        return 2 * tanf(dphi / 2.0) / cosf(dtheta / 2.0);
        /*
        float theta = dtheta / 2.0;
        float phi = dphi / 2.0;
        float xp = sinf(theta) * cosf(phi);
        float yp = -1 * sinf(phi);
        float zp = cosf(theta) * cosf(phi);
        
        return 2 * (2*(-yp) / (1+zp));
         */
    }
    void dpregular2dregular
    (float dtheta, float dphi, float& dx, float& dy) const {
        dx = dpregular2dx(dtheta, dphi); dy = dpregular2dy(dtheta, dphi);
    }
    // 正規化画像座標の距離を透視投影画像座標の距離に変換
    float dx2dud(float dx) const {
        return cameraMat.at<float>(0, 0) * dx;
    }
    float dy2dvd(float dy) const {
        return cameraMat.at<float>(1, 1) * dy;
    }
    void dregular2dpers(float dx, float dy, float& dud, float& dvd) const {
        dud = dx2dud(dx); dvd = dy2dvd(dy);
    }

    
private:
    const Transform& transform;
    cv::Mat& cameraMat;
};

#endif /* Perspective_hpp */
