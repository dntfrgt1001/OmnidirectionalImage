//
//  main.cpp
//  VideoMatchTest
//
//  Created by masakazu nakazawa on 2016/01/11.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Rotation.hpp"
#include "MatchMain.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"

int main(int argc, const char * argv[])
{    
    const std::string path = "/Users/masakazu/Desktop/video/20160725/";
    const std::string inputVideoName = path + "sample6.mp4";
    const std::string outputVideoName = path + "sample6";
    
    
    const cv::Size frameSizeOriginal(1000, 500);
    const cv::Size frameSize(800, 400);
    
    const Transform transformOriginal(frameSizeOriginal);
    const Transform transform(frameSize);
    
    int divNum = 6;
    ExtractFeaturePoint extract(frameSize, transform, divNum);
    
    int matchThres = 200;
    float coordThres = 0.4;
    MatchFeaturePoint match(frameSize, transform, matchThres, coordThres);
    
    Rotation rotation(transform);

    MatchMain matchMain(transformOriginal, transform, extract, match, rotation);
    
    VideoReader vr(frameSizeOriginal, inputVideoName);
    
    VideoWriterPic vw(frameSizeOriginal, outputVideoName);
    
    matchMain.ModifyVideo(vr, vw);
    
    return 0;
}
