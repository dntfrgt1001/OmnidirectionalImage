//
//  AffineStore.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "AffineStore.hpp"

AffineStore::AffineStore():
accMat(cv::Mat::eye(3, 3, CV_32FC1))
{}

AffineStore::~AffineStore()
{
}

void AffineStore::extractRotMatFromAffineMat
(const cv::Mat& affMat, cv::Mat& rotMat)
{
    rotMat = cv::Mat(3, 3, CV_32FC1);
    
    for (int u=0; u<3; u++) {
        for (int v=0; v<3; v++) {
            rotMat.at<float>(v, u) = affMat.at<double>(v, u);
        }
    }
    
//    affMat(cv::Range(0, 3), cv::Range(0, 3)).copyTo(rotMat);
}

void AffineStore::accRotMat(const cv::Mat &rotMat)
{
    accMat = accMat * rotMat;
}