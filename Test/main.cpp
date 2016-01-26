//
//  main.cpp
//  Test
//
//  Created by masakazu nakazawa on 2015/12/31.
//  Copyright © 2015年 masakazu. All rights reserved.
//

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Transform.hpp"
#include "Rotate.hpp"
#include "Match.hpp"

int main(int argc, const char * argv[])
{
    std::string workDir = "/Users/masakazu/Documents/Koike lab/product/OmnidirectionalImage/working/";
    std::string inputName = "R0010050.JPG";
    
    const cv::Size frameSize(960, 480);
    
    cv::Mat input, img;
    
    input = cv::imread(workDir + inputName);
    cv::resize(input, img, frameSize);

    cv::Mat maskImg(frameSize, CV_8UC3);

    Transform transform(frameSize);
    Rotate rotate(frameSize, transform);
    double matchRange = M_PI / 6.0;
    Match match(frameSize, img, transform, rotate, matchRange);
    
    match.lowLatitudeMask(img, maskImg);
    
    cv::namedWindow("mask", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("mask", maskImg);
    
    cv::waitKey(-1);
    
    return 0;
}
