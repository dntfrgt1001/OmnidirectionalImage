//
//  main.cpp
//  RotationTest
//
//  Created by masakazu nakazawa on 2016/01/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include "Transform.hpp"
#include "Quarternion.hpp"

int main(int argc, const char * argv[])
{
 
    const cv::Size frameSize(2000, 1000);
    
    Transform transform(frameSize);
    
    float theta = M_PI / 4.0;
    cv::Vec3f axis(0, 0, 1);
    
    cv::Mat rotMat;
    Quarternion::arbRotMat(theta, axis, rotMat);
    
    std::cout << "true R = " << std::endl << rotMat << std::endl;
    
    std::vector<cv::Point3f> forPoints, latPoints;
    cv::Point3f p1(1.0, 0.0, 0.0); forPoints.push_back(p1);
    cv::Point3f p2(0.0, 1.0, 0.0); forPoints.push_back(p2);
    cv::Point3f p3(0.0, 0.0, 1.0); forPoints.push_back(p3);
    
    for (int i=0; i<forPoints.size(); i++) {
        cv::Point3f tmpPoint;
        transform.orth3D2orth3DWithRotMat(forPoints[i], tmpPoint, rotMat);
        latPoints.push_back(tmpPoint);
    }
    
    cv::Mat accMat = cv::Mat::zeros(3, 3, CV_32FC1);
    for (int i=0; i<forPoints.size(); i++) {
        cv::Vec3f forVec = cv::Mat1f(forPoints[i]);
        cv::Vec3f latVec = cv::Mat1f(latPoints[i]);
        
        accMat = accMat + cv::Mat(latVec * forVec.t());
    }
    
    cv::Mat u, w, vt;
    cv::SVD::compute(accMat, w, u, vt);
    
    std::cout << "u = " << std::endl << u << std::endl;
    std::cout << "w = " << std::endl << w << std::endl;
    std::cout << "vt = " << std::endl << vt << std::endl;
    std::cout << "est R = " << std::endl << u * vt <<std::endl;
    
    return 0;
}
