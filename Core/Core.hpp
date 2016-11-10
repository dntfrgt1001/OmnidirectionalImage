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
    Equirect(const cv::Point2f& point2f): cv::Point2f(point2f){};
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
    
    template<class Tp>
    const Sphere operator*(const Tp value) {
        return Sphere(this->x * value,
                      this->y * value,
                      this->z * value);
    }
};

class Normal : public cv::Point2f
{
public:
    Normal(): cv::Point2f() {};
    Normal(float xn, float yn): cv::Point2f(xn, yn){};
    Normal(const cv::Point2f point2f): cv::Point2f(point2f) {};
};

class Pers : public cv::Point2f
{
public:
    Pers(): cv::Point2f() {};
    Pers(float xp, float yp): cv::Point2f(xp, yp){};
    Pers(const cv::Point2f point2f): cv::Point2f(point2f){};
};


#endif /* Core_hpp */
