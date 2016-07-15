//
//  Perspective.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/07/13.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Perspective.hpp"


void Perspective::persProjImg
(const cv::Mat &img, float rangeTheta, float rangePhi, cv::Mat &persedImg)
{
    float dud, dvd;
    calcPersLength(rangeTheta, rangePhi, dud, dvd);
    setPersCenter(dud / 2.0, dvd / 2.0);
    
    std::cout << -rangeTheta/2.0 << " <= " << rangeTheta/2.0 << std::endl;
    std::cout << -rangePhi/2.0 << " <= " << rangePhi/2.0 << std::endl;
    std::cout << "camera matrix = " << cameraMat << std::endl;
    
    int dudInt = (int) floorf(dud);
    int dvdInt = (int) floorf(dvd);

    std::cout << "width = " << dudInt << std::endl;
    std::cout << "height = " << dvdInt << std::endl;
    
    persedImg = cv::Mat(cv::Size(dudInt, dvdInt), CV_8UC3);
    
    for (int ud = 0; ud < dudInt; ud++) {
        for (int vd = 0; vd < dvdInt; vd++) {
            float x, y;
            pers2regular(ud, vd, x, y);
            
            float theta, phi;
            regular2pregular(x, y, theta, phi);
            
            float u, v;
            transform.ang2orth(theta, phi, u, v);
            
            cv::Vec3b dot;
            transform.getDotBilinear(img, u, v, dot);
            persedImg.at<cv::Vec3b>(vd, ud) = dot;
            
            
            if (ud == 0 && vd == 0) {
                std::cout << "(ud, vd) = (0, 0), " << "(theta, phi) = (" << theta << ", " << phi << ")" << std::endl;
            }
            if (ud == (int) dudInt/2 && vd == (int)dvdInt/2) {
                std::cout << "(ud, vd) = (rud/2, dvd/2), " << "(theta, phi) = (" << theta << ", " << phi << ")" << std::endl;
            }
            if (ud == dudInt - 1 && vd == dvdInt - 1) {
                std::cout << "(ud, vd) = (rud/2, dvd/2), " << "(theta, phi) = (" << theta << ", " << phi << ")" << std::endl;
            }
            
        }
    }
    
    cv::Mat distCoeffs = (cv::Mat_<float>(5, 1) <<
                          -0.2107004, 0.06231665, -0.0008263, -0.00171503, -0.0109820);
    
    cv::Mat tmp = persedImg.clone();
    
   // cv::undistort(tmp, persedImg, cameraMat, distCoeffs);
}