//
//  VideoWriter.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/14.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef VideoWriter_hpp
#define VideoWriter_hpp

#include <stdio.h>
#include <sys/stat.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

class VideoWriter
{
public:
    VideoWriter(const cv::Size& frameSize, const std::string& videoName);

    virtual void writeImg(const cv::Mat& img);
    
protected:
    const cv::Size& frameSize;
    const std::string& videoName;
};

class VideoWriterMov : public VideoWriter
{
public:
    VideoWriterMov(const cv::Size& frameSize, const std::string& videoName);
    
    void writeImg(const cv::Mat& img);
    
private:
    cv::VideoWriter video;
};

class VideoWriterPic : public VideoWriter
{
public:
    VideoWriterPic(const cv::Size& frameSize, const std::string& videoName);

    void writeImg(const cv::Mat& img);

private:
    int count;
};

#endif /* VideoWriter_hpp */
