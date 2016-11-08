//
//  Core.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Core_hpp
#define Core_hpp

#include <stdio.h>

#include <opencv2/core.hpp>

class Equirect : public cv::Point2f
{
public:
    Equirect(): cv::Point2f() {};
    Equirect(float u, float v): cv::Point2f(u, v){};
};

class Polar : public cv::Point2f
{
public:
    Polar(): cv::Point2f() {};
    Polar(float theta, float phi): cv::Point2f(theta, phi){};
};

class Sphere : public cv::Point3f
{
public:
    Sphere(): cv::Point3f() {};
    Sphere(float x, float y, float z): cv::Point3f(x, y, z){};
    
    Sphere& operator =(const Sphere& sphere) {
        cv::Point3f::operator=(sphere);
        return *this;
    }
    
    
    Sphere& operator *(const Sphere& sphere) {
        cv::Point3f::operator=(sphere); return *this;
    }
};

class Normal : public cv::Point2f
{
public:
    Normal(): cv::Point2f() {};
    Normal(float xn, float yn): cv::Point2f(xn, yn){};
};

class Pers : public cv::Point2f
{
public:
    Pers(): cv::Point2f() {};
    Pers(float xp, float yp): cv::Point2f(xp, yp){};
};

#endif /* Core_hpp */
