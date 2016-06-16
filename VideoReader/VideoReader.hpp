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

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


class VideoReader
{
public:
    VideoReader(const cv::Size& frameSize, const std::string& videoName);
    ~VideoReader();
    
    bool hasNext();
    void readImg(cv::Mat& img);
    
private:
    const cv::Size& frameSize;
    cv::VideoCapture video;
    bool hasChecked;
    bool isAvailable;
};

#endif /* VideoReader_hpp */
