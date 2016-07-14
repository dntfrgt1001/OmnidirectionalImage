//
//  Perspective.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/07/13.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Perspective.hpp"

Perspective::Perspective(const Transform& transform, const cv::Mat& cameraMat):
transform(transform), cameraMat(cameraMat)
{
    
}

void Perspective::persProjImg
(const cv::Mat &img, cv::Mat &persedImg,
 float rangeTheta, float rangePhi) const
{
    float rtheta = rangeTheta / 2.0;
    float dphi = rangePhi / 2.0;
    
    float rx = pregular2x(rtheta, dphi);
    float dy = pregular2y(rtheta, dphi);
    
    int rud = x2ud(pregular2x(rtheta, dphi));
    int dvd = y2vd(pregular2y(rtheta, dphi));
    
    std::cout << "camera matrix = " << std::endl << cameraMat << std::endl;
    std::cout << "rx = " << rx << std::endl;
    std::cout << "dy = " << dy << std::endl;
    std::cout << "rud = " << rud <<std::endl;
    std::cout << "dvd = " << dvd <<std::endl;
}