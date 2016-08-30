//
//  main.cpp
//  FrameMatchTest
//
//  Created by masakazu nakazawa on 2016/08/18.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "MatchFeaturePoint.hpp"
#include "Rotate.hpp"
#include "MatchMain.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string inputName1 = path + "test.jpg";
    const std::string inputName2 = path + "test_rot.jpg";
    
    const cv::Size frameSize(800, 400);
    
    const Transform tf(frameSize);
    
    int divNum = 6;
    ExtractFeaturePoint eft(frameSize, tf, divNum);
    
    float matchThres = 200;
    float coordThres = 0.3;
    
    MatchFeaturePoint mfp(frameSize, tf, matchThres, coordThres);
    
    float angle = M_PI / 3.0;
    int matchThre = 20;
    Rotation rot(tf, angle, matchThre);
    
    MatchMain mm(tf, tf, eft, mfp, rot);
    
    cv::Mat input1, input2, img1, img2;
    input1 = cv::imread(inputName1);
    cv::resize(input1, img1, frameSize);
    input2 = cv::imread(inputName2);
    cv::resize(input2, img2, frameSize);
    
    cv::Mat modImg;
    mm.ModifylatterImg(img1, img2, modImg);
    
    cv::namedWindow("img1");
    cv::namedWindow("img2");
    cv::namedWindow("mod img2");
    cv::imshow("img1", img1);
    cv::imshow("img2", img2);
    cv::imshow("mod img2", modImg);
    
    cv::waitKey(-1);
    
    return 0;
}
