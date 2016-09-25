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
frameSize(frameSize), videoName(videoName)
{
}

void VideoWriter::writeImg(const cv::Mat &img)
{
}

VideoWriterMov::VideoWriterMov
(const cv::Size& frameSize, const std::string& videoName):
VideoWriter(frameSize, videoName)
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

void VideoWriterMov::writeImg(const cv::Mat &img)
{
    video << img;
}

VideoWriterPic::VideoWriterPic
(const cv::Size& frameSize, const std::string& videoName):
VideoWriter(frameSize, videoName), count(0)
{
                                      // user       group           other
    int stat = mkdir(videoName.c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
    
    if (stat != 0) {
        std::cout << "error: output video cannot be opened" << std::endl;
        exit(1);
    }
}

void VideoWriterPic::writeImg(const cv::Mat &img)
{
    std::stringstream fileName;

    fileName << videoName << "/" << "image"
             << std::setw(4) << std::setfill('0') << count++ << ".jpg";
    
    cv::Mat resizedImg;
    cv::resize(img, resizedImg, frameSize);
    
    cv::imwrite(fileName.str(), img);
}
