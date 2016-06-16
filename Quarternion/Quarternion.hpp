//
//  Quarternion.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/10.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Quarternion_hpp
#define Quarternion_hpp

#include <stdio.h>
#include <math.h>
#include <opencv2/core/core.hpp>

class Quarternion
{
public:
    Quarternion(float theta, cv::Vec3f& axis);
    ~Quarternion();
    void transCounterRot();
    // クオータニオンを回転行列へ変換
    static void Quart2RotMat(const Quarternion& quart, cv::Mat& rotMat);
    // 回転行列をクオータニオンへ変換
//    static void RotMat2Quart(const cv::Mat& rotMat, Quarternion& quart);
    // 任意軸回転行列を生成
    static void arbRotMat
    (float theta, cv::Vec3f& axis, cv::Mat& rotMat);
    // 誤差を含む回転行列を正規化
    static void normalizeRotMat(cv::Mat& rotMat);
    
private:
    float t;
    float x;
    float y;
    float z;
};

#endif /* Quarternion_hpp */
