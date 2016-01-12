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

#include "Transform.hpp"

class Rotate {
public:
    Rotate(const cv::Size* frameSize, const Transform* transform);
    ~Rotate();
    
    //void orth2ang(int u, int v, double& theta, double& phi);
    //void ang2orth(double theta, double phi, int& u, int& v);
    //void ang2orthd(double theta, double phi, double& u, double& v);
    
    //void rotateXOrthBilinearDot(double chi, int u, int v, cv::Vec3b& pixel);
    void rotateXOrthNearDot(double chi, int u, int v, int& ur, int&vr);
    void rotateXOrth(double chi, const cv::Mat& img, cv::Mat& rotImg);
    
    
    //it is ineffecient to rotate dot one by one
    void rotateYOrthNearDot(double chi, int u, int v, int& ur, int& vr);
    void rotateYAng(double chi, const cv::Mat&img, cv::Mat& rotImg);
    void rotateYOrth(int chi, const cv::Mat& img, cv::Mat& rotImg);
    
    void rotateXAng(double chi, double theta, double phi,
                    double& thetar, double& phir);
    
    void writeRotateYMovie(double deltaChi, const cv::Mat& img, double angWidth,
                           const std::string& outputName, int frame);
    
private:
    const cv::Size* frameSize;
    const Transform* transform;
};

#endif /* Rotate_hpp */
