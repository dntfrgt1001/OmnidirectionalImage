//
//  main.cpp
//  ExtractFeatureTest
//
//  Created by masakazu nakazawa on 2016/05/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Quarternion.hpp"

#include <random>

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string inputName = path + "test.jpg";
    const std::string outputName = path + "key1.jpg";
    const cv::Size frameSize(1000, 500);
    
    cv::Mat input, img;
    input = cv::imread(inputName);
    cv::resize(input, img, frameSize);
    
    Transform tf(frameSize);
    
    int divNum = 6;
    ExtractFeaturePoint efp(frameSize, tf, divNum);
    
    std::vector<cv::KeyPoint> keyPoints;
    cv::Mat descriptors;
    efp.extractFeaturePoint(img, keyPoints, descriptors);
    
    cv::Mat imgKeyPoints;
    cv::drawKeypoints(img, keyPoints, imgKeyPoints);
    
    cv::namedWindow("key point image");
    cv::imshow("key point image", imgKeyPoints);
    
    cv::imwrite(outputName, imgKeyPoints);
    
    cv::waitKey(-1);

    return 0;
}
