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
    static void crossMatch
    (const std::vector<cv::DMatch>& dMatches1,
     const std::vector<cv::DMatch>& dMatches2,
     std::vector<cv::DMatch>& dMatches) ;
    // マッチング距離が閾値以下のものはフィルタリング
    void filterMatchDistance
    (std::vector<cv::DMatch>& dMatches) const;
    // 座標距離が大きく離れているものはフィルタリング
    void filterCoordDistance
    (std::vector<cv::Point3f>& for3DPoints,
     std::vector<cv::Point3f>& lat3DPoints) const;
    // マッチの描画 (画像を縦に並べる)
    static void drawMatchVertical
    (const cv::Mat& img1, const std::vector<cv::KeyPoint>& keyPoints1,
     const cv::Mat& img2, const std::vector<cv::KeyPoint>& keyPoints2,
     const std::vector<cv::DMatch>& dMatches, cv::Mat& outImg);
    // マッチの描画 DMatchなし
    static void drawMatchesVertical
    (const cv::Mat& img1, const std::vector<cv::Point2f>& for2DPoints,
     const cv::Mat& img2, const std::vector<cv::Point2f>& lat2DPoints,
     cv::Mat& outImg);
    static void drawMatchLine
    (const cv::Point2f& forPoint, const cv::Point2f& latPoint, cv::Mat& outImg);
    // 対になるように特徴点を並べ替え，２次元座標のみ出力
    static void sortMatchedPair
    (const std::vector<cv::KeyPoint>& forKeyPoints,
     const std::vector<cv::KeyPoint>& latKeyPoints,
     const std::vector<cv::DMatch>& dMatches,
     std::vector<cv::Point2f>& for2DPoints,
     std::vector<cv::Point2f>& lat2DPoints);
    
private:
    const cv::Size& frameSize;
    const Transform& transform;
    
    const cv::Ptr<cv::DescriptorMatcher> matcher;
    const int distThreshold;
    const float coordThreshold;
};

#endif /* MatchFeaturePoint_hpp */
