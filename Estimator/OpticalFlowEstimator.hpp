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
#include <string>
#include <iostream>
#include <iomanip>

#include "Estimator.hpp"
#include "Perspective.hpp"
#include "Epipolar.hpp"
#include "Rotation.hpp"
#include "CalcOpticalFlow.hpp"

class OpticalFlowEstimator: public Estimator
{
public:
    OpticalFlowEstimator
    (const CalcOpticalFlow& cof,
     const Perspective& per, const Epipolar& epi):
    cof(cof), per(per), epi(epi) {};
    
    // インタフェース
    cv::Mat getRotMat
    (const cv::Mat& forImg, const cv::Mat& latImg, const State& state);
    
    // 実体
    cv::Mat getRotMat
    (const cv::Mat& forImg, const cv::Mat& latImg, const cv::Mat& curRotMat);
    
private:
    const CalcOpticalFlow& cof;
    const Perspective& per;
    const Epipolar& epi;
};

#endif /* OpticalFlowEstimator_hpp */
