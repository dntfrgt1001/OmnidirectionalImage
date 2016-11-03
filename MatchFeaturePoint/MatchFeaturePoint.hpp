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

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

#include "Transform.hpp"

class MatchFeaturePoint
{
public:
    MatchFeaturePoint
    (const Transform& transform, const float distThreshold,
     const float coordThreshold);
    
    void match
    (const cv::Mat descriptors1, const cv::Mat descriptors2,
     std::vector<cv::DMatch>& dMatches) const;
   
    // 1->2，2->1へのクロスマッチ
    static void crossMatch
    (const std::vector<cv::DMatch>& dMatches1,
     const std::vector<cv::DMatch>& dMatches2,
     std::vector<cv::DMatch>& dMatches) ;
    
    // マッチング距離が閾値以下のものはフィルタリング
    void filterMatchDistance
    (std::vector<cv::DMatch>& dMatches) const;
    
    // 座標距離が大きく離れているものはフィルタリング
    void filterCoordDistance
    (std::vector<cv::Point3f>& forSpheres,
     std::vector<cv::Point3f>& latSpheres) const;
    
    // マッチの描画
    static void drawMatchVert
    (const cv::Mat& img1, const std::vector<cv::Point2f>& forEquirects,
     const cv::Mat& img2, const std::vector<cv::Point2f>& latEquirects,
     cv::Mat& outImg);

    // マッチした特徴点を返す
    void getMatchKeyPoint
    (const std::vector<cv::KeyPoint>& forKeyPointsSet,
     const std::vector<cv::KeyPoint>& latKeyPointsSet,
     const std::vector<cv::DMatch>& matchs,
     std::vector<cv::KeyPoint>& forKeyPoints,
     std::vector<cv::KeyPoint>& latKeyPoints) const;
    
private:
    const Transform& transform;
    
    const cv::Ptr<cv::DescriptorMatcher> matcher;
    const float distThreshold;
    const float coordThreshold;
};

#endif /* MatchFeaturePoint_hpp */
