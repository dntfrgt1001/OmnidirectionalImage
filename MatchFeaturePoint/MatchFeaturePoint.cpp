//
//  MatchFeaturePoint.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/05/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "MatchFeaturePoint.hpp"

MatchFeaturePoint::MatchFeaturePoint
(const float eucThre, const float sphereThre):
eucThre(eucThre), sphereThre(sphereThre),
matcher(cv::DescriptorMatcher::create("BruteForce"))
{
}

void MatchFeaturePoint::match
(const cv::Mat& descs1, const cv::Mat& descs2,
 std::vector<cv::DMatch> &dMatches) const
{
    std::vector<cv::DMatch> dMatches1, dMatches2;
    
    matcher->match(descs1, descs2, dMatches1);
    matcher->match(descs2, descs1, dMatches2);
    
    crossMatch(dMatches1, dMatches2, dMatches);
}

void MatchFeaturePoint::crossMatch
(const std::vector<cv::DMatch> &dMatches1,
 const std::vector<cv::DMatch> &dMatches2,
 std::vector<cv::DMatch> &dMatches) const
{
    dMatches.clear();
    
    for (int i = 0; i < dMatches1.size(); i++) {
        cv::DMatch forward = dMatches1[i];
        cv::DMatch backward = dMatches2[forward.trainIdx];
        
        if (forward.queryIdx == backward.trainIdx)
            dMatches.push_back(forward);
    }
}

void MatchFeaturePoint::filterMatchEuc
(std::vector<cv::DMatch> &dMatches) const
{
    for (int i = 0; i < dMatches.size();  ) {
        if (dMatches[i].distance > eucThre)
            dMatches.erase(dMatches.begin() + i);
        else
            i++;
    }
}

void MatchFeaturePoint::filterMatchSphere
(std::vector<Sphere> &forSpheres, std::vector<Sphere> &latSpheres) const
{
    for (int i = 0; i < forSpheres.size(); ) {
        // 球面距離を計算
        const float arc =
        acosf(Map::normalDomain(forSpheres[i].dot(latSpheres[i]), -1, 1));
        //cv::norm(forSpheres[i] - latSpheres[i]);
        if (arc > sphereThre) {
            forSpheres.erase(forSpheres.begin() + i);
            latSpheres.erase(latSpheres.begin() + i);
        } else {
            i++;
        }
    }
}

void MatchFeaturePoint::orderKeyPoint
(const std::vector<cv::KeyPoint> &forKeyPointSet,
 const std::vector<cv::KeyPoint> &latKeyPointSet,
 const std::vector<cv::DMatch> &matchs,
 std::vector<cv::KeyPoint> &forKeyPoints,
 std::vector<cv::KeyPoint> &latKeyPoints) const
{
    forKeyPoints = std::vector<cv::KeyPoint>(matchs.size());
    latKeyPoints = std::vector<cv::KeyPoint>(matchs.size());
    
    for (int i = 0; i < matchs.size(); i++) {
        forKeyPoints[i] = forKeyPointSet[matchs[i].queryIdx];
        latKeyPoints[i] = latKeyPointSet[matchs[i].trainIdx];
    }
}

void MatchFeaturePoint::drawMatchVert
(const cv::Mat &img1, const std::vector<Equirect> &forEquirects,
 const cv::Mat &img2, const std::vector<Equirect> &latEquirects,
 cv::Mat &outImg) const
{
    cv::Mat tmpOutImg;
    cv::vconcat(img1, img2, tmpOutImg);
    
    if (tmpOutImg.channels() == 1) cv::cvtColor(tmpOutImg, outImg, CV_GRAY2BGR);
    else outImg = tmpOutImg.clone();
        
    for (int i = 0; i < forEquirects.size(); i++) {
        drawLineVert(forEquirects[i], latEquirects[i], outImg);
    }
}

void MatchFeaturePoint::drawMatchVert
(const cv::Mat &img1, const std::vector<cv::KeyPoint> keyPoints1,
 const cv::Mat &img2, const std::vector<cv::KeyPoint> keyPoints2,
 const std::vector<cv::DMatch> matchs, cv::Mat &outImg) const
{
    cv::Mat tmpOutImg;
    cv::vconcat(img1, img2, tmpOutImg);
    
    if (tmpOutImg.channels() == 1) cv::cvtColor(tmpOutImg, outImg, CV_GRAY2BGR);
    else outImg = tmpOutImg.clone();
    
    for (int i = 0; i < matchs.size(); i++) {
        drawLineVert(keyPoints1[matchs[i].queryIdx].pt,
                     keyPoints2[matchs[i].trainIdx].pt, outImg);
    }
}

void MatchFeaturePoint::drawLineVert
(const cv::Point2f &point1, const cv::Point2f &point2,
 cv::Mat &outImg) const
{
    // 下側の特徴の座標を修正
    cv::Point2f modPoint2 = point2 + cv::Point2f(0, outImg.rows / 2);
    
    // 線の描画
    cv::line
    (outImg, point1, modPoint2,
     cv::Scalar(rand()%256, rand()%256, rand()%256), 2, CV_AA);
}

