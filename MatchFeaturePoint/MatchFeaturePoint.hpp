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

//#include "Transform.hpp"
#include "Core.hpp"

class MatchFeaturePoint
{
public:
    MatchFeaturePoint(const float eucThre, const float sphereThre);
    
    // マッチング
    void match
    (const cv::Mat& descs1, const cv::Mat& descs2,
     std::vector<cv::DMatch>& dMatches) const;
   
    // クロスマッチング
    void crossMatch
    (const std::vector<cv::DMatch>& dMatches1,
     const std::vector<cv::DMatch>& dMatches2,
     std::vector<cv::DMatch>& dMatches) const;
    
    // 球面距離が大きく離れているマッチングは除去
    void filterMatchEuc
    (std::vector<cv::DMatch>& dMatches) const;
    
    // 記述子のユークリッド距離が大きく離れているものは削除
    void filterMatchSphere
    (std::vector<Sphere>& forSpheres,
     std::vector<Sphere>& latSpheres) const;
    
    // 特徴点を対応するように並べ替える
    void orderKeyPoint
    (const std::vector<cv::KeyPoint>& forKeyPointSet,
     const std::vector<cv::KeyPoint>& latKeyPointSet,
     const std::vector<cv::DMatch>& matchs,
     std::vector<cv::KeyPoint>& forKeyPoints,
     std::vector<cv::KeyPoint>& latKeyPoints) const;
    
    // 画像座標でマッチングを表示
    void drawMatchVert
    (const cv::Mat& img1, const std::vector<Equirect>& forEquirects,
     const cv::Mat& img2, const std::vector<Equirect>& latEquirects,
     cv::Mat& outImg) const;
    
    // キーポイントでマッチングを表示
    void drawMatchVert
    (const cv::Mat& img1, const std::vector<cv::KeyPoint> keyPoints1,
     const cv::Mat& img2, const std::vector<cv::KeyPoint> keyPoints2,
     const std::vector<cv::DMatch> matchs, cv::Mat& outImg) const;

    void drawLineVert
    (const cv::Point2f& point1, const cv::Point2f& point2,
     cv::Mat& outImg) const;
    
private:
    const cv::Ptr<cv::DescriptorMatcher> matcher; // マッチャ
    const float eucThre;     // 記述子のユークリッド距離のフィルタ
    const float sphereThre;  // 球面距離のフィルタ
};

#endif /* MatchFeaturePoint_hpp */
