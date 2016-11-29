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
    float theta = 0.0;
    cv::Vec3f axis(1.0, 1.0, 1.0);
    Quaternion(theta, axis);
}

Quaternion::Quaternion(const float theta, const cv::Vec3f& axis)
{
    float norm = cv::norm(axis);

    t = cosf(theta/2.0);
    x = axis[0]/norm * sinf(theta/2.0);
    y = axis[1]/norm * sinf(theta/2.0);
    z = axis[2]/norm * sinf(theta/2.0);
}

Quaternion::Quaternion
(const float t, const float x, const float y, const float z):
t(t), x(x), y(y),z(z)
{
    
}

float Quaternion::norm(const Quaternion &quat)
{
    return sqrtf(quat.t*quat.t + quat.x*quat.x +
                 quat.y*quat.y + quat.z*quat.z);
}

void Quaternion::normalQuart(Quaternion &quat)
{
    float qnorm = norm(quat);
    
    quat.t /= qnorm; quat.x /= qnorm; quat.y /= qnorm; quat.z /= qnorm;
}

std::ostream& operator<<(std::ostream& os, const Quaternion& quat)
{
    return (os << "[" << quat.t << "; " << quat.x << ", "
            << quat.y << ", " << quat.z << "]");
}
