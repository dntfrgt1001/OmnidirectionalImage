//
//  main.cpp
//  ExtractFeatureTest
//
//  Created by masakazu nakazawa on 2016/05/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Transform.hpp"
#include "Rotate.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"

int main(int argc, const char * argv[])
{
    const std::string workDir = "/Users/masakazu/Desktop/";
    const std::string imgName1 = "img1.jpg";
    const std::string imgName2 = "img1.jpg";
    const cv::Size frameSize(1000, 500);
    
    cv::Mat input1, input2;
    cv::Mat img1, img2;
    input1 = cv::imread(workDir + imgName1);
    cv::resize(input1, img1, frameSize);
    input2 = cv::imread(workDir + imgName2);
    cv::resize(input2, img2, frameSize);
    
    Transform transform(frameSize);
    Rotate rotate(frameSize, transform);
    MatchFeaturePoint matchFeature(frameSize);
    
    int divNum = 8;
    ExtractFeaturePoint extractFeature(frameSize, divNum, transform, rotate);
    
    extractFeature.setImage(img1);
    std::vector<cv::KeyPoint> keyPoints1;
    cv::Mat descriptors1;
//    extractFeature.extractFeaturePoint(keyPoints1, descriptors1);
    extractFeature.extractRoiFeaturePoint(divNum/2, keyPoints1, descriptors1);
    
    extractFeature.setImage(img2);
    std::vector<cv::KeyPoint> keyPoints2;
    cv::Mat descriptors2;
//    extractFeature.extractFeaturePoint(keyPoints2, descriptors2);
    extractFeature.extractRoiFeaturePoint(divNum/2, keyPoints2, descriptors2);
    
    std::vector<cv::DMatch> dMatches;
    matchFeature.match(descriptors1, descriptors2, dMatches);
    
    std::vector<cv::DMatch> dMatchesSub;
    for (int i=0; i<dMatches.size(); i++) {
        if (dMatches[i].distance < 50) {
            dMatchesSub.push_back(dMatches[i]);
        }
    }
    
    cv::Mat drawMatchImg;
    cv::drawMatches
    (img1, keyPoints1, img2, keyPoints2, dMatchesSub, drawMatchImg,
     cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    
    cv::namedWindow("match");
    cv::imshow("match", drawMatchImg);
    
    cv::waitKey(-1);
    
    return 0;
}
