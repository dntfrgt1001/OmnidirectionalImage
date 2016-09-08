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

#include <opencv2/core.hpp>

class Quarternion
{
public:
    Quarternion();
    Quarternion(float theta, cv::Vec3f& axis);
    friend std::ostream& operator<<(std::ostream& os, const Quarternion& quart);
    
    // ノルム
    static float norm(Quarternion& quart);
    // 誤差を含む四元数を正規化
    static void normalQuart(Quarternion& quart);
    
    float t;
    float x;
    float y;
    float z;
};

#endif /* Quarternion_hpp */
