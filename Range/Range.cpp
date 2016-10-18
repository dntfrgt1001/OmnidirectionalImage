//
//  Range.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/17.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Range.hpp"

Range::Range
(const cv::Size& frameSize, const Transform& tf, const float rangeAngle):
fs(frameSize), tf(tf), rangeRadius(tanf(rangeAngle))
{
}

void Range::extFroSphere
(const std::vector<cv::Point3f> &forspheres,
 const std::vector<cv::Point3f> &latspheres,
 std::vector<cv::Point3f> &forspheresFront,
 std::vector<cv::Point3f> &latspheresFront) const
{
    for (int i = 0; i < forspheres.size(); i++) {
        if (isInFrontNotStride(forspheres[i], latspheres[i])) {
            forspheresFront.push_back(forspheres[i]);
            latspheresFront.push_back(latspheres[i]);
        }
    }
}

void Range::extRotFroFeat
(const std::vector<cv::KeyPoint> &keyPoints, const cv::Mat &descriptors,
 std::vector<cv::KeyPoint> &keyPointsValid, cv::Mat &descriptorsValid,
 const cv::Mat &froMat) const
{
    descriptorsValid = cv::Mat (0, descriptors.cols, descriptors.type());
    
    for (int i = 0; i < keyPoints.size(); i++) {
        if (isInFront(keyPoints[i].pt, froMat)) {
            // 特徴点追加
            keyPointsValid.push_back(keyPoints[i]);
            // 記述子追加
            cv::vconcat
            (descriptorsValid, descriptors.row(i), descriptorsValid);
        }
    }
}


