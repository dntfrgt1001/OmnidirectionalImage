//
//  main.cpp
//  Mask
//
//  Created by masakazu nakazawa on 2016/07/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"

int main(int argc, const char * argv[])
{
    const cv::Size frameSize(1000, 500);
    
    Transform transform(frameSize);
    
    const int divNum = 6;
    ExtractFeaturePoint efp(frameSize, transform, divNum);
    
    cv::Mat img(frameSize, CV_8UC3);
    
    for (int u=0; u<frameSize.width; u++) {
        for (int v=0; v<frameSize.height; v++) {
            if (efp.isInLowLatitude(u, v)) {
                img.at<cv::Vec3b>(v, u) = cv::Vec3b(255, 255, 255);
            } else {
                img.at<cv::Vec3b>(v, u) = cv::Vec3b(0, 0, 0);
            }
        }
    }
    
    float chi = M_PI / 6.0 * 5;
    cv::Mat rotImg(frameSize, CV_8UC3);
    transform.rotateVerticalImg(chi, img, rotImg);
    
    cv::namedWindow("mask");
    cv::imshow("mask", img);
    
    cv::namedWindow("rot");
    cv::imshow("rot", rotImg);
    
    cv::waitKey(-1);
    
    return 0;
}
