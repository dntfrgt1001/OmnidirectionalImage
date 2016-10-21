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
/*
float ellLengHori = inParaMat.at<float>(0, 0) * rangeRadius;
float ellLengVert = inParaMat.at<float>(1, 1) * rangeRadius;

float persWidth = inParaMat.at<float>(0, 2) + ellLengHori;
float persHeight = inParaMat.at<float>(1, 2) + ellLengVert;
*/

class Perspective
{
public:
    Perspective
    (const Transform& tf, const cv::Mat& inParaMat,
     const float rangeAngle);

    // 透視投影画像を返す
    void getPersImg
    (const cv::Mat& img, cv::Mat& persImg,
     const cv::Mat& froMat, const bool isFront) const;
    
    // 透視投影画像の中で有効な部分のマスク
    void getMask(cv::Mat& mask);
    
    // 透視投影画像中の中心を返す
    void getPersCenter(cv::Point2f& center) const {
        center = cv::Point2f(inParaMat.at<float>(0, 2),
                             inParaMat.at<float>(1, 2));
    }
    
    const cv::Mat& getInParaMat() const {
        return inParaMat;
    }
    
    /*
    // 指定したθとφの範囲の透視投影画像を得る
    void persProjImg
    (const cv::Mat& img, float rtheta, float rphi,
     cv::Mat& persedImg) ;

    void setPersCenter(float cud, float cvd) {
        inParaMat.at<float>(0, 2) = cud;
        inParaMat.at<float>(1, 2) = cvd;
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
        return inParaMat.at<float>(0, 0) * dx;
    }
    float dy2dvd(float dy) const {
        return inParaMat.at<float>(1, 1) * dy;
    }
    void dregular2dpers(float dx, float dy, float& dud, float& dvd) const {
        dud = dx2dud(dx); dvd = dy2dvd(dy);
    }
     */
    
private:
    const Transform& tf;
    const cv::Mat& inParaMat;
    
    const float rangeRadius;
    const float ellLengHori;
    const float ellLengVert;
    const float persWidth;
    const float persHeight;
};

#endif /* Perspective_hpp */
