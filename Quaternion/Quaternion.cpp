//
//  Quaternion.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/09/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Quaternion.hpp"

Quaternion::Quaternion()
{
    float angle = 0.0;
    cv::Vec3f axis(1.0, 1.0, 1.0);
    Quaternion(angle, axis);
}

Quaternion::Quaternion(const float angle, const cv::Vec3f& axis)
{
    float norm = cv::norm(axis);

    w = cosf(angle/2.0);
    x = axis[0]/norm * sinf(angle/2.0);
    y = axis[1]/norm * sinf(angle/2.0);
    z = axis[2]/norm * sinf(angle/2.0);
}

Quaternion::Quaternion
(const float w, const float x, const float y, const float z):
w(w), x(x), y(y),z(z)
{
    
}

std::ostream& operator<<(std::ostream& os, const Quaternion& quat)
{
    return (os << "[" << quat.w << "; " << quat.x << ", "
            << quat.y << ", " << quat.z << "]");
}

float Quaternion::norm(Quaternion &quat)
{
    return sqrtf(quat.w*quat.w+quat.x*quat.x+quat.y*quat.y+quat.z*quat.z);
}

void Quaternion::normalQuart(Quaternion &quat)
{
    float qnorm = norm(quat);
    
    quat.w /= qnorm; quat.x /= qnorm; quat.y /= qnorm; quat.z /= qnorm;
}


