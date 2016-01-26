//
//  main.cpp
//  RotationTest
//
//  Created by masakazu nakazawa on 2016/01/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Rotate.hpp"
#include "Transform.hpp"

int main(int argc, const char * argv[])
{
    std::string workDir = "/Users/masakazu/Documents/Koike lab/product/OmnidirectionalImage/working/";
    std::string inputName = "R0010050.JPG";
    
    const cv::Size frameSize(1001, 500);
    
    cv::Mat input, img;
    
    input = cv::imread(workDir + inputName);
    cv::resize(input, img, frameSize);
    
    const Transform transform(frameSize);
    Rotate rot(frameSize, transform);
    
    cv::Mat rotImg(frameSize, CV_8UC3);


    cv::namedWindow("rotImg", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    
    img.copyTo(rotImg);
    cv::imshow("rotImg", rotImg);
    cv::waitKey(-1);

    double deltaXChi = M_PI * 1.0/60.0;
    double deltaYChi = M_PI * 1.0/60.0;
    
    int limit = 300;
    for (int i=0; i<limit; i++) {
        double curXChi = deltaXChi * i;
        double curYChi = deltaYChi * i;
        
        rot.rotateXYAng(curXChi, curYChi, img, rotImg);
        cv::imshow("rotImg", rotImg);
        
        if (cv::waitKey(10) > 0) break;
     }
    
    /*
    rot.rotateYOrth(thetaChi, rotThetaImg);
    
    rot.setImage(&rotThetaImg);
    
    rot.rotateXOrth(phiChi, rotPhiImg);
    
    cv::namedWindow("input", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    //    cv::namedWindow("rotTheta", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("rotPhi", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    
    cv::imshow("input", img);
    //    cv::imshow("rotTheta", rotThetaImg);
    cv::imshow("rotPhi", rotPhiImg);
    
    double deltaChi = M_PI * 1.0/40.0;
    double angWidth = M_PI * 3.0/2.0;
    std::string outputName = "out.mp4";
    
    rot.writeRotateYMovie(deltaChi, angWidth, workDir+outputName, 150);
    */
    //    rot.showotateYMovie(deltaChi, angWidth);
    
    
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

    
    return 0;
}
