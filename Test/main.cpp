//
//  main.cpp
//  Test
//
//  Created by masakazu nakazawa on 2015/12/31.
//  Copyright © 2015年 masakazu. All rights reserved.
//

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include "Transform.hpp"
#include "Rotate.hpp"
#include "Match.hpp"

int main(int argc, const char * argv[])
{
    std::string workDir = "/Users/masakazu/Desktop/working/";
    std::string inputName = "R0010050.JPG";
    
    const cv::Size frameSize(960, 480);
    
    cv::Mat input, stdImg;
    
    input = cv::imread(workDir + inputName);
    cv::resize(input, stdImg, frameSize);


    Transform transform(frameSize);
    Rotate rotate(frameSize, transform);
    int divNum = 8;
    Match match(frameSize, stdImg, transform, rotate, divNum);
    
    double chiX = M_PI / 6.0;
    double chiY = M_PI / 2.0;
    
    cv::Mat rotXImg(frameSize, CV_8UC3);
    cv::Mat rotXYImg(frameSize, CV_8UC3);
    rotate.rotateXAng(chiX, stdImg, rotXImg);
    rotate.rotateYAng(chiY, rotXImg, rotXYImg);
    
    cv::namedWindow("rotated Image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("rotated Image", rotXYImg);
    
    int matchOrthHeight = transform.dphi2v(M_PI / divNum);
    cv::Rect roi(0, frameSize.height/2 - matchOrthHeight/2,
                 frameSize.width, matchOrthHeight);
    
    cv::Mat stdRoiImg = stdImg(roi);
    cv::Mat rotRoiImg = rotXYImg(roi);
    
    cv::Ptr<cv::FeatureDetector> detector;
    detector = cv::FeatureDetector::create("SIFT");
    cv::Ptr<cv::DescriptorExtractor> extractor;
    extractor = cv::DescriptorExtractor::create("SIFT");
    cv::Ptr<cv::DescriptorMatcher> matcher;
    matcher = cv::DescriptorMatcher::create("BruteForce");
    
    std::vector<cv::KeyPoint> stdRoiKeyPoints;
    cv::Mat stdRoiDescriptor;
    detector->detect(stdRoiImg, stdRoiKeyPoints);
    extractor->compute(stdRoiImg, stdRoiKeyPoints, stdRoiDescriptor);
    
    std::vector<cv::KeyPoint> rotRoiKeyPoints;
    cv::Mat rotRoiDescriptor;
    detector->detect(rotRoiImg, rotRoiKeyPoints);
    extractor->compute(rotRoiImg, rotRoiKeyPoints, rotRoiDescriptor);
    
    std::vector<cv::DMatch> dmatches1, dmatches2, dmatches;
    matcher->match(stdRoiDescriptor, rotRoiDescriptor, dmatches1);
    matcher->match(rotRoiDescriptor, stdRoiDescriptor, dmatches2);
    for (int i=0; i<dmatches1.size(); i++) {
        cv::DMatch forward = dmatches1[i];
        cv::DMatch backward = dmatches2[forward.trainIdx];
        
        if (forward.queryIdx == backward.trainIdx) {
            if (forward.distance < 100) {
                dmatches.push_back(forward);
            }
        }
    }

    std::vector<float> stdVec, rotVec;
    std::vector<float> moveVec;
    for (int i=0; i<dmatches.size(); i++) {
        cv::Point2f pt1 = stdRoiKeyPoints[dmatches[i].queryIdx].pt;
        cv::Point2f pt2 = rotRoiKeyPoints[dmatches[i].trainIdx].pt;
        
        cv::Point2f tmpVec;
        if (pt1.x <= pt2.x) tmpVec = pt2 - pt1;
        else                tmpVec = pt2 - pt1 + cv::Point2f(frameSize.width, 0);
        
        moveVec.push_back(tmpVec.x);
        
        stdVec.push_back(pt1.x);
        rotVec.push_back(pt2.x);
    }
    
    std::sort(moveVec.begin(), moveVec.end());
    
    cv::Mat drawOutput;
    
    cv::drawMatches(stdRoiImg, stdRoiKeyPoints, rotRoiImg, rotRoiKeyPoints,
                    dmatches, drawOutput, cv::Scalar::all(0),
                    cv::Scalar::all(255));
    
    cv::namedWindow("Test", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("Test", drawOutput);
    
    cv::waitKey(-1);
    
    return 0;
}
