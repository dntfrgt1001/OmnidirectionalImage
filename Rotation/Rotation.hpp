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
#include "Quarternion.hpp"

class Rotation
{
public:
    Rotation();
    ~Rotation();
    
    // 単位四元数->回転行列
    static void Quart2RotMat(const Quarternion& quart, cv::Mat& rotMat);
    // 回転行列->四元数
    static void RotMat2Quart(const cv::Mat& rotMat, Quarternion& quart);
    // 回転ベクトル->回転行列
    static void RotVec2RotMat
    (const cv::Vec3f& rotVec, cv::Mat& rotMat);
    // 回転行列->回転ベクトル
    static void RotMat2RotVec
    (const cv::Mat& rotMat, cv::Vec3f& rotVec);

    // 回転行列を正規化
    static void normalRotMat(cv::Mat& rotMat);
};
#endif /* Rotation_hpp */
