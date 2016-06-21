//
//  VideoReader.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/14.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "VideoReader.hpp"

VideoReader::VideoReader
(const cv::Size& frameSize, const std::string& videoName):
frameSize(frameSize), hasChecked(false)
{
    video.open(videoName);
    if (!video.isOpened()) {
        std::cout << "video open error" << std::endl;
        exit(1);
    }
}

VideoReader::~VideoReader()
{
}

bool VideoReader::hasNext()
{
    /*
    if (curInput.empty()) {
        video >> curInput;
    } else {
        return true;
    }
    
    if (curInput.empty()) {
        return false;
    } else {
        return true;
    }
     */
    
    if (hasChecked) {
        return isAvailable;
    } else {
        hasChecked = true;
        
        cv::Mat input;
        video >> input;
        
        if (input.empty()) {
            isAvailable = false;
        } else {
            isAvailable = true;
            cv::resize(input, nextFrame, frameSize);
        }

        return isAvailable;
    }
}

void VideoReader::readImg(cv::Mat &img)
{
    // 未チェック
    if (!hasChecked) {
        hasNext();
    }
    
    // ビデオが空
    if (!isAvailable) {
        std::cout << "error: video has been empty" << std::endl;
        exit(1);
    }
    
    // 未チェックに
    hasChecked = false;
    
    img = nextFrame.clone();
}


