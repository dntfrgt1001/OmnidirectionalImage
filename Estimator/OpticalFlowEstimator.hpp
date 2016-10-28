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
#include "OpticalFlow.hpp"
#include "Perspective.hpp"
#include "Epipolar.hpp"

class OpticalFlowEstimator: public Estimator
{
public:
    OpticalFlowEstimator
    (const cv::Size& fs, const Transform& tf,
     const OpticalFlow& of, const Perspective& pers, const Epipolar& epi):
    Estimator(fs, tf), of(of), epi(epi) {};
    
    cv::Mat getRotMat(const cv::Mat& forImg, const cv::Mat latImg) const;
    
private:
    const OpticalFlow& of;
    const Epipolar& epi;
};

#endif /* OpticalFlowEstimator_hpp */
