//
//  main.cpp
//  PlayVideo
//
//  Created by masakazu nakazawa on 2016/02/16.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

#include "VideoReader.hpp"
#include "VideoWriter.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/high/";
    const cv::Size frameSize(960, 480);
    const std::string inputName = path + "sample1.mp4";
    const std::string outputName = path + "sample1";
    
    const int stride = 1;
    VideoReaderMov vr(frameSize, inputName, stride);
    VideoWriterPic vw(frameSize, outputName);
    
    cv::namedWindow("img");
    
    int i = 0;
    while (vr.hasNext()) {
        cv::Mat img;
        
        vr.readImg(img);
        
        vw.writeImg(img);
        
        cv::imshow("img", img);
        
        cv::waitKey(10);
        
        std::cout << i++ << std::endl;
    }
    
    return 0;
}
