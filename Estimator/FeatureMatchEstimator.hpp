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
     const MatchFeaturePoint& mfp,const Epipolar& epi):
    Estimator(tf), efp(efp), mfp(mfp), epi(epi) {};
    
    // 画像は入力したそのまま
    cv::Mat getRotMat(const cv::Mat& forImg, const cv::Mat& latImg) const;

private:
    const ExtractFeaturePoint& efp;
    const MatchFeaturePoint& mfp;
    const Epipolar& epi;
};

#endif /* FeatureMatchEstimator_hpp */
