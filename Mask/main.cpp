//
//  main.cpp
//  Mask
//
//  Created by masakazu nakazawa on 2016/07/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <math.h>
#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "Quarternion.hpp"


int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/rotation/";
    const std::string inputName = path + "R0010090.JPG";
//    const cv::Size frameSize(5376, 2688);
    const cv::Size frameSize(2000, 1000);
    cv::Mat input = cv::imread(inputName);
    cv::Mat img;
    cv::resize(input, img, frameSize);
    
    Transform transform(frameSize);
    
    float theta =  M_PI / 3;
    cv::Vec3f axis(1, 0, 0);
    cv::Mat rotMat;
    Quarternion::arbRotMat(theta, axis, rotMat);
    
    const int divNum = 3;
    
    ExtractFeaturePoint efp(frameSize, transform, divNum);
    
    cv::Mat rotImg;
    transform.rotateImgWithRotMat(img, rotImg, rotMat);
    
    for (int u=0; u<frameSize.width; u++) {
        for (int v=0; v<frameSize.height; v++) {
            
                cv::Vec3b tmpVec = rotImg.at<cv::Vec3b>(v, u);
                uchar R = tmpVec[0];
                uchar G = tmpVec[1];
                uchar B = tmpVec[2];
                
                
                uchar umax = MAX(MAX(R, G), B);
                uchar umin = MIN(MIN(R, G), B);
                
                uchar gray = umax/2 + umin/2;
                
                rotImg.at<cv::Vec3b>(v, u) = cv::Vec3b(gray, gray, gray);
            //} else {
            //    img.at<cv::Vec3b>(v, u) = cv::Vec3b(0, 0, 0);
            if (efp.isInLowLatitude(cv::Point2f(u, v))) {
                rotImg.at<cv::Vec3b>(v, u) += cv::Vec3b(0, 0, 100);
            }
        }
    }
    
    cv::Mat rotInvImg;
    transform.rotateImgWithRotMat(rotImg, rotInvImg, rotMat.inv());
    
    //cv::Mat resizeImg;
    //cv::resize(img, resizeImg, frameSizeRe);
    
    cv::namedWindow("rot img");
    cv::imshow("rot img", rotImg);
    const std::string outputName1 = path + "rotImg.jpg";
    cv::imwrite(outputName1, rotImg);

    cv::namedWindow("rot inv img");
    cv::imshow("rot inv img", rotInvImg);
    const std::string outputName2 = path + "rotInvImg.jpg";
    cv::imwrite(outputName2, rotInvImg);
    
//    cv::namedWindow("rot");
//    cv::imshow("rot", rotImg);
    
    cv::waitKey(-1);
    
    return 0;
}
