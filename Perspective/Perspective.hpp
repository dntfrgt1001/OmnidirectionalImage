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
    Perspective(const Transform& transform, const cv::Mat& cameraMat);
    ~Perspective(){};
    
    // 正規化球面画像座標から正規化画像座標に変換
    static float pregular2x(float theta, float phi = 0.0) {
        return tanf(theta);
    }
    static float pregular2y(float theta, float phi) {
        return - tanf(phi)/cosf(theta);
    }
    // 正規化画像座標から透視投影画像座標に変換
    float x2ud(float x) const {
        return cameraMat.at<float>(0, 0) * x + cameraMat.at<float>(0, 2);
    }
    float y2vd(float y) const {
        return cameraMat.at<float>(1, 1) * y + cameraMat.at<float>(1, 2);
    }
    
    // 指定したθとφの範囲の透視投影画像を得る
    void persProjImg
    (const cv::Mat& img, cv::Mat& persedImg,
     float rangeTheta, float rangePhi) const;
    
private:
    const Transform& transform;
    const cv::Mat& cameraMat;
};

#endif /* Perspective_hpp */
