//
//  Range.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/17.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Range.hpp"

Range::Range(const float fovAngle):
fovRadius(tanf(fovAngle))
{
}

void Range::extFroSphere
(const std::vector<Sphere> &forspheres,
 const std::vector<Sphere> &latspheres,
 std::vector<Sphere> &forspheresFront,
 std::vector<Sphere> &latspheresFront) const
{
    for (int i = 0; i < forspheres.size(); i++) {
        if (isValidSpherePair(forspheres[i], latspheres[i])) {
            forspheresFront.push_back(forspheres[i]);
            latspheresFront.push_back(latspheres[i]);
        }
    }
}

void Range::extRotFroFeat
(const std::vector<cv::KeyPoint> &keyPoints, const cv::Mat &descriptors,
 const cv::Mat& froMat, std::vector<cv::KeyPoint> &keyPointsValid,
 cv::Mat &descriptorsValid) const
{
    descriptorsValid = cv::Mat (0, descriptors.cols, descriptors.type());
    
    for (int i = 0; i < keyPoints.size(); i++) {
        if (isInRange(keyPoints[i].pt, froMat)) {
            // 特徴点追加
            keyPointsValid.push_back(keyPoints[i]);
            // 記述子追加
            cv::vconcat
            (descriptorsValid, descriptors.row(i), descriptorsValid);
        }
    }
}


