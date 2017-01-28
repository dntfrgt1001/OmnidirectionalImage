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
#include "Rotation.hpp"
#include "Quaternion.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"

int main(int argc, const char * argv[])
{
//    const std::string path = "/Users/masakazu/Desktop/casio/bowling/02/";
    const std::string path = "/Users/masakazu/Desktop/Jack/";
    const std::string inputVideoName = path + "sample14.mp4";
    const std::string outputVideoName = path + "sample14-rot";
    
//    const cv::Size frameSize(1920, 960);
//    const cv::Size frameSize(480, 240);
    const cv::Size frameSize(800, 400);
    
    Transform tf(frameSize);
    const int stride = 1;
    VideoReaderMov vr(frameSize, inputVideoName, stride);
    
    cv::Mat rotImg;
    
    cv::Vec3f rotVec1 = 1 * M_PI/0.90 * cv::Vec3f(0.0, 1.0, 0.0);
    cv::Mat rotMat1 = Rotation::RotVec2RotMat(rotVec1);
    
    cv::Vec3f rotVec2 = 1 * M_PI/20.0 * cv::Vec3f(0.0, 0.0, 1.0);
    cv::Mat rotMat2 = Rotation::RotVec2RotMat(rotVec2);
    
    cv::Vec3f rotVec3 = 0 * M_PI/0.95 * cv::Vec3f(0.0, 1.0, 0.0);
    cv::Mat rotMat3 = Rotation::RotVec2RotMat(rotVec3);
    
    cv::Vec3f rotVec4 = 0 * M_PI/2.5 * cv::Vec3f(1.0, 0.0, 0.0);
    cv::Mat rotMat4 = Rotation::RotVec2RotMat(rotVec4);
    
    cv::Mat img;
    vr.readImg(img);
    
    cv::Mat rotMat = rotMat4 * rotMat3 * rotMat2 * rotMat1;
    tf.rotateImg(img, rotImg, rotMat);
    
    cv::namedWindow("original");
    cv::namedWindow("rotated");
    
    cv::imshow("original", img);
    cv::imshow("rotated", rotImg);
    
    cv::waitKey(10000);
    
    VideoWriterPic vw(frameSize, outputVideoName);
    
    vw.writeImg(rotImg);
    
    for (int i=1; vr.hasNext(); i++) {
        cv::Mat curImg;
        vr.readImg(curImg);
        
        cv::Mat curRotImg;
        tf.rotateImg(curImg, curRotImg, rotMat);
        
        vw.writeImg(curRotImg);
        
        cv::imshow("original", curImg);
        cv::imshow("rotated", curRotImg);
        
        std::cout << i << "-th frame finished" << std::endl;
        
        cv::waitKey(10);
    }

    return 0;
}
