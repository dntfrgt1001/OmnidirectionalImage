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
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string inputName = path + "img1.jpg";
    const std::string outputName = path + "key.jpg";
//    const cv::Size frameSize(960, 480);
    const cv::Size frameSize(640, 320);
    
    cv::Mat input, img;
    input = cv::imread(inputName);
    cv::resize(input, img, frameSize);
    
    Transform tf(frameSize);
    
    int divNum = 6;
    ExtractFeaturePoint efp(frameSize, tf, divNum);
    
    /*
    float distThre = 250;
    float coordThre = 0.5;
    MatchFeaturePoint mfp(tf, distThre, coordThre);
    */
    
    std::vector<cv::KeyPoint> keyPoints;
    cv::Mat descriptors;
    efp.extractFeaturePoint(img, keyPoints, descriptors);

    cv::Mat keyPointImg;
    
    cv::drawKeypoints(img, keyPoints, keyPointImg);
    
    cv::namedWindow("key point image");
    cv::imshow("key point image", keyPointImg);

    cv::waitKey(-1);
    
    cv::imwrite(outputName, keyPointImg);
    


    return 0;
}
