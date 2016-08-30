//
//  main.cpp
//  RangeTest
//
//  Created by masakazu nakazawa on 2016/08/17.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "Transform.hpp"
#include "Quarternion.hpp"

int main(int argc, const char * argv[])
{
    const cv::Size framesize(500, 250);
    
    Transform tf(framesize);
    
    cv::Mat img(framesize, CV_8UC1);
    float fieldRadius = tanf(M_PI/2.5);
    
    for (int u=0; u<framesize.width; u++) {
        for (int v=0; v<framesize.height; v++) {
            cv::Point2f equirect(u, v);
            cv::Point2f psphere;
            tf.equirect2psphere(equirect, psphere);
            
            
            float tant2 = tanf(psphere.x) * tanf(psphere.x);
            float tanp2 = tanf(psphere.y) * tanf(psphere.y);
            float leftValue = tant2 + tanp2 + tant2*tanp2;
            
            if (leftValue <= fieldRadius*fieldRadius) {
                img.at<uchar>(v, u) = 255;
            } else {
                img.at<uchar>(v, u) = 0;
            }
        }
    }
    
    /*
    float chiy = M_PI/2.0;
    cv::Vec3f axisy(0.0, 1.0, 0.0);
    cv::Mat rotMaty;
    Quarternion::Rodrigues2RotMat(chiy, axisy, rotMaty);
    for (int u=0; u<framesize.width; u++) {
        for (int v=0; v<framesize.height; v++) {
            cv::Point2f equirect(u, v);
            cv::Point2f equirectRot;
            tf.equirectWithRotMat(equirect, equirectRot, rotMaty);
            
            cv::Point2f psphere;
            tf.equirect2psphere(equirectRot, psphere);
            
            float tant2 = tanf(psphere.x) * tanf(psphere.x);
            float tanp2 = tanf(psphere.y) * tanf(psphere.y);
            float leftValue = tant2 + tanp2 + tant2*tanp2;
            
            if (leftValue <= fieldRadius*fieldRadius) {
                //img.at<uchar>(v, u) = 85;
                img.at<char>(v, u) = 255;
            }
            //else {
            //    img.at<uchar>(v, u) = 0;
            //}

        }
    }
     
    
    float chix = M_PI/2.0;
    cv::Vec3f axisx(1.0, 0.0, 0.0);
    cv::Mat rotMatx;
    Quarternion::Rodrigues2RotMat(chix, axisx, rotMatx);
    for (int u=0; u<framesize.width; u++) {
        for (int v=0; v<framesize.height; v++) {
            cv::Point2f equirect(u, v);
            cv::Point2f equirectRot;
            tf.equirectWithRotMat(equirect, equirectRot, rotMatx);
            
            cv::Point2f psphere;
            tf.equirect2psphere(equirectRot, psphere);
            
            float tant2 = tanf(psphere.x) * tanf(psphere.x);
            float tanp2 = tanf(psphere.y) * tanf(psphere.y);
            float leftValue = tant2 + tanp2 + tant2*tanp2;
            
            if (leftValue <= fieldRadius*fieldRadius) {
                //img.at<uchar>(v, u) = 170;
                img.at<char>(v, u) = 255;
            }
            //else {
            //    img.at<uchar>(v, u) = 0;
            //}
            
        }
    }
    
*/
    /*
    float chix = M_PI/2.0;
    cv::Vec3f axisx(1.0, 0.0, 0.0);
    cv::Mat rotMatx;
    Quarternion::arbRotMat(chix, axisx, rotMatx);
    cv::Mat invRotMatx = rotMatx.inv();
    
    cv::Mat rotImg(framesize, CV_8UC1);
    for (int u=0; u<framesize.width; u++) {
        for (int v=0; v<framesize.height; v++) {
            cv::Point2f equirectRot(u, v);
            cv::Point2f equirect;
            tf.equirectWithRotMat(equirectRot, equirect, invRotMatx);
            rotImg.at<uchar>(v, u) = img.at<uchar>(equirect.y, equirect.x);
        }
    }*/
    
    cv::namedWindow("Test");
    cv::imshow("Test", img);
    
    
//    cv::namedWindow("RotTest");
//    cv::imshow("RotTest", rotImg);
    
    cv::waitKey(-1);
    
    return 0;
}
