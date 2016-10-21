//
//  main.cpp
//  PerspectiveProjection
//
//  Created by masakazu nakazawa on 2016/07/13.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "Transform.hpp"
#include "Perspective.hpp"
#include "Rotation.hpp"
#include "OpticalFlow.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string inputName1 = path + "image1.jpg";
    const std::string inputName2 = path + "image2.jpg";
    
    const cv::Size frameSize(5376, 2688);
    
    Transform tf(frameSize);
    
    cv::Mat input1, img1, input2, img2;
    input1 = cv::imread(inputName1);
    cv::resize(input1, img1, frameSize);
    input2 = cv::imread(inputName2);
    cv::resize(input2, img2, frameSize);
    
    float halfWidth = 200;
    float halfHeight = 200;
    
    cv::Mat inParaMat = (cv::Mat_<float>(3, 3) <<
                                    halfWidth, 0, halfWidth,
                                    0, halfHeight, halfHeight,
                                    0, 0, 1);
    
    const float rangeAngle = M_PI / 4.0;
    Perspective pers(tf, inParaMat, rangeAngle);

    const float angle = M_PI/2.0;
    const cv::Vec3f rotVec = angle * cv::Vec3f(0.0, 1.0, 0.0);
    cv::Mat rotMat;
    Rotation::RotVec2RotMat(rotVec, rotMat);
    
    cv::Mat persImg1, persImg2;
    const bool isFront = true;
    pers.getPersImg(img1, persImg1, rotMat, isFront);
    pers.getPersImg(img2, persImg2, rotMat, isFront);
    
    cv::namedWindow("pers 1");
    cv::namedWindow("pers 2");
    cv::imshow("pers 1", persImg1);
    cv::imshow("pers 2", persImg2);
    
    
    OpticalFlow opf(tf, pers);
    std::vector<cv::Point2f> forPoints, latPoints;
    
    cv::Mat mask;
    pers.getMask(mask);
    opf.getFeatures(persImg1, forPoints, mask);
    
    cv::Mat keyImg;
    opf.drawPoint(persImg1, forPoints, keyImg);
    cv::namedWindow("key");
    cv::imshow("key", keyImg);
    
    opf.getOpticalFlow(persImg1, persImg2, forPoints, latPoints);
    
    cv::Mat optImg;
    opf.drawOpticalFlow(persImg1, forPoints, latPoints, optImg);
    
    cv::namedWindow("opt");
    cv::imshow("opt", optImg);
    
//    const std::string outName = "5.jpg";
//    cv::imwrite(path + outName, persImg);
    
    cv::waitKey(-1);
    
    return 0;
}
