//
//  main.cpp
//  WriteVideo
//
//  Created by masakazu nakazawa on 2016/01/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Rotate.hpp"

int main(int argc, const char * argv[])
{
    std::string workDir = "/Users/masakazu/Documents/Koike lab/product/OmnidirectionalImage/working/";
    std::string inputName = "R0010050.JPG";
    
    const cv::Size frameSize(1000, 500);
    
    cv::Mat input, img;
    
    input = cv::imread(workDir + inputName);
    cv::resize(input, img, frameSize);
    
    std::string outputName = "rotation.mp4";
    Rotate rot(&frameSize, &img);
    
    double deltaChi = M_PI * 1.0/40.0;
    double angWidth = M_PI * 3.0/2.0;
    
    
    std::string dir = workDir + outputName;
    rot.writeRotateYMovie(deltaChi, angWidth, dir, 150);
    
    return 0;
}
