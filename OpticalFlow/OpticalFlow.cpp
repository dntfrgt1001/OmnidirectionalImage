//
//  OpticalFlow.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/20.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "OpticalFlow.hpp"


void OpticalFlow::getFeatures
(const cv::Mat &img, std::vector<cv::Point2f> &points,
 const cv::Mat &mask) const
{
    cv::Mat grayImg;
    cv::cvtColor(img, grayImg, CV_BGR2GRAY);
    
    cv::goodFeaturesToTrack(grayImg, points, 400, 0.01, 3, mask);
}

void OpticalFlow::getOpticalFlow
(const cv::Mat &forPersImg, const cv::Mat &latPersImg,
 const std::vector<cv::Point2f> &forPoints,
 std::vector<cv::Point2f> &latPoints) const
{
    cv::Mat forGrayImg, latGrayImg;
    cv::cvtColor(forPersImg, forGrayImg, CV_BGR2GRAY);
    cv::cvtColor(latPersImg, latGrayImg, CV_BGR2GRAY);
    
    std::vector<unsigned char> statuss;
    std::vector<float> errors;
    
    cv::calcOpticalFlowPyrLK
    (forGrayImg, latGrayImg, forPoints, latPoints,
     statuss, errors);
}

void OpticalFlow::drawPoint
(const cv::Mat &persImg, const std::vector<cv::Point2f> &points,
 cv::Mat &outImg)
{
    outImg = persImg.clone();
    
    for (int i = 0; i < points.size(); i++) {
        cv::circle(outImg, points[i], 5, cv::Scalar(-1));
    }
}

void OpticalFlow::drawOpticalFlow
(const cv::Mat &persImg, const std::vector<cv::Point2f> &forPoints,
 const std::vector<cv::Point2f> &latPoints, cv::Mat &outImg)
{
    outImg = persImg.clone();
    
    for (int i = 0; i < forPoints.size(); i++) {
        cv::line(outImg, forPoints[i], latPoints[i],
                 cv::Scalar(-1,-1,-1), 2);
        cv::circle(outImg, latPoints[i], 5, cv::Scalar(-1));
    }
}
