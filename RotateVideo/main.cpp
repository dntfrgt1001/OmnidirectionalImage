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
    const std::string path = "/Users/masakazu/Desktop/EX-FR200/";
    const std::string inputVideoName = path + "sample3";
    const std::string outputVideoName = path + "sample3-rot";
    
    const cv::Size frameSize(1000, 500);
    
    Transform tf(frameSize);
    const int stride = 1;
    VideoReaderPic vr(frameSize, inputVideoName, stride);
    
    cv::Mat rotImg;
    
    cv::Vec3f rotVec1 = M_PI/4.7 * cv::Vec3f(0.0, 1.0, 0.0);
    cv::Mat rotMat1;
    Rotation::RotVec2RotMat(rotVec1, rotMat1);

    cv::Vec3f rotVec2 = -M_PI/2.2 * cv::Vec3f(1.0, 0.0, 0.0);
    cv::Mat rotMat2;
    Rotation::RotVec2RotMat(rotVec2, rotMat2);
    
    cv::Vec3f rotVec3 = M_PI/2.3 * cv::Vec3f(0.0, 1.0, 0.0);
    cv::Mat rotMat3;
    Rotation::RotVec2RotMat(rotVec3, rotMat3);
    
    cv::Mat img;
    vr.readImg(img);
    
    cv::Mat rotMat = rotMat3 * rotMat2 * rotMat1;
    tf.rotateImgWithRotMat(img, rotImg, rotMat3 * rotMat2 * rotMat1);
    
    cv::namedWindow("original");
    cv::namedWindow("rotated");
    
    cv::imshow("original", img);
    cv::imshow("rotated", rotImg);
    
    cv::waitKey(-1);
    
    VideoWriterPic vw(frameSize, outputVideoName);
    
    vw.writeImg(rotImg);
    
    for (int i=1; vr.hasNext(); i++) {
        cv::Mat curImg;
        vr.readImg(curImg);
        
        cv::Mat curRotImg;
        tf.rotateImgWithRotMat(curImg, curRotImg, rotMat);
        
        vw.writeImg(curRotImg);
        
        cv::imshow("original", curImg);
        cv::imshow("rotated", curRotImg);
        
        std::cout << i << "-th frame finished" << std::endl;
        
        cv::waitKey(10);
    }

    return 0;
}
