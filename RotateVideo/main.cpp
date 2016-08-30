//
//  main.cpp
//  RotateVideo
//
//  Created by masakazu nakazawa on 2016/07/05.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

#include "Transform.hpp"
#include "Quarternion.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"

int main(int argc, const char * argv[])
{
//    const std::string path = "/Users/masakazu/Desktop/video/20160810/";
    const std::string path = "/Users/masakazu/Desktop/PIXPRO/video5/";
    const std::string inputName = path + "sample2";
    const std::string outputName = path + "sample2-rot";
    
    const cv::Size frameSize(1280, 640);
    // sample1用
    //float chi = M_PI;
    // sample2用
 //   float chi = M_PI*8.0/5.0;
    // sample4用
    float angle = M_PI;
    cv::Vec3f axis(1.0, 0.0, 0.0);
    cv::Mat rotMat;
    Quarternion::Rodrigues2RotMat(angle, axis, rotMat);
    
    Transform transform(frameSize);

    int stride = 1;
    VideoReaderPic vr(frameSize, inputName, stride);
    VideoWriterPic vw(frameSize, outputName);
    
    cv::namedWindow("current image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("rotated image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    
    int count = 0;
    while (vr.hasNext()) {
        cv::Mat curImg;
        vr.readImg(curImg);
        
        cv::Mat rotImg;
        transform.rotateImgWithRotMat(curImg, rotImg, rotMat);
        vw.writeImg(rotImg);
        
        cv::imshow("current image", curImg);
        cv::imshow("rotated image", rotImg);

        std::cout << count++ << "-th finished" << std::endl;
        
        cv::waitKey(10);
    }

    return 0;
    
}

