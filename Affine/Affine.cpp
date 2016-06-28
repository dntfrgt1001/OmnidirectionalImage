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
    // RANSACの乱数用
    srand((unsigned int) time(NULL));
}

Affine::~Affine()
{
    
}

#include <iostream>
void Affine::estimate3DRotMat
(std::vector<cv::Point3f> &forPoints, std::vector<cv::Point3f> &latPoints,
 cv::Mat &estRotMat)
{
    // 三次元アフィン行列を推定
    cv::Mat estAffMat;
    estimate3DAffineMat(forPoints, latPoints, estAffMat);
    
    std::cout << "affine mat = " << std::endl << estAffMat << std::endl;
    
    // 回転行列を取り出す
    extractRotMatFromAffineMat(estAffMat, estRotMat);
    
    std::cout << "raw estRotMat = " << std::endl << estRotMat << std::endl;
    
    // 回転行列を正規化
    normalizeRotMat(estRotMat);
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

void Affine::normalizeRotMat(cv::Mat &rotMat)
{
    cv::Vec3f a, b, c;
    cv::Vec3f e1, e2, e3;
    cv::Vec3f f1, f2, f3;
    
    a = rotMat.col(0);
    b = rotMat.col(1);
    c = rotMat.col(2);
    
    f1 = a;
    e1 = 1.0/cv::norm(f1) * f1;
    
    f2 = b - b.dot(e1) * e1;
    e2 = 1.0/cv::norm(f2) * f2;
    
    f3 = c - c.dot(e1) * e1 - c.dot(e2) * e2;
    e3 = 1.0/cv::norm(f3) * f3;
    
    for (int v=0; v<3; v++) {
        rotMat.at<float>(v, 0) = e1[v];
        rotMat.at<float>(v, 1) = e2[v];
        rotMat.at<float>(v, 2) = e3[v];
    }
}

void Affine::estimate3DRotMatSVD
(std::vector<cv::Point3f> &forPoints, std::vector<cv::Point3f> &latPoints,
 cv::Mat& estRotMat)
{
    int sampleSize = 5;
    cv::Mat betRotMat = cv::Mat(3, 3, CV_32FC1);
    float betError = FLT_MAX;
    
    // ランダムにサンプリングした点から推定を行う
    for (int i=0; i<5; i++) {
        std::vector<cv::Point3f> forPointx, latPointx;
        getRondomPoint(forPoints, latPoints, forPointx, latPointx, sampleSize);
        
        cv::Mat curEstRotMat;
        estimate3DRotMatSVDPartial(forPointx, latPointx, curEstRotMat);
        
        float curError;
        curError = evalEstRotMat(forPoints, latPoints, curEstRotMat);
        std::cout << "current error = " << curError << std::endl;
        std::cout << "current mat = " << std::endl <<  curEstRotMat << std::endl;
        
        if (curError < betError) {
            betError = curError;
            curEstRotMat.copyTo(betRotMat);
        }
    }
    
    // 最も高いスコアの行列を用いてインライアーを求める
    std::vector<cv::Point3f> inForPoints, inLatPoints;
    removeOutlier
    (forPoints, latPoints, inForPoints, inLatPoints, betError/sampleSize);
    
    // インライアーから回転行列を推定する
    estRotMat = cv::Mat(3, 3, CV_32FC1);
    estimate3DRotMatSVDPartial(inForPoints, inLatPoints, estRotMat);
    
    
    /*
    cv::Mat corMat = cv::Mat::zeros(3, 3, CV_32FC1);
    
    for (int i=0; i<forPoints.size(); i++) {
        cv::Vec3f forVec = cv::Mat1f(forPoints[i]);
        cv::Vec3f latVec = cv::Mat1f(latPoints[i]);
        
        corMat = corMat + cv::Mat(forVec * latVec.t());
    }
    
    cv::Mat u, w, vt;
    cv::SVD::compute(corMat, w, u, vt);
    cv::Mat est = u * vt;
    
    estRotMat = cv::Mat(3, 3, CV_32FC1);
    est.copyTo(estRotMat);
     */
}

void Affine::getRondomPoint
(const std::vector<cv::Point3f> &forPoints,
 const std::vector<cv::Point3f> &latPoints,
 std::vector<cv::Point3f> &forPointx, std::vector<cv::Point3f> &latPointx,
 size_t getSize)
{
    forPointx.clear();
    latPointx.clear();
    
    for (int i=0; i<getSize; i++) {
        int randIdx = rand() % forPoints.size();
        
        forPointx.push_back(forPoints[randIdx]);
        latPointx.push_back(latPoints[randIdx]);
    }
}

void Affine::estimate3DRotMatSVDPartial
(const std::vector<cv::Point3f> &forPointx,
 const std::vector<cv::Point3f> &latPointx, cv::Mat &estRotMatPart)
{
    cv::Mat corMat = cv::Mat::zeros(3, 3, CV_32FC1);
    
    for (int i=0; i<forPointx.size(); i++) {
        cv::Vec3f forVec = cv::Mat1f(forPointx[i]);
        cv::Vec3f latVec = cv::Mat1f(latPointx[i]);
        
        corMat = corMat + cv::Mat(forVec * latVec.t());
    }
    
    cv::Mat u, w, vt;
    cv::SVD::compute(corMat, w, u, vt);
    
    cv::Mat est = u * vt;
    
    estRotMatPart = cv::Mat(3, 3, CV_32FC1);
    
    est.copyTo(estRotMatPart);
}

float Affine::evalEstRotMat
(const std::vector<cv::Point3f> &forPoints,
 const std::vector<cv::Point3f> &latPoints, const cv::Mat &estRotMat)
{
    float accErr = 0.0;
    for (int i=0; i<forPoints.size(); i++) {
        cv::Point3f modLatPoint;
        transform.orth3d2orth3dWithRotMat(latPoints[i], modLatPoint, estRotMat);
        
        accErr += cv::norm(forPoints[i] - modLatPoint);
    }
    
    return accErr;
}

void Affine::removeOutlier
(const std::vector<cv::Point3f> &forPoints,
 const std::vector<cv::Point3f> &latPoints,
 std::vector<cv::Point3f> &inForPoints, std::vector<cv::Point3f> &inLatPoints,
 float normThre)
{
    inForPoints.clear();
    inLatPoints.clear();
    
    for (int i=0; i<forPoints.size(); i++) {
        if (cv::norm(forPoints[i] - latPoints[i]) < normThre) {
            inForPoints.push_back(forPoints[i]);
            inLatPoints.push_back(latPoints[i]);
        }
    }
}

