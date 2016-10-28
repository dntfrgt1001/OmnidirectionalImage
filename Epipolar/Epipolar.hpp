//
//  Epipolar.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Epipolar_hpp
#define Epipolar_hpp

#include <stdio.h>

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>

#include "Transform.hpp"
#include "Range.hpp"
#include "Rotation.hpp"

class Epipolar {
public:
    Epipolar(const Transform& tf, const Range& rg, const int numThre);
    
    // 8点アルゴリズムで回転行列を推定する
    cv::Mat getRotMatEssMat
    (const std::vector<cv::Point2f>& forNormals,
     const std::vector<cv::Point2f>& latNormals, cv::Mat& mask) const;
    
    // 指定した方向の特徴点で回転行列を推定
    cv::Mat getRotMatSpecDir
    (const std::vector<cv::Point3f>& forSpheres,
     const std::vector<cv::Point3f>& latSpheres,
     const cv::Mat& froChgMat, float& weight) const;
    
    // 最大重みの回転行列
    cv::Mat getRotMatWeightMax
    (const std::vector<cv::Point3f>& forSpheres,
     const std::vector<cv::Point3f>& latSpheres, int& maxIdx) const;
    
    // マスクから重みを計算する
    float getWeight(const cv::Mat& mask) const;
    
    // 最も重みの大きいインデックスを返す
    int getMaxWeightIndex(const std::vector<float>& weights) const;
    
private:
    const Transform& tf;
    const Range& rg;
    const int numThre;
    
    std::vector<cv::Mat> froChgMats;
};

#endif /* Epipolar_hpp */
