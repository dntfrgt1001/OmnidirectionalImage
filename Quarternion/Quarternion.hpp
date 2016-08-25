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

#include <opencv2/core.hpp>

class Quarternion
{
public:
    Quarternion();
    Quarternion(float theta, cv::Vec3f& axis);
    ~Quarternion();
    friend std::ostream& operator<<(std::ostream& os, const Quarternion& quart);
    
    // 大きさ
    static float norm(Quarternion& quart);
    // 逆回転を表す四元数
    void transCounterRot();
    // 四元数を回転行列へ変換
    static void Quart2RotMat(const Quarternion& quart, cv::Mat& rotMat);
    // 回転行列を四元数へ変換
    static void RotMat2Quart(const cv::Mat& rotMat, Quarternion& quart);
    // 任意軸回転行列を生成
    static void Rodrigues2RotMat
    (const float angle, const cv::Vec3f& axis, cv::Mat& rotMat);
    // 回転行列から回転角度と回転軸を求める
    static void RotMat2Rodrigues
    (const cv::Mat& rotMat, float& angle, cv::Vec3f& axis);
    
    
    // 誤差を含む四元数を正規化
    static void normalQuart(Quarternion& quart);
    // 誤差を含む回転行列を正規化
    static void normalRotMat(cv::Mat& rotMat);
    
    float t;
    float x;
    float y;
    float z;
};

#endif /* Quarternion_hpp */
