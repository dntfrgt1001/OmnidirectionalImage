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

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string imgName = "R0010023.JPG";
    
    const cv::Size frameSize(5000, 2500);
    
    Transform transform(frameSize);
    
    cv::Mat input = cv::imread(path + imgName);
    cv::Mat img;
    cv::resize(input, img, frameSize);
    
    int halfWidth = 200;
    int halfHeight = 200;
    
    cv::Mat cameraMat = (cv::Mat_<float>(3, 3) <<
                                    halfWidth, 0, halfWidth,
                                    0, halfHeight, halfHeight,
                                    0, 0, 1);
    
    Perspective perspective(transform, cameraMat);
    
    cv::Mat persedImg;
    float rangeTheta = M_PI / 1.5;
    float rangePhi = M_PI / 3.0;
    perspective.persProjImg(img, rangeTheta, rangePhi, persedImg);
    
    
    cv::namedWindow("Perspective");
    cv::imshow("Perspective", persedImg);
    
    const std::string outName = "5.jpg";
    cv::imwrite(path + outName, persedImg);
    cv::waitKey(-1);
    
    return 0;
}
