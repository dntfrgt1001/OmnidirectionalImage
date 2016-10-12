//
//  main.cpp
//  AffineTest
//
//  Created by masakazu nakazawa on 2016/06/02.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "Rotation.hpp"

int main(int argc, const char * argv[])
{
    cv::Vec3f vec(-1.0, 2.0, -3.0);
    cv::Mat rotMat;
    
    Rotation::vec2zDirMat(vec, rotMat);
    
    std::cout << rotMat << std::endl;
    
    return 0;
}
