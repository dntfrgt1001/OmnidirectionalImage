//
//  main.cpp
//  MatchTest
//
//  Created by masakazu nakazawa on 2016/01/09.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <math.h>
#include "Rotate.hpp"
#include "Transform.hpp"
#include "Match.hpp"

int main(int argc, const char * argv[])
{
    std::string workDir = "/Users/masakazu/Documents/Koike lab/product/OmnidirectionalImage/working/";
    std::string inputName = "R0010050.JPG";
    
    const cv::Size frameSize(1280, 640);
    
    cv::Mat input, img;
    input = cv::imread(workDir + inputName);
    cv::resize(input, img, frameSize);
    
    Transform transform(frameSize);
    
    Rotate rot(frameSize, transform);
    
    cv::Mat img2(frameSize, CV_8UC3);
    rot.rotateXAng(M_PI * 1.0/8.0, img, img2);
    
    Match match(frameSize, img, transform, rot, 6);
    
    cv::Mat rotImg(frameSize, CV_8UC3);
    match.rotateXMatch(img2, rotImg);
    
    
    /*
    cv::Mat modImg(frameSize, CV_8UC3);
    match.rotateYMatch(img2, modImg);

    
    cv::namedWindow("standard image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("query image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("modified image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    
    cv::imshow("standard image", img);
    cv::imshow("query image", img2);
    cv::imshow("modified image", modImg);
    */
    /*
    
    cv::Rect roi(0, frameSize.height/2 - orthRange/2,
                 frameSize.width, orthRange);
    
    cv::Mat roi1 = img(roi);
    cv::Mat roi2 = img2(roi);
    
//    cv::initModule_nonfree();
    
    cv::Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create("SIFT");
    
    cv::vector<cv::KeyPoint> keyPoints1, keyPoints2;
    
    detector->detect(roi1, keyPoints1);
    detector->detect(roi2, keyPoints2);
    
    cv::Ptr<cv::DescriptorExtractor> extractor = cv::DescriptorExtractor::create("SIFT");
    
    cv::Mat descriptor1, descriptor2;
    
    extractor->compute(roi1, keyPoints1, descriptor1);
    extractor->compute(roi2, keyPoints2, descriptor2);
    
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce");
    
    cv::vector<cv::DMatch> dmatch12, dmatch21, dmatch;
    
    matcher->match(descriptor1, descriptor2, dmatch12);
    matcher->match(descriptor2, descriptor1, dmatch21);
    
    for (int i=0; i<dmatch12.size(); i++) {
        cv::DMatch forward = dmatch12[i];
        cv::DMatch backward = dmatch21[forward.trainIdx];
        
        if (backward.trainIdx == forward.queryIdx) {
            dmatch.push_back(forward);
        }
    }
    
    std::cout << "match number = " << dmatch.size() << std::endl;
    
    int count = 0;
    cv::Point2f avePoint(0.0, 0.0);
    for (int i=0; i<dmatch.size(); i++) {
        cv::Point2f pt1 = keyPoints1[dmatch[i].queryIdx].pt;
        cv::Point2f pt2 = keyPoints2[dmatch[i].trainIdx].pt;
        
        count++;
        if (pt1.x <= pt2.x) {
            avePoint += pt2 - pt1;
        }else{
            avePoint += cv::Point2f(frameSize.width, 0.0) + pt2 - pt1;
        }
    }
    avePoint = avePoint * (1.0/count);
    
    
    std::cout << "delta Point = " << avePoint << std::endl;
    
    cv::Mat result;
    
//    cv::drawKeypoints(roi1, keyPoints1, roi1);
    
    
    cv::drawMatches(roi1, keyPoints1, roi2, keyPoints2, dmatch, result);
    
    std::cout << "number of keypoint is " << keyPoints1.size() << std::endl;
    
    
    cv::namedWindow("result");
    cv::imshow("result", result);
    */
    
    return 0;
}
