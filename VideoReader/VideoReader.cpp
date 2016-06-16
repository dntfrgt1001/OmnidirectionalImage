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
    
    if (!video.isOpened()) exit(1);
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
        isAvailable = video.grab();
        hasChecked = true;
        
        return isAvailable;
    }
}

void VideoReader::readImg(cv::Mat &img)
{
    /*
    if (curInput.empty()) {
        bool isNext
    }
    
    cv::resize(curInput, img, frameSize);
    
    curInput.release();
     */
    
    // 未チェック
    if (!hasChecked) {
        hasNext();
    }
    
    // ビデオが空
    if (!isAvailable) {
        std::cout << "error: video has been empty" << std::endl;
        exit(1);
    }
    
    hasChecked = false;
    
    cv::Mat input;
    video.retrieve(input);
    cv::resize(input, img, frameSize);
}
