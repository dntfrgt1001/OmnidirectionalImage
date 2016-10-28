//
//  OpticalFlowEstimator.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "OpticalFlowEstimator.hpp"

cv::Mat OpticalFlowEstimator::getRotMat
(const cv::Mat &forImg, const cv::Mat latImg) const
{
    // グレースケールに変換
    cv::Mat forImgGray, latImgGray;
    cv::cvtColor(forImg, forImgGray, CV_BGR2GRAY);
    cv::cvtColor(latImg, latImgGray, CV_BGR2GRAY);
    
    
}