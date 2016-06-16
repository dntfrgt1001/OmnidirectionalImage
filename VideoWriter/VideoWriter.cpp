//
//  VideoWriter.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/14.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "VideoWriter.hpp"

VideoWriter::VideoWriter
(const cv::Size& frameSize, const std::string& videoName):
frameSize(frameSize)
{
    // mp4コンテナの出力ができないのでffmpegで変換
    // ffmpeg -i inputvideo -vcodec libx264 outputvideo
    video = cv::VideoWriter
    (videoName, CV_FOURCC('m', 'p', '4', 'v'), 30.0, frameSize);
    
    if (!video.isOpened()) {
        std::cout << "error: output video cannot be opened" << std::endl;
        exit(1);
    }
}

VideoWriter::~VideoWriter(){
    
}

void VideoWriter::writeImg(const cv::Mat &img)
{
    video << img;
}