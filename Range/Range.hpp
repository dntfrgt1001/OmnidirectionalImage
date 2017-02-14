//
//  Range.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/17.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Range_hpp
#define Range_hpp

#include <stdio.h>
#include <iostream>

#include <opencv2/core.hpp>

//#include "Transform.hpp"
#include "Core.hpp"

class Range {
public:
    Range(const float fovAngle);
    
    // 特徴点がカメラの前方または後方にあるか
    bool isInRange(const Normal& normal) const {
        return normal.x*normal.x + normal.y*normal.y < fovRadius*fovRadius;
    }
    bool isInRange(const Sphere& sphere) const {
        return isInRange(Map::sphere2normal(sphere));
    }
    bool isInRange(const Equirect& equirect) const {
        return isInRange(Map::equirect2sphere(equirect));
    }
    
    // マッチした特徴点の組が境界を跨いでいるか
    bool isStrideBorder
    (const Sphere& forsphere, const Sphere& latsphere) const {
        return forsphere.z * latsphere.z < 0;
    }
    
    // 推定に使う特徴点か(カメラの前方または後方にあり境界を跨がない)
    bool isValidSpherePair
    (const Sphere& forsphere, const Sphere& latsphere) const {
        return isInRange(forsphere) && isInRange(latsphere) &&
               !isStrideBorder(forsphere, latsphere);
    }
    
    // 推定に使う特徴点の組を取り出す
    void extFroSphere
    (const std::vector<Sphere>& forspheres,
     const std::vector<Sphere>& latspheres,
     std::vector<Sphere>& forspheresFront,
     std::vector<Sphere>& latspheresFront) const;
    
    // 回転後の特徴点がカメラの前後にあるか
    bool isInRange
    (const Sphere& sphere, const cv::Mat& froMat) const {
        return isInRange(Map::rotateSphere(sphere, froMat));
    }
    bool isInRange
    (const Equirect& equirect, const cv::Mat& froMat) const {
        return isInRange(Map::rotateEquirect(equirect, froMat));
    }
 
    // 回転後にカメラの前後にある特徴点を取り出す
    void extRotFroFeat
    (const std::vector<cv::KeyPoint>& keyPoints, const cv::Mat& descriptors,
     const cv::Mat& froMat, std::vector<cv::KeyPoint>& keyPointsValid,
     cv::Mat& descriptorsValid) const;

private:
//    const cv::Size& fs;
//    const Transform& tf;
    const float fovRadius;
};

#endif /* Range_hpp */
