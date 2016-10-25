//
//  main.cpp
//  Test
//
//  Created by masakazu nakazawa on 2016/10/21.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>

#include "Transform.hpp"
#include "Perspective.hpp"
#include "OpticalFlow.hpp"

int main(int argc, const char * argv[])
{
    const cv::Size fs(1000, 500);

    const Transform tf(fs);
    
    const float halfWidth = 200;
    const float halfHeight = 200;
    
    const cv::Mat inParaMat = (cv::Mat_<float>(3, 3) <<
                         halfWidth, 0, halfWidth,
                         0, halfHeight, halfHeight,
                         0, 0, 1);
    
    const float rangeAngle = M_PI/ 4.0;
    Perspective ps(tf, inParaMat, rangeAngle);
    
    OpticalFlow opt(tf, ps);
    
    cv::Point2f forNormal(1.0, 0.4);
    cv::Point2f latNormal(1.0, -0.5);
    
    std::cout << forNormal.dot(latNormal) << std::endl;
    std::cout << forNormal.x*latNormal.x + forNormal.y*latNormal.y
    << std::endl;
    
    float cosAngle = forNormal.dot(latNormal) / (cv::norm(forNormal)*cv::norm(latNormal));
    
    std::cout << cosAngle << std::endl;
    
    opt.isCosCond(forNormal, latNormal);

    
    return 0;
}
