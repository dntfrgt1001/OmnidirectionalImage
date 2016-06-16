//
//  MatchFeaturePoint.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/05/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "MatchFeaturePoint.hpp"

MatchFeaturePoint::MatchFeaturePoint
(const cv::Size& frameSize, const Transform& transform,
 int distThreshold, float coordThreshold):
frameSize(frameSize), transform(transform),
distThreshold(distThreshold), coordThreshold(coordThreshold)
{
    matcher = cv::DescriptorMatcher::create("BruteForce");
}

MatchFeaturePoint::~MatchFeaturePoint()
{
    
}

void MatchFeaturePoint::match
(const cv::Mat descriptors1, const cv::Mat descriptors2,
 std::vector<cv::DMatch> &dMatches) const
{
    std::vector<cv::DMatch> dMatches1, dMatches2;
    
    matcher->match(descriptors1, descriptors2, dMatches1);
    matcher->match(descriptors2, descriptors1, dMatches2);
    
    crossMatch(dMatches1, dMatches2, dMatches);
}

void MatchFeaturePoint::crossMatch
(const std::vector<cv::DMatch> &dMatches1,
 const std::vector<cv::DMatch> &dMatches2,
 std::vector<cv::DMatch> &dMatches) const
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

void MatchFeaturePoint::filterMatchDistance
(std::vector<cv::DMatch> &dMatches) const
{
    // .eraseに注意
    for (int i=0; i<dMatches.size();  ) {
        if (dMatches[i].distance > distThreshold) {
            dMatches.erase(dMatches.begin() + i);
        } else {
            i++;
        }
    }
}

void MatchFeaturePoint::filterMatchCoordinate
(std::vector<cv::Point3f> &for3DPoints, std::vector<cv::Point3f> &lat3DPoints)
{
    // .eraseに注意
    for (int i=0; i<for3DPoints.size(); ) {
        if (cv::norm(for3DPoints[i] - lat3DPoints[i]) > coordThreshold) {
            for3DPoints.erase(for3DPoints.begin() + i);
            lat3DPoints.erase(lat3DPoints.begin() + i);
        } else {
            i++;
        }
    }
}