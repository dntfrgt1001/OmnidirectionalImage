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
#include "Core.hpp"

#include <random>

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/TestImage/";
    const std::string inputName = path + "hall.jpg";
    const std::string outputName = path + "key.jpg";
//    const cv::Size frameSize(960, 480);
//    const cv::Size frameSize(640, 320);
    const cv::Size frameSize(800, 400);
    
    cv::Mat input, img, grayimg;
    input = cv::imread(inputName);
    cv::resize(input, img, frameSize);
    
    Map::fs = frameSize;
    
    const int divNum = 6;
    const int mergin = 10;
    ExtractFeaturePoint efp(frameSize, divNum, mergin);
    
    /*
    float distThre = 250;
    float coordThre = 0.5;
    MatchFeaturePoint mfp(tf, distThre, coordThre);
    */
    
    std::vector<cv::KeyPoint> keyPoints;
    cv::Mat descriptors;
    efp.extFeat(img, keyPoints, descriptors);

    cv::Mat keyPointImg;
    
    cv::drawKeypoints(img, keyPoints, keyPointImg);
    
    cv::namedWindow("key point image");
    cv::imshow("key point image", keyPointImg);

    cv::waitKey(-1);
    
    cv::imwrite(outputName, keyPointImg);
    


    return 0;
}
