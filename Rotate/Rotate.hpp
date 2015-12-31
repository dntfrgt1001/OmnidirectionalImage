//
//  Rotate.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2015/12/27.
//  Copyright © 2015年 masakazu. All rights reserved.
//

#ifndef Rotate_hpp
#define Rotate_hpp

#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Rotate {
public:
    Rotate(const cv::Size* frameSize, cv::Mat* img);
    ~Rotate();
    
    void orth2ang(int u, int v, double& theta, double& phi);
    void ang2orth(double theta, double phi, int& u, int& v);
    
    void rotateXOrthDot(double chi, int u, int v, int& ur, int& vr);
    void rotateXOrth(double chi, cv::Mat& rotImg);
    
    void rotateYOrthDot(double chi, int u, int v, int& ur, int& vr);
    void rotateYOrth(double chi, cv::Mat& rotImg);
    
    void setThetaScale();
    double normalizeTheta(double rawTheta);
    
    void setImage(cv::Mat* newImg);
    
private:
    const cv::Size* frameSize;
    cv::Mat* img;
    double thetaScale; // [pixel/rad]
};

#endif /* Rotate_hpp */
