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
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class VideoWriter
{
public:
    VideoWriter(const cv::Size& frameSize, const std::string& videoName);
    ~VideoWriter();

    void writeImg(const cv::Mat& img);
    
private:
    const cv::Size& frameSize;
    cv::VideoWriter video;
};

#endif /* VideoWriter_hpp */
