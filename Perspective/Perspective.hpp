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
#include <opencv2/calib3d.hpp>

#include "Transform.hpp"

class Perspective
{
public:
    Perspective(const Transform& transform, cv::Mat& cameraMat):
    transform(transform), cameraMat(cameraMat){};
    
    // 球面座標->正規化画像座標
    static void psphere2regular
    (const cv::Point2f& psphere, cv::Point2f& regular) {
        regular.x = tanf(psphere.x);
        regular.y = - tanf(psphere.y)/cosf(psphere.x);
    }
    static void psphere2regular
    (const std::vector<cv::Point2f>& forRegular,
     std::vector<cv::Point2f>& latRegular);
    
    // 正規化画像座標->球面座標
    static void regular2psphere
    (const cv::Point2f& regular, cv::Point2f& psphere) {
        psphere.x = atanf(regular.x);
        psphere.y = - atanf(regular.y / sqrtf(1 + regular.x*regular.x));
    }
    
    // 正規化画像座標->透視投影画像座標
    void regular2pers(const cv::Point2f& regular, cv::Point2f& pers) const {
        pers.x = cameraMat.at<float>(0, 0)*regular.x+cameraMat.at<float>(0, 2);
        pers.y = cameraMat.at<float>(1, 1)*regular.y+cameraMat.at<float>(1, 2);
    }

    // 透視投影画像座標から正規化画像座標に変換
    void pers2regular(const cv::Point2f& pers, cv::Point2f& regular) const {
        regular.x = (pers.x-cameraMat.at<float>(0,2))/cameraMat.at<float>(0,0);
        regular.y = (pers.y-cameraMat.at<float>(1,2))/cameraMat.at<float>(1,1);
    }

    // 指定したθとφの範囲の透視投影画像を得る
    void persProjImg
    (const cv::Mat& img, float rtheta, float rphi,
     cv::Mat& persedImg) ;
    
    void setPersCenter(float cud, float cvd) {
        cameraMat.at<float>(0, 2) = cud;
        cameraMat.at<float>(1, 2) = cvd;
    }
    // 透視投影画像の画像中心を計算する
    void calcPersLength
    (float rtheta, float rphi, float& dud, float& dvd) const {
        float dx, dy;
        dpsphere2dregular(rtheta, rphi, dx, dy);
        dregular2dpers(dx, dy, dud, dvd);
    };
    // 球面正規化画像座標の距離を正規化画像座標の距離に変換
    // 線形変換ではないので，対称的な場合のみ
    float dpsphere2dx(float dtheta, float dphi) const {
        return 2 * tanf(dtheta / 2.0);
    }
    float dpsphere2dy(float dtheta, float dphi) const {
        return 2 * tanf(dphi / 2.0) / cosf(dtheta / 2.0);
    }
    void dpsphere2dregular
    (float dtheta, float dphi, float& dx, float& dy) const {
        dx = dpsphere2dx(dtheta, dphi); dy = dpsphere2dy(dtheta, dphi);
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
