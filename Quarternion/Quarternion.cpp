//
//  Quarternion.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/10.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Quarternion.hpp"

Quarternion::Quarternion()
{
    float theta = 0.0;
    cv::Vec3f axis(1.0, 1.0, 1.0);
    Quarternion(theta, axis);
}

Quarternion::Quarternion(float theta, cv::Vec3f& axis)
{
    float norm = cv::norm(axis);
    
    t = cosf(theta/2.0);
    x = axis[0]/norm * sinf(theta/2.0);
    y = axis[1]/norm * sinf(theta/2.0);
    z = axis[2]/norm * sinf(theta/2.0);
}

float Quarternion::norm(Quarternion &quart)
{
    return sqrtf(quart.t*quart.t + quart.x*quart.x +
                 quart.y*quart.y + quart.z*quart.z);
}

void Quarternion::normalQuart(Quarternion &quart)
{
    float qnorm = norm(quart);
    
    quart.t /= qnorm; quart.x /= qnorm; quart.y /= qnorm; quart.z /= qnorm;
}

std::ostream& operator<<(std::ostream& os, const Quarternion& quart)
{
    return (os << "[" << quart.t << "; " << quart.x << ", "
            << quart.y << ", " << quart.z << "]");
}


