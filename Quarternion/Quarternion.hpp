//
//  Quarternion.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/10.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Quarternion_hpp
#define Quarternion_hpp

#include <stdio.h>
#include <math.h>
#include <opencv2/core/core.hpp>

class Quarternion
{
public:
    Quarternion(float theta, cv::Vec3f& axis);
    ~Quarternion();
    void transCounterRot();
//    static float getNorm(float xcomp, float ycomp, float zcomp);
    static void Quart2RotMat(const Quarternion& quart, cv::Mat& rotMat);
//    static void RotMat2Quart(const cv::Mat& rotMat, Quarternion& quart);
    static void arbRotMat
    (float theta, cv::Vec3f& axis, cv::Mat& rotMat);
    
private:
    float t;
    float x;
    float y;
    float z;
};

#endif /* Quarternion_hpp */
