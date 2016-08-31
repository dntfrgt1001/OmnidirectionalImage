//
//  VideoReader.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/14.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "VideoReader.hpp"

VideoReader::VideoReader
(const cv::Size& frameSize, const std::string& videoName, const int stride):
frameSize(frameSize), videoName(videoName),
isChecked(false), frameRateStandard(30), stride(stride)
{
    assert(stride > 0);
}

VideoReader::~VideoReader()
{
}

bool VideoReader::hasNext()
{
    return false;
}

void VideoReader::readImg(cv::Mat &img)
{
    // 未チェック
    if (!isChecked) {
        hasNext();
    }
    
    // ビデオが空
    if (!isAvailable) {
        std::cout << "error: video has been empty" << std::endl;
        exit(1);
    }
    
    // 未チェックに
    isChecked = false;
    
    img = nextFrame.clone();
}

VideoReaderMov::VideoReaderMov
(const cv::Size& frameSize, const std::string& videoName, const int stride):
VideoReader(frameSize, videoName, stride)
{
    // VideoCaptureをオープン
    video.open(videoName);
    if (!video.isOpened()) {
        std::cout << "video open error" << std::endl;
        exit(1);
    }
}

bool VideoReaderMov::hasNext()
{
    if (isChecked) {
        return isAvailable;
    } else {
        isChecked = true;
        
        // strideごとにフレームを取り出す
        cv::Mat input;
        for (int i=0; i<stride; i++) {
            video >> input;

            if (input.empty()) {
                isAvailable = false;
                return false;
            }
        }
        
        isAvailable = true;
        cv::resize(input, nextFrame, frameSize);
        
        return isAvailable;
    }
}

VideoReaderPic::VideoReaderPic
(const cv::Size& frameSize, const std::string& videoName, const int stride):
VideoReader(frameSize, videoName, stride), count(0)
{
    
}

bool VideoReaderPic::hasNext()
{
    if (isChecked) {
        return isAvailable;
    } else {
        isChecked = true;
        
        // strideごとにフレームを取り出す
        std::stringstream fileName;
        fileName << videoName << "/" << "image"
                 << std::setw(4) << std::setfill('0') << count << ".jpg";
        cv::Mat input;
        input = cv::imread(fileName.str());
        count += stride;
        
        if (input.empty()) {
            isAvailable = false;
        } else {
            isAvailable = true;
            cv::resize(input, nextFrame, frameSize);
        }
        
        return isAvailable;
    }
}


