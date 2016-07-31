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
    Rotation(const Transform& transform);
    ~Rotation();
    
    // 特異値分解により回転行列を推定する [forP] = [R]*[latP]
    void estimate3DRotMatSVD
    (const std::vector<cv::Point3f>& forspheres,
     const std::vector<cv::Point3f>& latspheres, cv::Mat& estRotMat) const;
    // ランダムに点を取り出す
    static void getRondomPoint
    (const std::vector<cv::Point3f>& forPoints,
     const std::vector<cv::Point3f>& latPoints,
     std::vector<cv::Point3f>& forPointx, std::vector<cv::Point3f>& latPointx,
     size_t getSize);
    // ランダムに取り出した少数の点から回転行列を推定
    static void estimate3DRotMatSVDPartial
    (const std::vector<cv::Point3f>& forPointx,
     const std::vector<cv::Point3f>& latPointx, cv::Mat& estRotMatPart);
    // 推定した回転行列を評価
    float evalEstRotMat
    (const std::vector<cv::Point3f>& forPoints,
     const std::vector<cv::Point3f>& latPoints,
     const cv::Mat& estRotMat) const;
    // 閾値以下のサンプルを取り出す
    size_t removeOutlier
    (const std::vector<cv::Point3f>& forPoints,
     const std::vector<cv::Point3f>& latPoints,
     std::vector<cv::Point3f>& inForPoints,
     std::vector<cv::Point3f>& inLatPoints,
     float normThre, const cv::Mat& estRotMat) const;
    // 四元数を使って回転行列を正規化
    static void normalRotMat(cv::Mat& rotMat);
    
    // 基本行列により回転行列を推定する
    void estimate3DRotMatEssential
    (const std::vector<cv::Point2f>& fornormals,
     const std::vector<cv::Point2f>& latnormals, cv::Mat& estRotMat) const;
    
private:
    const Transform& transform;
};

#endif /* Rotation_hpp */
