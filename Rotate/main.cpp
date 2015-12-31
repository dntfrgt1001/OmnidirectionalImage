//
//  main.cpp
//  Rotate
//
//  Created by masakazu nakazawa on 2015/12/27.
//  Copyright © 2015年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Rotate.hpp"


int main(int argc, const char * argv[])
{
    std::string workDir = "/Users/masakazu/Documents/Koike lab/product/OmnidirectionalImage/working/";
    std::string inputName = "R0010050.JPG";
    
    const cv::Size frameSize(1001, 500);
    
    cv::Mat input, img;
    
    input = cv::imread(workDir + inputName);
    cv::resize(input, img, frameSize);
    
    Rotate rot(&frameSize, &img);
    
    
    cv::Mat rotThetaImg = cv::Mat::zeros(frameSize, CV_8UC3);
    cv::Mat rotPhiImg = cv::Mat::zeros(frameSize, CV_8UC3);
    
    double thetaChi = M_PI * 1.0/3.0;
    double phiChi = M_PI * 1.0/2.0;

    rot.rotateYOrth(thetaChi, rotThetaImg);
    
    rot.setImage(&rotThetaImg);

    rot.rotateXOrth(phiChi, rotPhiImg);
    
    cv::namedWindow("input", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("rotTheta", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("rotPhi", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    
    cv::imshow("input", img);
    cv::imshow("rotTheta", rotThetaImg);
    cv::imshow("rotPhi", rotPhiImg);
    
//    std::string outputName = "lattice30.png";
//    cv::imwrite(workDir + outputName, rotateImg);
    
    /*
    cv::Mat zero = cv::Mat::zeros(frameSize, CV_8UC1);
    
    int n = 30;
    
    float thetar = M_PI * 1.0/1.1 * (-1);
    float phir = M_PI * 1.0/2.1 ;
    int ur, vr;
    rot.ang2orth(thetar, phir, ur, vr);
    
    cv::circle(zero, cv::Point(ur, vr), 5, cv::Scalar(255));
    
    for (int i=0; i<n; i++) {
        float chi = 2*M_PI * (float)i/n;
        
        int u, v;
        rot.rotateReverse(chi, ur, vr, u, v);
        
        std::cout << "(u, v) = (" << u << ", " << v << ")" << std::endl;
        
        cv::circle(zero, cv::Point(u,v), 3, cv::Scalar(125));
    }
    
    cv::namedWindow("test", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    
    cv::imshow("test", zero);
  */
    cv::waitKey(-1);
    
    return 0;
}
