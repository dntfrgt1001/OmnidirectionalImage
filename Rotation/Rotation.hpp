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
    
    // 単位四元数->回転行列
    static cv::Mat Quat2RotMat(const Quaternion& quat);
    // 回転行列->四元数
    static Quaternion RotMat2Quat(const cv::Mat& rotMat);
    // 回転ベクトル->回転行列
    static cv::Mat RotVec2RotMat(const cv::Vec3f& rotVec);
    // 回転行列->回転ベクトル
    static cv::Vec3f RotMat2RotVec(const cv::Mat& rotMat);
    
    // ベクトルをz軸正の向きになるよう回転させる行列
    static cv::Mat getFroChgMat(const cv::Mat& rotMat);
    
    // 回転行列の座標系を変更
    static cv::Mat chgRotMatCoo
    (const cv::Mat& rotMat, const cv::Mat& froChgMat);

    // 回転行列を正規化
    static void normalRotMat(cv::Mat& rotMat);
};

#endif /* Rotation_hpp */
