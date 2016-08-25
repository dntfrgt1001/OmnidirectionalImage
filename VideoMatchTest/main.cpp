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
    // ffmpeg -f image2 -r 30 -i mod%4d.jpg -pix_fmt yuv420p video.mp4
    
    const std::string path = "/Users/masakazu/Desktop/video/20160810/";
    const std::string inputVideoName = path + "sample1.mp4";
    const std::string outputVideoName = path + "sample1";
    
    
    const cv::Size frameSizeOriginal(1000, 500);
    const cv::Size frameSize(800, 400);
    
    const Transform tfo(frameSizeOriginal);
    const Transform tf(frameSize);
    
    int divNum = 6;
    ExtractFeaturePoint efp(frameSize, tf, divNum);
    
    int matchThres = 200;
    float coordThres = 0.3;
    MatchFeaturePoint mfp(frameSize, tf, matchThres, coordThres);
    
    float fieldAngle = M_PI / 2.75;
    int matchThre = 20;
    Rotation rot(tf, fieldAngle, matchThre);

    MatchMain mm(tfo, tf, efp, mfp, rot);
    
    VideoReader vr(frameSizeOriginal, inputVideoName);
    
    VideoWriterPic vw(frameSizeOriginal, outputVideoName);
    
    mm.ModifyVideo(vr, vw);
    
    return 0;
}
