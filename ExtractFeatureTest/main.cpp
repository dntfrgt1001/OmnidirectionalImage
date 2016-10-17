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

#include <random>

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/rotation/";
    const std::string inputName = path + "rotimg.jpg";
    const std::string outputName = path + "key2.jpg";
    const cv::Size frameSize(600, 300);
    
    cv::Mat input, img;
    input = cv::imread(inputName);
    cv::resize(input, img, frameSize);
    
    Transform tf(frameSize);
    
    int divNum = 6;
    ExtractFeaturePoint efp(frameSize, tf, divNum);
    
    float distThre = 250;
    float coordThre = 0.5;
    MatchFeaturePoint(frameSize, tf, distThre, coordThre);
    
    std::vector<cv::KeyPoint> keyPoints;
    cv::Mat descriptors;
    efp.extractFeaturePoint(img, keyPoints, descriptors);
    
    


    cv::Mat imgKeyPoints;
    cv::Mat grayImg;
    cv::cvtColor(img, grayImg, CV_BGR2GRAY);
    cv::drawKeypoints(grayImg, keyPoints, imgKeyPoints);
    
    cv::namedWindow("key point image");
    cv::imshow("key point image", imgKeyPoints);

    cv::waitKey(-1);
    
    cv::imwrite(outputName, imgKeyPoints);
    


    return 0;
}
