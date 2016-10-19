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

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string inputName = path + "img.jpg";
    
    const cv::Size frameSize(5376, 2688);
    
    Transform tf(frameSize);
    
    cv::Mat input, img;
    input = cv::imread(inputName);
    cv::resize(input, img, frameSize);
    
    float halfWidth = 200;
    float halfHeight = 200;
    
    cv::Mat inParaMat = (cv::Mat_<float>(3, 3) <<
                                    halfWidth, 0, halfWidth,
                                    0, halfHeight, halfHeight,
                                    0, 0, 1);
    
    const float rangeAngle = M_PI / 4.0;
    Perspective pers(tf, inParaMat, rangeAngle);
    

    cv::Mat persImg;
    const float angle = M_PI/2.0;
    const cv::Vec3f rotVec = angle * cv::Vec3f(1.0, 0.0, 0.0);
    cv::Mat rotMat;
    Rotation::RotVec2RotMat(rotVec, rotMat);

    const bool isFront = true;
    pers.getPersImg(img, persImg, rotMat, isFront);
    
    //    float rangeTheta = M_PI / 1.5;
//    float rangePhi = M_PI / 3.0;
//    pers.persProjImg(img, rangeTheta, rangePhi, persImg);
    
    cv::namedWindow("Perspective");
    cv::imshow("Perspective", persImg);
    
//    const std::string outName = "5.jpg";
//    cv::imwrite(path + outName, persImg);
    
    cv::waitKey(-1);
    
    return 0;
}
