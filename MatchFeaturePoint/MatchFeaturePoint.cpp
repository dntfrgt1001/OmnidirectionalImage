//
//  MatchFeaturePoint.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/05/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "MatchFeaturePoint.hpp"

MatchFeaturePoint::MatchFeaturePoint
(const Transform& transform, const float distThreshold,
 const float coordThreshold):
transform(transform),
distThreshold(distThreshold), coordThreshold(coordThreshold),
matcher(cv::DescriptorMatcher::create("BruteForce"))
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
 std::vector<cv::DMatch> &dMatches)
{
    dMatches.clear();
    
    for (int i = 0; i < dMatches1.size(); i++) {
        cv::DMatch forward = dMatches1[i];
        cv::DMatch backward = dMatches2[forward.trainIdx];
        
        if (forward.queryIdx == backward.trainIdx)
            dMatches.push_back(forward);
    }
}

void MatchFeaturePoint::filterMatchDistance
(std::vector<cv::DMatch> &dMatches) const
{
    // .eraseに注意
    for (int i = 0; i < dMatches.size();  ) {
        if (dMatches[i].distance > distThreshold)
            dMatches.erase(dMatches.begin() + i);
        else
            i++;
    }
}

void MatchFeaturePoint::filterCoordDistance
(std::vector<Sphere> &forSpheres, std::vector<Sphere> &latSpheres) const
{
    // .eraseに注意
    for (int i = 0; i < forSpheres.size(); ) {
        if (cv::norm(forSpheres[i] - latSpheres[i]) > coordThreshold) {
            forSpheres.erase(forSpheres.begin() + i);
            latSpheres.erase(latSpheres.begin() + i);
        } else {
            i++;
        }
    }
}

void MatchFeaturePoint::drawMatchVert
(const cv::Mat &img1, const std::vector<Equirect> &forEquirects,
 const cv::Mat &img2, const std::vector<Equirect> &latEquirects,
 cv::Mat &outImg)
{
    cv::Mat tmpOutImg;
    cv::vconcat(img1, img2, tmpOutImg);
    
    if (outImg.channels() == 1) Transform::changeChannel(tmpOutImg, outImg);
    
    for (int i = 0; i < forEquirects.size(); i++) {
        drawLineVert(forEquirects[i], latEquirects[i], outImg);
    }
}

void MatchFeaturePoint::drawMatchVert
(const cv::Mat &img1, const std::vector<cv::KeyPoint> keyPoints1,
 const cv::Mat &img2, const std::vector<cv::KeyPoint> keyPoints2,
 const std::vector<cv::DMatch> matchs, cv::Mat &outImg)
{
    cv::Mat tmpOutImg;
    cv::vconcat(img1, img2, tmpOutImg);
    
    if (outImg.channels() == 1) Transform::changeChannel(tmpOutImg, outImg);
    
    for (int i = 0; i < matchs.size(); i++) {
        drawLineVert(keyPoints1[matchs[i].queryIdx].pt,
                     keyPoints2[matchs[i].trainIdx].pt, outImg);
    }
}

void MatchFeaturePoint::drawLineVert
(const cv::Point2f &point1, const cv::Point2f &point2, cv::Mat &outImg)
{
    // 下側の特徴の座標を修正
    cv::Point2f modPoint2 = point2 + cv::Point2f(0, outImg.rows / 2);
    
    // 線の描画
    cv::line
    (outImg, point1, modPoint2,
     cv::Scalar(rand()%256, rand()%256, rand()%256), 2, CV_AA);
}

void MatchFeaturePoint::getMatchKeyPoint
(const std::vector<cv::KeyPoint> &forKeyPointsSet,
 const std::vector<cv::KeyPoint> &latKeyPointsSet,
 const std::vector<cv::DMatch> &matchs,
 std::vector<cv::KeyPoint> &forKeyPoints,
 std::vector<cv::KeyPoint> &latKeyPoints) const
{
    for (int i = 0; i < matchs.size(); i++) {
        forKeyPoints.push_back(forKeyPointsSet[matchs[i].queryIdx]);
        latKeyPoints.push_back(latKeyPointsSet[matchs[i].trainIdx]);
    }
}
