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
#include "Quarternion.hpp"
#include "VideoReader.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/20160725/";
    const std::string inputVideoName = "sample4-1.mov";
    const std::string outputVideoName = "modmod.mov";
    
    const cv::Size frameSize(800, 400);
    
    Transform transform(frameSize);
    Rotation rotation(transform);
    VideoReader vr(frameSize, path+inputVideoName);

    cv::Mat img;
    vr.hasNext();
    vr.readImg(img);

    cv::namedWindow("input");
    cv::imshow("input", img);
    
    float chitheta = M_PI;
    cv::Vec3f axistheta(0.0, 1.0, 0.0);
    cv::Mat rotMattheta;
    Quarternion::arbRotMat(chitheta, axistheta, rotMattheta);
    
    std::cout << rotMattheta << std::endl;
    
    cv::Mat rotyImg;
    transform.rotateImgWithRotMat(img, rotyImg, rotMattheta);
    
    cv::namedWindow("mod y");
    cv::imshow("mod y", rotyImg);
    
    float chiphi = M_PI * 0.0;
    cv::Vec3f axisphi(1.0, 0.0, 0.0);
    cv::Mat rotMatphi;
    Quarternion::arbRotMat(chiphi, axisphi, rotMatphi);
    
    std::cout << rotMatphi << std::endl;
    
    cv::Mat rotxImg;
    transform.rotateImgWithRotMat(rotyImg, rotxImg, rotMatphi);
    
    cv::namedWindow("mod x");
    cv::imshow("mod x", rotxImg);
    
    cv::waitKey(-1);
    
    
    
    return 0;
}
