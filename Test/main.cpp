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
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Affine.hpp"
#include "MatchMain.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/phi/";
    const std::string imgName1 = "phi1.jpg";
    const std::string imgName2 = "phi2.jpg";
    
    const cv::Size frameSize(1920, 960);
    
    Transform transform(frameSize);
    
    int divNum = 4;
    ExtractFeaturePoint extractFeature(frameSize, transform, divNum);
    
    int distThreshold = 150;
    float coordThreshold = 0.6;
    MatchFeaturePoint
    matchFeature(frameSize, transform, distThreshold, coordThreshold);
    
    Affine affine(transform);
    
    MatchMain matchMain(transform, extractFeature, matchFeature, affine);
    
    cv::Mat input1, input2;
    cv::Mat img1, img2;
    input1 = cv::imread(path + imgName1);
    cv::resize(input1, img1, frameSize);
    input2 = cv::imread(path + imgName2);
    cv::resize(input2, img2, frameSize);
    
    cv::Mat modImg2;
    
    matchMain.ModifylatterImg(img1, img2, modImg2);
    
    cv::namedWindow("img1");
    cv::namedWindow("img2");
    cv::namedWindow("mod img2");
    
    cv::imshow("img1", img1);
    cv::imshow("img2", img2);
    cv::imshow("mod img2", modImg2);
  
    cv::waitKey(-1);
    
//    cv::imwrite(path + "modOutImg1.jpg", modImg2);
    
//    cv::waitKey(-1);
    
    return 0;
}
