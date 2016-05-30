//
//  MatchFeaturePoint.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/05/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef MatchFeaturePoint_hpp
#define MatchFeaturePoint_hpp

#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

class MatchFeaturePoint
{
public:
    MatchFeaturePoint(const cv::Size& frameSize);
    ~MatchFeaturePoint();
    
    void match
    (const cv::Mat descriptors1, const cv::Mat descriptors2,
     std::vector<cv::DMatch>& dMatches);
    void crossMatch
    (const std::vector<cv::DMatch>& dMatches1,
     const std::vector<cv::DMatch>& dMatches2,
     std::vector<cv::DMatch>& dMatches);
    
private:
    const cv::Size& frameSize;
    
    cv::Ptr<cv::DescriptorMatcher> matcher;
};

#endif /* MatchFeaturePoint_hpp */
