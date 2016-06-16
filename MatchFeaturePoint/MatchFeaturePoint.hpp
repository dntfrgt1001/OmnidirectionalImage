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

#include "Transform.hpp"

class MatchFeaturePoint
{
public:
    MatchFeaturePoint
    (const cv::Size& frameSize, const Transform& transform,
     int distThreshold, float coordThreshold);
    ~MatchFeaturePoint();
    
    void match
    (const cv::Mat descriptors1, const cv::Mat descriptors2,
     std::vector<cv::DMatch>& dMatches) const;
    // 1->2，2->1へのクロスマッチ
    void crossMatch
    (const std::vector<cv::DMatch>& dMatches1,
     const std::vector<cv::DMatch>& dMatches2,
     std::vector<cv::DMatch>& dMatches) const;
    // マッチングの距離が閾値以下のものはフィルタリング
    void filterMatchDistance
    (std::vector<cv::DMatch>& dMatches) const;
    // マッチングの座標が大きく離れているものはフィルタリング
    void filterMatchCoordinate
    (std::vector<cv::Point3f>& for3DPoints,
     std::vector<cv::Point3f>& lat3DPoints);
    
private:
    const cv::Size& frameSize;
    const Transform& transform;
    
    cv::Ptr<cv::DescriptorMatcher> matcher;
    const int distThreshold;
    const float coordThreshold;
};

#endif /* MatchFeaturePoint_hpp */
