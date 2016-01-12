//
//  Match.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/01/11.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Match.hpp"
#include <iostream>

Match::Match(const cv::Size* frameSize, cv::Mat* stdImg, Rotate* rot,
             int matchRange):
frameSize(frameSize), stdImg(stdImg), rot(rot), matchRange(matchRange)
{
    roi = cv::Rect(0, frameSize->height/2 - matchRange/2,
                   frameSize->width, matchRange);
    
    cv::Mat stdRoiImg = (*stdImg)(roi);
    
    cv::Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create("SIFT");
    detector->detect(stdRoiImg, stdKeyPoints);
    
    cv::Ptr<cv::DescriptorExtractor> extractor = cv::DescriptorExtractor::create("SIFT");
    extractor->compute(stdRoiImg, stdKeyPoints, stdDescriptor);
}

void Match::rotateYMatch(const cv::Mat &img, cv::Mat &rotImg)
{
    const cv::Mat roiImg = img(roi);
    
    cv::Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create("SIFT");
    cv::vector<cv::KeyPoint> keyPoints;
    detector->detect(roiImg, keyPoints);
    
    cv::Ptr<cv::DescriptorExtractor> extractor = cv::DescriptorExtractor::create("SIFT");
    cv::Mat descriptor;
    extractor->compute(roiImg, keyPoints, descriptor);
    
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce");
    cv::vector<cv::DMatch> dmatch1, dmatch2, dmatch;
    matcher->match(stdDescriptor, descriptor, dmatch1);
    matcher->match(descriptor, stdDescriptor, dmatch2);
    
    crossMatch(dmatch1, dmatch2, dmatch);
    
    cv::Point2f avePoint(0.0, 0.0);
    for (int i=0; i<dmatch.size(); i++) {
        cv::Point2f pt1 = stdKeyPoints[dmatch[i].queryIdx].pt;
        cv::Point2f pt2 = keyPoints[dmatch[i].trainIdx].pt;
        
        if (pt1.x <= pt2.x) avePoint += pt2 - pt1;
        else avePoint += cv::Point2f(frameSize->width, 0.0) + pt2 - pt1;
    }
    
    avePoint *= 1.0 / dmatch.size();
    
    int count = 0;
    cv::Point2f avePoint2(0.0, 0.0);
    for (int i=0; i<dmatch.size(); i++) {
        cv::Point2f pt1 = stdKeyPoints[dmatch[i].queryIdx].pt;
        cv::Point2f pt2 = keyPoints[dmatch[i].trainIdx].pt;
        
        cv::Point2f tmpPoint;
        if (pt1.x <= pt2.x) tmpPoint = pt2 - pt1;
        else tmpPoint += cv::Point2f(frameSize->width, 0.0) + pt2 - pt1;
        
        int thre = 2;
        if (tmpPoint.x * thre < avePoint.x || avePoint.x * thre < tmpPoint.x) ;
        else {avePoint2 += tmpPoint; count++;}
    }
    
    avePoint2 *= 1.0 / count;
    
    std::cout << "delta = " << avePoint2 << ", number = " << dmatch.size() << std::endl;
    
    rot->rotateYOrth((-1) * round(avePoint2.x), img, rotImg);
}

void Match::crossMatch(cv::vector<cv::DMatch> &dmatch1, cv::vector<cv::DMatch> &dmatch2, cv::vector<cv::DMatch> &crossMatch)
{
    for (int i=0; i<dmatch1.size(); i++) {
        cv::DMatch forward = dmatch1[i];
        cv::DMatch backward = dmatch2[forward.trainIdx];
        
        if (forward.queryIdx == backward.trainIdx) {
            crossMatch.push_back(forward);
        }
    }
}



