//
//  OpticalFlow.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/20.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef OpticalFlow_hpp
#define OpticalFlow_hpp

#include <stdio.h>

#include <opencv2/tracking.hpp>
#include <opencv2/video.hpp>

class OpticalFlow {
public:
    OpticalFlow(){};

    void getFeatures
    (const cv::Mat& persImg, std::vector<cv::Point2f>& points,
     const cv::Mat& mask) const;

    void getOpticalFlow
    (const cv::Mat& forPersImg, const cv::Mat& latPersImg,
     const std::vector<cv::Point2f>& forPoints,
     std::vector<cv::Point2f>& latPoints) const;
    
    void drawPoint
    (const cv::Mat& persImg, const std::vector<cv::Point2f>& points,
     cv::Mat& outImg);
    
    void drawOpticalFlow
    (const cv::Mat& persImg, const std::vector<cv::Point2f>& forPoints,
     const std::vector<cv::Point2f>& latPoints, cv::Mat& outImg);
};

#endif /* OpticalFlow_hpp */
