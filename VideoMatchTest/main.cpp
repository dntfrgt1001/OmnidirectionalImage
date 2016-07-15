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
    const std::string path = "/Users/masakazu/Desktop/20160701/";
    const std::string inputVideoName = "sample1.mp4";
    const std::string outputVideoName = "mod.mov";
    
    const cv::Size origFrameSize(1000, 500);
    const cv::Size frameSize(800, 400);
    
    Transform transform(frameSize);
    
    int divNum = 6;
    ExtractFeaturePoint extract(frameSize, transform, divNum);
    
    int matchThres = 200;
    float coordThres = 0.4;
    MatchFeaturePoint match(frameSize, transform, matchThres, coordThres);
    
    Rotation rotation(transform);
    
    MatchMain matchMain(origFrameSize,transform, extract, match, rotation);
    
    VideoReader vr(origFrameSize, path + inputVideoName);
    VideoWriter vw(origFrameSize, path + outputVideoName);
    
    matchMain.ModifyVideo(vr, vw);
    

    /*
    
    const std::string workDir = "/Users/masakazu/Desktop/working/";
    
    const cv::Size frameSize(1280, 640);
    
    Transform transform(frameSize);
    Rotate rot(frameSize, transform);
    
    const std::string videoName = "phi.mp4";
    cv::VideoCapture capture(workDir + videoName);
    if (!capture.isOpened()) return -1;
    
    cv::Mat input, stdImg;
    capture >> input;
    if (input.empty()) return -1;
    cv::resize(input, stdImg, frameSize);
    
    int divNum = 8;
    Match match(frameSize, stdImg, transform, rot, divNum);
    
    const std::string outputName = "output11.mp4";
    cv::VideoWriter writer(workDir+outputName, CV_FOURCC('m', 'p', '4', 'v'),
                           30, frameSize, true);
    if (!writer.isOpened()) return -1;
    
    cv::namedWindow("standard image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("current image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("modified image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    
    int i = 0;
    while (true) {
        cv::Mat curImg;
        cv::Mat curModImg(frameSize, CV_8UC3);
        
        capture >> input;
        if (input.empty() || cv::waitKey(10)>0) break;
        cv::resize(input, curImg, frameSize);
        
        match.rotateXMatch(curImg, curModImg);
        
        if (i % 3 == 0) match.updateStdImg(curModImg);
        
        cv::imshow("standard image", stdImg);
        cv::imshow("current image", curImg);
        cv::imshow("modified image", curModImg);
        
        std::cout << "frame number = " << i << std::endl;
        
        writer << curModImg;
        
        i++;
        if(i >= 120) break;
        
//        cv::waitKey(-1);
    }
    
    */
     
    return 0;
}
