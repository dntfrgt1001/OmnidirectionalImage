//
//  MainProcess.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef MainProcess_hpp
#define MainProcess_hpp

#include <stdio.h>

#include <opencv2/core.hpp>

#include "Transform.hpp"
#include "FeatureMatchEstimator.hpp"
#include "OpticalFlowEstimator.hpp"

class MainProcess
{
public:
    MainProcess
    (const Transform& tf, const FeatureMatchEstimator& fme,
     const OpticalFlowEstimator& ofe): tf(tf), fme(fme), ofe(ofe) {};
    
    void modifyLatImgFeatureMatch
    (const cv::Mat& forImg, const cv::Mat& latImg, cv::Mat& modLatImg);
    
    void modifyLatImgOpticalFlow
    (const cv::Mat& forImg, const cv::Mat& latImg, cv::Mat& modLatImg);
    
private:
    const Transform& tf; // 出力画像用
    const FeatureMatchEstimator& fme;
    const OpticalFlowEstimator& ofe;
};

#endif /* MainProcess_hpp */
