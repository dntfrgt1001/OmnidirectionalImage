//
//  main.cpp
//  AffineTest
//
//  Created by masakazu nakazawa on 2016/06/02.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string fileName = "R0010052.JPG";
    const cv::Size frameSize(1000, 500);

    const Transform transform(frameSize);
    const ExtractFeaturePoint ef(frameSize, transform, 6);
    
    cv::Mat input, img;
    
    input = cv::imread(path + fileName);

    cv::resize(input, img, frameSize);
  
    float theta = M_PI / 4.0;
    cv::Mat rotImg;
    cv::Rect rect(0, frameSize.height/3, frameSize.width, frameSize.height/3);
    transform.rotateVerticalImgRect(theta, img, rect, rotImg);

    std::vector<cv::KeyPoint> keyPoints;
    cv::Mat descriptors;
    ef.extractFeaturePoint(img, keyPoints, descriptors);
    
    cv::Mat keyImg;
    cv::drawKeypoints(img, keyPoints, keyImg);
    
    cv::namedWindow("Original Image");
    cv::imshow("Original Image", img);
    
    cv::namedWindow("KeyPoint Image");
    cv::imshow("KeyPoint Image", keyImg);
        
    cv::waitKey(-1);
    
    return 0;
}
