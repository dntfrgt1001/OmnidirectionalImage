//
//  Rotation.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/07/04.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Rotation.hpp"

Rotation::Rotation(const Transform& transform):
transform(transform)
{
    // RANSACの乱数用
    srand((unsigned int) time(NULL));
}

Rotation::~Rotation(){}

void Rotation::estimate3DRotMatSVD
(const std::vector<cv::Point3f> &forPoints,
 const std::vector<cv::Point3f> &latPoints, cv::Mat& estRotMat) const
{
    int sampleSize = 6;
    cv::Mat betRotMat = cv::Mat(3, 3, CV_32FC1);
    size_t betInlierSize = 0;
    std::vector<cv::Point3f> betInForPoints, betInLatPoints;
    
//    float betError = FLT_MAX;
    
    // ランダムにサンプリングした点から推定を行う
    for (int i=0; i<sampleSize; i++) {
        std::vector<cv::Point3f> forPointx, latPointx;
        getRondomPoint(forPoints, latPoints, forPointx, latPointx, sampleSize);
        
        cv::Mat curEstRotMat;
        estimate3DRotMatSVDPartial(forPointx, latPointx, curEstRotMat);
        
        float error;
        error = evalEstRotMat(forPoints, latPoints, curEstRotMat);
//        std::cout << "current error = " << curError << std::endl;
//        std::cout << "current mat = " << std::endl <<  curEstRotMat << std::endl;
        
        std::vector<cv::Point3f> inForPoints, inLatPoints;
        size_t inLierSize =  removeOutlier(forPoints, latPoints, inForPoints, inLatPoints, error/forPoints.size(), curEstRotMat);
        
        if (inLierSize > betInlierSize) {
            betInlierSize = inLierSize;
            curEstRotMat.copyTo(betRotMat);
            betInForPoints.clear();
            std::copy
            (inForPoints.begin(), inForPoints.end(),
             std::back_inserter(betInForPoints));
            betInLatPoints.clear();
            std::copy
            (inLatPoints.begin(), inLatPoints.end(),
             std::back_inserter(betInLatPoints));
        }
        
        std::cout <<  i << "-th sample (error, inlier) = ("
        << error << ", " << inLierSize << ")" << std::endl;
        std::cout << "mat = " << std::endl << curEstRotMat << std::endl;
        
        /*
        if (curError < betError) {
            betError = curError;
            curEstRotMat.copyTo(betRotMat);
        }
         */
    }
    /*
    // 最も高いスコアの行列を用いてインライアーを求める
    std::vector<cv::Point3f> inForPoints, inLatPoints;
    removeOutlier
    (forPoints, latPoints, inForPoints, inLatPoints,
     betError/forPoints.size(), betRotMat);
    */
    
    std::cout << "(all, inlier) = (" << forPoints.size() << ", "
              << betInlierSize << ")" << std::endl;
    
    // インライアーから回転行列を推定する
    estRotMat = cv::Mat(3, 3, CV_32FC1);
    estimate3DRotMatSVDPartial(betInForPoints, betInLatPoints, estRotMat);
}

void Rotation::getRondomPoint
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

void Rotation::estimate3DRotMatSVDPartial
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

float Rotation::evalEstRotMat
(const std::vector<cv::Point3f> &forPoints,
 const std::vector<cv::Point3f> &latPoints, const cv::Mat &estRotMat) const
{
    float accErr = 0.0;
    for (int i=0; i<forPoints.size(); i++) {
        cv::Point3f modLatPoint;
        transform.orth3D2orth3DWithRotMat(latPoints[i], modLatPoint, estRotMat);
        
        accErr += cv::norm(forPoints[i] - modLatPoint);
    }
    
    return accErr;
}

size_t Rotation::removeOutlier
(const std::vector<cv::Point3f> &forPoints,
 const std::vector<cv::Point3f> &latPoints,
 std::vector<cv::Point3f> &inForPoints, std::vector<cv::Point3f> &inLatPoints,
 float normThre, const cv::Mat &estRotMat) const
{
    inForPoints.clear();
    inLatPoints.clear();
    
    std::cout << "thre = " << normThre << std::endl;
    
    for (int i=0; i<forPoints.size(); i++) {
        cv::Point3f modLatPoint;
        transform.orth3D2orth3DWithRotMat(latPoints[i], modLatPoint, estRotMat);
        
        // 閾値以下ならインライアーに加える
        if (cv::norm(forPoints[i] - modLatPoint) < normThre) {
            inForPoints.push_back(forPoints[i]);
            inLatPoints.push_back(latPoints[i]);
        }
    }
    
    return inForPoints.size();
}