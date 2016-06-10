//
//  Affine.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/09.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Affine.hpp"

Affine::Affine(Transform& transform):
transform(transform), accMat(cv::Mat::eye(3, 3, CV_32FC1))
{
    
}

Affine::~Affine()
{
    
}

void Affine::extractRotMatFromAffineMat
(const cv::Mat &affMat, cv::Mat &rotMat)
{
    rotMat = cv::Mat(3, 3, CV_32FC1);
    
    for (int u=0; u<3; u++) {
        for (int v=0; v<3; v++) {
            rotMat.at<float>(v, u) = (float) affMat.at<double>(v, u);
        }
    }
}

void Affine::accRotMat(const cv::Mat &curRotMat)
{
    accMat = accMat * curRotMat;
}

void Affine::estimate3DAffineMat
(std::vector<cv::Point3f> &forPoints, std::vector<cv::Point3f> &latPoints,
 cv::Mat &affMat)
{
    std::vector<uchar> inliers;
    
    cv::estimateAffine3D(forPoints, latPoints, affMat, inliers);
}

void Affine::get3DPointPair
(const std::vector<cv::KeyPoint> &forKeyPoints,
 const std::vector<cv::KeyPoint> &latKeyPoints,
 const std::vector<cv::DMatch> &dMatches,
 std::vector<cv::Point3f> &for3DPoints, std::vector<cv::Point3f> &lat3DPoints)
{
    
    for (int i=0; i<dMatches.size(); i++) {
        cv::Point2f for2DPoint, lat2DPoint;
        cv::Point3f for3DPoint, lat3DPoint;
        
        // 回転前の3次元座標
        for2DPoint = forKeyPoints[dMatches[i].queryIdx].pt;
        transform.orth2d2orth3d(for2DPoint, for3DPoint);
        for3DPoints.push_back(for3DPoint);
        // 回転後の3次元座標
        lat2DPoint = latKeyPoints[dMatches[i].trainIdx].pt;
        transform.orth2d2orth3d(lat2DPoint, lat3DPoint);
        lat3DPoints.push_back(lat3DPoint);
    }
}
