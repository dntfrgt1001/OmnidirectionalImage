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

void MatchFeaturePoint::filterMatchCoordinateDebug
(std::vector<cv::KeyPoint> &forKeyPoints,
 std::vector<cv::KeyPoint> &latKeyPoints, std::vector<cv::DMatch> &dMatches)
{
    // .eraseに注意
    for (int i=0; i<dMatches.size(); ) {
        cv::Point2f for2DPoint, lat2DPoint;
        cv::Point3f for3DPoint, lat3DPoint;
        
        for2DPoint = forKeyPoints[dMatches[i].queryIdx].pt;
        transform.orth2d2orth3d(for2DPoint, for3DPoint);
        lat2DPoint = latKeyPoints[dMatches[i].trainIdx].pt;
        transform.orth2d2orth3d(lat2DPoint, lat3DPoint);
        
        if (cv::norm(for3DPoint - lat3DPoint) > coordThreshold) {
            dMatches.erase(dMatches.begin() + i);
        } else {
            i++;
        }
    }
}

void MatchFeaturePoint::drawMatchesVertical
(const cv::Mat &img1, const std::vector<cv::KeyPoint> &keyPoints1,
 const cv::Mat &img2, const std::vector<cv::KeyPoint> &keyPoints2,
 const std::vector<cv::DMatch> &dMatches, cv::Mat &outImg)
{
    cv::vconcat(img1, img2, outImg);
    
    for (int i=0; i<dMatches.size(); i++) {
        cv::Point2f dpt1 = keyPoints1[dMatches[i].queryIdx].pt;
        cv::Point2f dpt2 = keyPoints2[dMatches[i].trainIdx].pt;
        // 縦に重ねる分の座標を足す
        dpt2.y = dpt2.y + img1.rows;
        
        cv::line(outImg, dpt1, dpt2, cv::Scalar(rand()%256, rand()%256, rand()%256), 2, CV_AA);
    }
}

void MatchFeaturePoint::drawMatchesVertical
(const cv::Mat &img1, const std::vector<cv::Point2f> &for2DPoints,
 const cv::Mat &img2, const std::vector<cv::Point2f> &lat2DPoints,
 cv::Mat &outImg)
{
    cv::vconcat(img1, img2, outImg);
    
    for (int i=0; i<for2DPoints.size(); i++) {
        cv::Point2f dpt1 = for2DPoints[i];
        cv::Point2f dpt2 = lat2DPoints[i];
        
        dpt2.y = dpt2.y + img1.rows;
        
        cv::line(outImg, dpt1, dpt2, cv::Scalar(rand()%256, rand()%256, rand()%256), 2, CV_AA);
    }
}

void MatchFeaturePoint::sortMatchedPair
(const std::vector<cv::KeyPoint> &forKeyPoints,
 const std::vector<cv::KeyPoint> &latKeyPoints,
 const std::vector<cv::DMatch> &dMatches,
 std::vector<cv::Point2f> &for2DPoints, std::vector<cv::Point2f> &lat2DPoints)
{
    for (auto dMatch: dMatches) {
        for2DPoints.push_back(forKeyPoints[dMatch.queryIdx].pt);
        lat2DPoints.push_back(latKeyPoints[dMatch.trainIdx].pt);
    }
}