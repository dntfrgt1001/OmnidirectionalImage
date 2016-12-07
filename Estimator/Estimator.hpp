//
//  Estimator.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Estimator_hpp
#define Estimator_hpp

#include <stdio.h>

#include <opencv2/core.hpp>

#include "Transform.hpp"

class Estimator {
public:
    Estimator(const Transform& tf): tf(tf) {};
    
    virtual cv::Mat getRotMat
    (const cv::Mat& forImg, const cv::Mat& latImg, const int frameNum) = 0;
    
protected:
    const Transform& tf;
};

#endif /* Estimator_hpp */
