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

#include <math.h>

#include <opencv2/core.hpp>

class Quaternion
{
public:
    Quaternion();
    Quaternion(const float theta, const cv::Vec3f& axis);
    Quaternion(const float t, const float x, const float y, const float z);
    friend std::ostream& operator<<(std::ostream& os, const Quaternion& quat);
    
    // ノルム
    static float norm(Quaternion& quat);
    // 誤差を含む四元数を正規化
    static void normalQuart(Quaternion& quat);
    
    float t;
    float x;
    float y;
    float z;
};

#endif /* Quaternion_hpp */
