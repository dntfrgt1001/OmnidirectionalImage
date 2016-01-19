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
#include "Transform.hpp"

int main(int argc, const char * argv[])
{
    std::string workDir = "/Users/masakazu/Documents/Koike lab/product/OmnidirectionalImage/working/";
    std::string inputName = "R0010050.JPG";
    
    const cv::Size frameSize(1920, 960);
    
    cv::Mat input, img;
    
    input = cv::imread(workDir + inputName);
    cv::resize(input, img, frameSize);
    
    Transform transform(&frameSize);
    
    const std::string outputName = "rotation4.mp4";
    Rotate rot(&frameSize, &transform);
    
    double deltaChi = M_PI * 1.0/60.0;
    //double repWidth = M_PI;
    int frameNum = 360;
    
    //rot.writeRepeatYMovie(img, workDir+outputName, repWidth, deltaChi, frameNum);
    //rot.writeConstYMovie(img, workDir+outputName, deltaChi, frameNum);
    rot.writeConstXMovie(img, workDir+outputName, deltaChi, frameNum);
    
    return 0;
}
