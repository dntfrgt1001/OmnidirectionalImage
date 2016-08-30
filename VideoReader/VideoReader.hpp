//
//  VideoReader.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/14.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef VideoReader_hpp
#define VideoReader_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <iomanip>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

class VideoReader
{
public:
//    enum V_METHOD {VR_VIDEO, VR_ARRAY,};
    
    VideoReader
    (const cv::Size& frameSize, const std::string& videoName,
     const int stride);
    ~VideoReader();
    
    virtual bool hasNext();
    virtual void readImg(cv::Mat& img);
    
protected:
    const cv::Size& frameSize;
    const std::string& videoName;
    cv::Mat nextFrame;
    bool hasChecked;
    bool isAvailable;
    const int frameRateStandard;
    const int stride;
};

class VideoReaderMov: public VideoReader
{
public:
    VideoReaderMov
    (const cv::Size& frameSize, const std::string& videoName,
     const int stride);

    bool hasNext();
  
private:
    cv::VideoCapture video;
};

class VideoReaderPic: public VideoReader
{
public:
    VideoReaderPic
    (const cv::Size& frameSize, const std::string& videoName,
     const int stride);
    
    bool hasNext();

private:
    int count;
};


#endif /* VideoReader_hpp */
