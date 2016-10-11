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

int main(int argc, const char * argv[])
{
//    cv::Mat mat1 = (cv::Mat_<int>(0, 2) << 1,2,3,4);
    cv::Mat mat1 = cv::Mat_<int>(0, 2);
    cv::Mat mat2 = (cv::Mat_<int>(2, 2) << 5,6,7,8);
    
    cv::Mat mat3;
    
    std::cout << mat2.row(1) << std::endl;
    cv::vconcat(mat1, mat2.row(1), mat3);
    
    std::cout << "mat1 = " << std::endl << mat1 << std::endl;
    std::cout << "mat2 = " << std::endl << mat2 << std::endl;
    std::cout << "mat3 = " << std::endl << mat3 << std::endl;
    
    return 0;
}
