//
//  main.cpp
//  Test
//
//  Created by masakazu nakazawa on 2016/10/21.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>

#include "Transform.hpp"
#include "Perspective.hpp"
#include "OpticalFlow.hpp"
#include "Quaternion.hpp"
#include "Rotation.hpp"

int main(int argc, const char * argv[])
{
    float angle1 = M_PI / 2.0;
    cv::Vec3f axis1(1.0, 0.0, 0.0);
    Quaternion quat1(angle1, axis1);
    
    float angle2 = M_PI / 2.0;
    cv::Vec3f axis2(1.0, 0.0, 0.0);
    Quaternion quat2(angle2, axis2);
    
    std::cout << quat2 * quat1 << std::endl;

    
    return 0;
}
