//
//  MatchFeaturePoint.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/05/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "MatchFeaturePoint.hpp"

MatchFeaturePoint::MatchFeaturePoint(const cv::Size& frameSize):
frameSize(frameSize)
{
    matcher = cv::DescriptorMatcher::create("BruteForce");
}

MatchFeaturePoint::~MatchFeaturePoint()
{
    
}

void MatchFeaturePoint::match
(const cv::Mat descriptors1, const cv::Mat descriptors2,
 std::vector<cv::DMatch> &dMatches)
{
    std::vector<cv::DMatch> dMatches1, dMatches2;
    
    matcher->match(descriptors1, descriptors2, dMatches1);
    matcher->match(descriptors2, descriptors1, dMatches2);
    
    crossMatch(dMatches1, dMatches2, dMatches);
}

void MatchFeaturePoint::crossMatch
(const std::vector<cv::DMatch> &dMatches1,
 const std::vector<cv::DMatch> &dMatches2, std::vector<cv::DMatch> &dMatches)
{
    dMatches.clear();
    
    for (int i=0; i<dMatches1.size(); i++) {
        cv::DMatch forward = dMatches1[i];
        cv::DMatch backward = dMatches2[forward.trainIdx];
        
        if (forward.queryIdx == backward.trainIdx) {
            dMatches.push_back(forward);
        }
    }
}


