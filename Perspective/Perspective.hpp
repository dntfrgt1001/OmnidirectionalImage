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

#include "Core.hpp"
#include "Transform.hpp"

class Perspective
{
public:
    Perspective(const int persRad, const float rangeAngle);
    
    // 内部パラメータ行列を返す
    cv::Mat getInParaMat(const int persRad, const float ratio) const;
    
    // 透視投影画像を返す
    template <class Dot>
    void getPersImg
    (const cv::Mat& img, const cv::Mat& froMat,
     const bool isFront, cv::Mat& persImg) const;
    
    void getPersImg
    (const cv::Mat& img, const cv::Mat& froMat,
     const bool isFront, cv::Mat& persImg) const;
    
    // 透視投影画像の中で有効な部分のマスク(CalcOpticalFlow用)
    cv::Mat getMask(const float margin) const;
    
    // 有効範囲内か
    bool isInRange
    (const Pers& pers, const float rad) const {
        Normal normal = Map::pers2normal(pers, inParaMat);
        return normal.x*normal.x + normal.y*normal.y < rad*rad;
    }
    
    // 透視投影画像中の中心を返す
    void getPersCenter(cv::Point2f& center) const {
        center = cv::Point2f(inParaMat.at<float>(0, 2),
                             inParaMat.at<float>(1, 2));
    }
    
    //内部パラメータ行列を返す
    const cv::Mat& getInParaMat() const { return inParaMat; }
    
private:
    const cv::Mat inParaMat;
    const cv::Size pfs;
    const float normalRad;
    
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
    

};

#endif /* Perspective_hpp */
