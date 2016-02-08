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
    
    Transform transform(frameSize);
    
    const std::string outputName = "rotation8.mp4";
    Rotate rot(frameSize, transform);
    
    double deltaXChi = M_PI * 1.0/30.0;
    double deltaYChi = M_PI * 1.0/90.0;
    int frameNum = 360;
    
    //rot.writeConstXMovie(img, workDir+outputName, deltaXChi, frameNum);
    //rot.writeConstYMovie(img, workDir+outputName, deltaYChi, frameNum);
    rot.writeConstXYMovie(img, workDir+outputName,
                          deltaXChi, deltaYChi, frameNum);
    
    return 0;
}
