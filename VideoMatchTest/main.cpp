//
//  main.cpp
//  VideoMatchTest
//
//  Created by masakazu nakazawa on 2016/01/11.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include "Rotate.hpp"
#include "Transform.hpp"
#include "Match.hpp"

int main(int argc, const char * argv[])
{
    const std::string workDir = "/Users/masakazu/Documents/Koike lab/product/OmnidirectionalImage/working/";
    
    const cv::Size frameSize(1000, 500);
    
    const Transform transform(&frameSize);
    Rotate rot(&frameSize, &transform);
    
    const std::string videoName = "rotation.mp4";
    cv::VideoCapture capture(workDir + videoName);
    if (!capture.isOpened()) return -1;
    
    cv::Mat input, stdImg;
    capture >> input;
    if (input.empty()) return -1;
    cv::resize(input, stdImg, frameSize);
    
    int orthRange = transform.dphi2v(M_PI * 1.0/3.0);
    Match match(&frameSize, &stdImg, &rot, orthRange);
    
    const std::string outputName = "output.mp4";
    cv::VideoWriter writer(workDir+outputName, CV_FOURCC('m', 'p', '4', 'v'),
                           30, frameSize, true);
    if (!writer.isOpened()) return -1;
    
    cv::namedWindow("standard image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("current image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("modified image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    
    while (true) {
        cv::Mat curImg;
        cv::Mat curModImg(frameSize, CV_8UC3);
        
        capture >> input;
        if (input.empty() || cv::waitKey(10)>0) break;
        cv::resize(input, curImg, frameSize);
        
        match.rotateYMatch(curImg, curModImg);
        
        cv::imshow("standard image", stdImg);
        cv::imshow("current image", curImg);
        cv::imshow("modified image", curModImg);
        
        writer << curModImg;
        
//        cv::waitKey(-1);
    }
    
    
    return 0;
}
