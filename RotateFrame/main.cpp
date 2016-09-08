//
//  main.cpp
//  RotateFrame
//
//  Created by masakazu nakazawa on 2016/09/02.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "Transform.hpp"
#include "Quarternion.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/PIXPRO/video8/sample2/";
    const std::string inputName = path + "image0000.jpg";
    const std::string outputName = path + "rot.jpg";
    
    const cv::Size frameSize(1280, 640);
    
    const Transform tf(frameSize);
    
    cv::Mat input, img;
    input = cv::imread(inputName);
    cv::resize(input, img, frameSize);
    
    cv::Mat rotImg;
    
    float anglez = -M_PI/2.0;
    cv::Vec3f axisz(0.0, 0.0, 1.0);
    cv::Mat rotMatz;
    Quarternion::Rodrigues2RotMat(anglez, axisz, rotMatz);
    
    float anglex = M_PI / 9.0;
    cv::Vec3f axisx(1.0, 0.0, 0.0);
    cv::Mat rotMatx;
    Quarternion::Rodrigues2RotMat(anglex, axisx, rotMatx);
    
    float angley = M_PI / 4.0;
    cv::Vec3f axisy(0.0, 1.0, 0.0);
    cv::Mat rotMaty;
    Quarternion::Rodrigues2RotMat(angley, axisy, rotMaty);
    
    tf.rotateImgWithRotMat(img, rotImg, rotMaty * rotMatx * rotMatz);
    
    cv::namedWindow("original");
    cv::namedWindow("rotated");
    
    cv::imshow("original", img);
    cv::imshow("rotated", rotImg);
    
    cv::waitKey(-1);
    
    return 0;
}
