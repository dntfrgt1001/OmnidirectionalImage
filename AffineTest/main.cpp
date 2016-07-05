//
//  main.cpp
//  AffineTest
//
//  Created by masakazu nakazawa on 2016/06/02.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/opencv.hpp>
#include "Transform.hpp"
#include "Affine.hpp"
#include "Quarternion.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string fileName = "img1.jpg";
    const cv::Size frameSize(800, 400);
    
    cv::Mat input, img;
    input = cv::imread(path + fileName);

    cv::resize(input, img, frameSize);
  
    float theta = M_PI / 4.0;
    cv::Vec3f axis(0.0, 0.0, 1.0);
    cv::Mat rotMat;
    Quarternion::arbRotMat(theta, axis, rotMat);
    
    Transform transform(frameSize);
    
    cv::Mat rotImg;
    
    transform.rotateVerticalImg(theta, img, rotImg);
//    transform.rotateImgWithRotMat(img, rotImg, rotMat);
    cv::namedWindow("Original Image");
    cv::imshow("Original Image", img);
    
    cv::namedWindow("Rotated Image");
    cv::imshow("Rotated Image", rotImg);
        
    cv::waitKey(-1);
    
    return 0;
}
