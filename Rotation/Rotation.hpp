//
//  Rotation.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/07/04.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Rotation_hpp
#define Rotation_hpp

#include <stdio.h>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2//features2d.hpp>
#include <opencv2/calib3d.hpp>

#include "Transform.hpp"
#include "Quaternion.hpp"

class Rotation
{
public:
    Rotation();
    ~Rotation();
    
    static void getRotMat
    (const float angle, const cv::Vec3f& axis, cv::Mat rotMat);
    static void getRotVec
    (const float angle, const cv::Vec3f& axis, cv::Vec3f& rotVec);
    static void getQuaternion
    (const float angle, const cv::Vec3f& axis, Quaternion& quat);
    
    // 単位四元数->回転行列
    static void Quat2RotMat(const Quaternion& quat, cv::Mat& rotMat);
    // 回転行列->四元数
    static void RotMat2Quat(const cv::Mat& rotMat, Quaternion& quat);
    // 回転ベクトル->回転行列
    static void RotVec2RotMat
    (const cv::Vec3f& rotVec, cv::Mat& rotMat);
    // 回転行列->回転ベクトル
    static void RotMat2RotVec
    (const cv::Mat& rotMat, cv::Vec3f& rotVec);
    
    // ベクトルをz軸正の向きになるよう回転させる行列
    static void vec2zDirMat(const cv::Vec3f& vec, cv::Mat& rotMat);

    // 回転行列を正規化
    static void normalRotMat(cv::Mat& rotMat);
};
#endif /* Rotation_hpp */
