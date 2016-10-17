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
#include "Estimate.hpp"
#include "Range.hpp"

int main(int argc, const char * argv[])
{
    // ffmpeg -f image2 -r 30 -i image%4d.jpg -pix_fmt yuv420p video.mp4
    
//    const std::string path = "/Users/masakazu/Desktop/video/20160901/THETA/";
    const std::string path = "/Users/masakazu/Desktop/THETA/";
    const std::string inputVideoName = path + "sample4";
//    const std::string inputVideoNamePreFixed = path + "sample2-pre";
    const std::string outputVideoName = path + "sample4-2";
    
//    const cv::Size frameSizeOriginal(1280, 640);
    const cv::Size fso(960, 480);
    const cv::Size fs(960, 480);
    
    int stride = 1;
    VideoReaderPic vr(fso, inputVideoName, stride);
    VideoWriterMov vw(fso, outputVideoName);
    
    const Transform tfo(fso);
    const Transform tf(fs);
    
    int divNum = 6;
    ExtractFeaturePoint efp(fs, tf, divNum);
    
    int matchThres = 150;
    float coordThres = 0.35;
    MatchFeaturePoint mfp(fs, tf, matchThres, coordThres);

    float rangeAngle = M_PI / 3.0;
    const Range rg(fs, tf, rangeAngle);

    int numThre = 10;
    const Estimate est(tf, rg, numThre);
    MatchMain mm(tfo, tf, efp, mfp, est);

    cv::Mat curRotMat = (cv::Mat_<float>(3,3)
                         << -0.39498305, -0.34497485, 0.85145819,
                            -0.64893043, 0.76081359, 0.0072172284,
                            -0.65029073, -0.54968637, -0.52437317);
    
    mm.ModifyVideo(vr, vw);
    //VideoReaderPic vrPreFixed(frameSizeOriginal, inputVideoNamePreFixed, stride);
    //mm.ModifyVideoMid(vr, vrPreFixed, vw, 30, curRotMat);
    
    return 0;
}
