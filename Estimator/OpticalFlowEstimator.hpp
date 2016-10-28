//
//  OpticalFlowEstimator.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef OpticalFlowEstimator_hpp
#define OpticalFlowEstimator_hpp

#include <stdio.h>

#include <opencv2/core.hpp>

#include "Estimator.hpp"
#include "Perspective.hpp"
#include "Epipolar.hpp"
#include "Rotation.hpp"
#include "CalcOpticalFlow.hpp"

class OpticalFlowEstimator: public Estimator
{
public:
    OpticalFlowEstimator
    (const Transform& tf, const CalcOpticalFlow& cof,
     const Perspective& per, const Epipolar& epi):
    Estimator(tf), cof(cof), per(per), epi(epi) {};
    
    cv::Mat getRotMat(const cv::Mat& forImg, const cv::Mat& latImg) const;
    
    cv::Mat getRotMat
    (const cv::Mat& forImg, const cv::Mat& latImg,
     const cv::Mat& froChgMat) const;
    
private:
    const CalcOpticalFlow& cof;
    const Perspective& per;
    const Epipolar& epi;
};

#endif /* OpticalFlowEstimator_hpp */
