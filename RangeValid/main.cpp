//
//  main.cpp
//  RangeValid
//
//  Created by masakazu nakazawa on 2016/08/30.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "Transform.hpp"
#include "Rotation.hpp"

int main(int argc, const char * argv[])
{
    const cv::Size frameSize(500, 250);
    
    cv::Mat img(frameSize, CV_8UC1);
    
    const Transform tf(frameSize);
    
    const float filedAngle = M_PI / 6.0;
    const int matchThre = 20;
    const Rotation rot(tf, filedAngle, matchThre);
    
    
    for (int u = 0; u < frameSize.width; u++) {
        for (int v = 0; v < frameSize.height; v++) {
            cv::Point2f equirect(u, v);
            cv::Point3f sphere;
            tf.equirect2sphere(equirect, sphere);
            
            if (rot.isInFront(sphere)) {
                img.at<uchar>(v, u) = 255;
            } else {
                img.at<uchar>(v, u) = 0;
            }
        }
    }
    
    const cv::Mat rotMatY = (cv::Mat_<float>(3,3) << 0,0,-1,0,1,0,1,0,0);
    for (int u = 0; u < frameSize.width; u++) {
        for (int v = 0; v < frameSize.height; v++) {
            cv::Point2f equirect(u, v);
            cv::Point3f sphere;
            tf.equirect2sphere(equirect, sphere);
            cv::Point3f sphereRot;
            tf.rotateSphere(sphere, sphereRot, rotMatY);
            
            if (rot.isInFront(sphereRot)) {
                img.at<uchar>(v, u) = 170;
            }
        }
    }
    
    const cv::Mat rotMatX = (cv::Mat_<float>(3,3) << 1,0,0,0,0,-1,0,1,0);
    for (int u = 0; u < frameSize.width; u++) {
        for (int v = 0; v < frameSize.height; v++) {
            cv::Point2f equirect(u, v);
            cv::Point3f sphere;
            tf.equirect2sphere(equirect, sphere);
            cv::Point3f sphereRot;
            tf.rotateSphere(sphere, sphereRot, rotMatX);
            
            if (rot.isInFront(sphereRot)) {
                img.at<uchar>(v, u) = 85;
            }
        }
    }
    
    cv::namedWindow("img");
    cv::imshow("img", img);
    
    cv::waitKey(-1);
    
    
    return 0;
}
