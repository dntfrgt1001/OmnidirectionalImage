//
//  main.cpp
//  MatchTest
//
//  Created by masakazu nakazawa on 2016/01/09.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/calib3d.hpp>

#include "Transform.hpp"
#include "MatchMain.hpp"
#include "Rotation.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Quarternion.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string inputName = "R0010018.JPG";

    const cv::Size origFrameSize(1000, 500);
    const cv::Size frameSize(1000, 500);
    
    cv::Mat input1, img1;
    input1 = cv::imread(path + inputName);
    cv::resize(input1, img1, frameSize);
    
    const Transform transform(frameSize);
    
    int divNum = 6;
    ExtractFeaturePoint ef(frameSize, transform, divNum);
    int distThreshold = 200;
    float coordThreshold = 0.5;
    MatchFeaturePoint mf(frameSize, transform, distThreshold, coordThreshold);
    Rotation rot(transform);
    
    cv::Vec3f axis(1, 1, 1);
    float theta = M_PI/5.0;
    cv::Mat rotMat;
    Quarternion::arbRotMat(theta, axis, rotMat);
    
    cv::Mat img2;
    transform.rotateImgWithRotMat(img1, img2, rotMat);
    
    MatchMain mm(origFrameSize, transform, ef, mf, rot);
    
    cv::Mat modImg;
    mm.ModifylatterImg(img1, img2, modImg);
    
    cv::namedWindow("img1");
    cv::namedWindow("img2");
    cv::namedWindow("mod");
    
    cv::imshow("img1", img1);
    cv::imshow("img2", img2);
    cv::imshow("mod", modImg);
    
    cv::waitKey(-1);
    
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
