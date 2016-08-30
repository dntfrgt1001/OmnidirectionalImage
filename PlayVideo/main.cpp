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
    //const std::string path = "/Users/masakazu/Desktop/video/20160816/";
    const std::string path = "/Users/masakazu/Desktop/PIXPRO/video5/";
    //const cv::Size frameSize(1000, 500);
    const cv::Size frameSize(1440, 720);
    const std::string inputName = path + "sample2.mp4";
    const std::string outputName = path + "sample2-30fps";
    
    const int stride = 4;
    VideoReaderMov vr(frameSize,inputName, stride);
    VideoWriterPic vw(frameSize, outputName);
    
    cv::namedWindow("img");
    
    int i=0;
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
