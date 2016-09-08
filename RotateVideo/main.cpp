//
//  main.cpp
//  RotateVideo
//
//  Created by masakazu nakazawa on 2016/07/05.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Transform.hpp"
#include "Quarternion.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/PIXPRO/video8/";
    const std::string inputVideoName = path + "sample2.mp4";
    const std::string outputVideoName = path + "sample2-ori-rot";
    
    const cv::Size frameSize(1280, 640);
    
    Transform tf(frameSize);
    const int stride = 1;
    VideoReaderMov vr(frameSize, inputVideoName, stride);
    VideoWriterPic vw(frameSize, outputVideoName);
    
    cv::Mat rotImg;
    
    float anglez = -M_PI/2.0;
    cv::Vec3f axisz(0.0, 0.0, 1.0);
    cv::Mat rotMatz;
    Quarternion::Rodrigues2RotMat(anglez, axisz, rotMatz);
    
    float anglex = M_PI / 9.0;
    cv::Vec3f axisx(1.0, 0.0, 0.0);
    cv::Mat rotMatx;
    Quarternion::Rodrigues2RotMat(anglex, axisx, rotMatx);
    
    float angley = M_PI / 4.0;
    cv::Vec3f axisy(0.0, 1.0, 0.0);
    cv::Mat rotMaty;
    Quarternion::Rodrigues2RotMat(angley, axisy, rotMaty);
    
    cv::Mat img;
    vr.readImg(img);
    
    cv::Mat rotMat = rotMaty * rotMatx * rotMatz;
    tf.rotateImgWithRotMat(img, rotImg, rotMaty * rotMatx * rotMatz);
    
    cv::namedWindow("original");
    cv::namedWindow("rotated");
    
    cv::imshow("original", img);
    cv::imshow("rotated", rotImg);
    
    cv::waitKey(-1);
    
    while (vr.hasNext()) {
        cv::Mat curImg;
        vr.readImg(curImg);
        
        cv::Mat curRotImg;
        tf.rotateImgWithRotMat(curImg, curRotImg, rotMat);
        
        vw.writeImg(curRotImg);
        
        cv::imshow("original", curImg);
        cv::imshow("rotated", curRotImg);
        
        cv::waitKey(10);
    }
    
    return 0;
}
