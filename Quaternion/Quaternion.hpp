//
//  Quaternion.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/09/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Quaternion_hpp
#define Quaternion_hpp

#include <stdio.h>
#include <iostream>

#include <math.h>

#include <opencv2/core.hpp>

class Quaternion
{
public:
    Quaternion();
    Quaternion(const float angle, const cv::Vec3f& axis);
    Quaternion(const float w, const float x, const float y, const float z);
    friend std::ostream& operator<<(std::ostream& os, const Quaternion& quat);
    
    // ノルム
    static float norm(const Quaternion& quat);
    // 誤差を含む四元数を正規化
    static void normalQuart(Quaternion& quat);
    
    // 共役の四元数
    static Quaternion conjugate(const Quaternion& quat) {
        return Quaternion(quat.w, -quat.x, -quat.y, -quat.z);
    }
    
    // 四元数の積
    Quaternion operator *(const Quaternion& quat) const {
        return
        Quaternion(
            this->w*quat.w - this->x*quat.x - this->y*quat.y - this->z*quat.z,
            this->x*quat.w + this->w*quat.x - this->z*quat.y + this->y*quat.z,
            this->y*quat.w + this->z*quat.x + this->w*quat.y - this->x*quat.z,
            this->z*quat.w - this->y*quat.x + this->x*quat.y + this->w*quat.z);
    }
    
    float w;
    float x;
    float y;
    float z;
};

#endif /* Quaternion_hpp */
