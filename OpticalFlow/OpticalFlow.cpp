//
//  OpticalFlow.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/20.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "OpticalFlow.hpp"


void OpticalFlow::getFeatures
(const cv::Mat &img, std::vector<cv::Point2f> &pers,
 const cv::Mat &mask) const
{
    cv::Mat grayImg;
    cv::cvtColor(img, grayImg, CV_BGR2GRAY);
    
    cv::goodFeaturesToTrack(grayImg, pers, 400, 0.01, 3, mask);
}

void OpticalFlow::getOpticalFlow
(const cv::Mat &forPersImg, const cv::Mat &latPersImg,
 const std::vector<cv::Point2f> &forPerss,
 std::vector<cv::Point2f> &latPerss) const
{
    cv::Mat forGrayImg, latGrayImg;
    cv::cvtColor(forPersImg, forGrayImg, CV_BGR2GRAY);
    cv::cvtColor(latPersImg, latGrayImg, CV_BGR2GRAY);
    
    std::vector<unsigned char> statuss;
    std::vector<float> errors;
    
    cv::calcOpticalFlowPyrLK
    (forGrayImg, latGrayImg, forPerss, latPerss,
     statuss, errors);
}

void OpticalFlow::removeOutlier
(std::vector<cv::Point2f> &forPerss,
 std::vector<cv::Point2f> &latPerss) const
{
    for (int i = 0; i < forPerss.size(); ) {
        if (isOutlier(forPerss[i], latPerss[i])) {
            forPerss.erase(forPerss.begin() + i);
            latPerss.erase(latPerss.begin() + i);
        } else {
            i++;
        }
    }
}

void OpticalFlow::drawPoint
(const cv::Mat &persImg, const std::vector<cv::Point2f> &perss,
 cv::Mat &outImg)
{
    outImg = persImg.clone();
    
    for (int i = 0; i < perss.size(); i++) {
        cv::circle(outImg, perss[i], 5, cv::Scalar(-1));
    }
}

void OpticalFlow::drawOpticalFlow
(const cv::Mat &persImg, const std::vector<cv::Point2f> &forPerss,
 const std::vector<cv::Point2f> &latPerss, cv::Mat &outImg)
{
    outImg = persImg.clone();
    
    for (int i = 0; i < forPerss.size(); i++) {
        cv::line(outImg, forPerss[i], latPerss[i],
                 cv::Scalar(-1,-1,-1), 2);
        cv::circle(outImg, latPerss[i], 5, cv::Scalar(-1));
    }
}
