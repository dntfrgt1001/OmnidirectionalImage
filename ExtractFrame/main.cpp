//
//  main.cpp
//  ExtractFrame
//
//  Created by masakazu nakazawa on 2016/07/04.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "VideoReader.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string inputName = "R0010012_er.MP4";

    const cv::Size frameSize(1920, 960);
    const std::string outputName1 = "frame1.jpg";
    const std::string outputName2 = "frame2.jpg";
    
    VideoReader vr(frameSize, path + inputName);
    
    const int frameNum = 11;
    
    cv::namedWindow("current image");
    
    for (int i=0; vr.hasNext(); i++) {
        cv::Mat img;
        
        vr.readImg(img);
        
        if (i == frameNum) {
            cv::imwrite(path + outputName1, img);
            vr.readImg(img);
            cv::imwrite(path + outputName2, img);
            
            break;
        }
        
        cv::imshow("current image", img);
        cv::waitKey(10);
    }
    
    return 0;
}
