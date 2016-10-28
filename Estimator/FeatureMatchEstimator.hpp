//
//  FeatureMatchEstimator.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef FeatureMatchEstimator_hpp
#define FeatureMatchEstimator_hpp

#include <stdio.h>

#include <opencv2/core.hpp>

#include "Estimator.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Range.hpp"
#include "Estimate.hpp"
#include "Epipolar.hpp"

class FeatureMatchEstimator: public Estimator {
public:
    FeatureMatchEstimator
    (const Transform& tf, const ExtractFeaturePoint& efp,
     const MatchFeaturePoint& mfp, const Epipolar& epi, const Range& ran);
    
    // 画像は入力したそのまま
    cv::Mat getRotMat(const cv::Mat& forImg, const cv::Mat& latImg) const;
    
    // 指定した方向の特徴点で回転行列を推定
    cv::Mat getRotMatSpecDir
    (const std::vector<cv::Point3f>& forSpheres,
     const std::vector<cv::Point3f>& latSpheres,
     const cv::Mat& froChgMat, float &weight) const;
    
    // 最大重みの回転行列
    cv::Mat getRotMatWeightMax
    (const std::vector<cv::Point3f>& forSpheres,
     const std::vector<cv::Point3f>& latSpheres, int& maxIdx) const;
    
    // マスクから重みを計算する
    float getWeight(const cv::Mat& mask) const;
    
    // 最も重みの大きいインデックスを返す
    int getMaxWeightIndex(const std::vector<float>& weights) const;
    
private:
    const ExtractFeaturePoint& efp;
    const MatchFeaturePoint& mfp;
    const Epipolar& epi;
    const Range& ran;
    std::vector<cv::Mat> froChgMats;
};

#endif /* FeatureMatchEstimator_hpp */
