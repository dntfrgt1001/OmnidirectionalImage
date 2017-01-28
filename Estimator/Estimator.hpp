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

typedef struct State {
    int frameNum;
    cv::Mat& curRotMat;
} State;

class Estimator {
public:
    Estimator(const Transform& tf): tf(tf) {};
    
    // インタフェース
    virtual cv::Mat getRotMat
    (const cv::Mat& forImg, const cv::Mat& latImg, const State& state) = 0;
    
protected:
    const Transform& tf;
};

#endif /* Estimator_hpp */
