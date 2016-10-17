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
#include "Quaternion.hpp"
#include "Rotation.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/rotation/";
    const std::string inputName = path + "img.jpg";
    const std::string outputName = path + "rotimg.jpg";
    
    const cv::Size frameSize(1280, 640);
    
    const Transform tf(frameSize);
    
    cv::Mat input, img;
    input = cv::imread(inputName);
    cv::resize(input, img, frameSize);
    
    cv::Mat rotImg;
    
    float anglez = M_PI/8.0;
    cv::Vec3f axisz(1.0, 1.0, 1.0);
    axisz = axisz / cv::norm(axisz);
    cv::Mat rotMatz;
    Rotation::RotVec2RotMat(anglez * axisz, rotMatz);
    
    float anglex = 0 * M_PI / 6.0;
    cv::Vec3f axisx(1.0, 0.0, 0.0);
    cv::Mat rotMatx;
    Rotation::RotVec2RotMat(anglex * axisx, rotMatx);
    
    float angley = 0 * M_PI / 4.0;
    cv::Vec3f axisy(0.0, 1.0, 0.0);
    cv::Mat rotMaty;
    Rotation::RotVec2RotMat(angley * axisy, rotMaty);
    
    tf.rotateImgWithRotMat(img, rotImg, rotMaty * rotMatx * rotMatz);
    
    cv::namedWindow("original");
    cv::namedWindow("rotated");
    
    cv::imshow("original", img);
    cv::imshow("rotated", rotImg);
    
    cv::waitKey(-1);
    
    cv::imwrite(outputName, rotImg);
    
    return 0;
}
