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

#include "Transform.hpp"

class Range {
public:
    Range
    (const cv::Size& frameSize, const Transform& tf,
     const float rangeAngle);
    
    
    // 特徴点がカメラの前後にあるか
    bool isInRangeNormal(const cv::Point2f& normal) const {
        return normal.x*normal.x + normal.y*normal.y <
               rangeRadius*rangeRadius;
    }
    bool isInRangeSphere(const Sphere& sphere) const {
        Normal normal;
        tf.sphere2normal(sphere, normal);
        return isInRangeNormal(normal);
    }
    bool isInRangeEquirect
    (const Equirect& equirect) const {
        Sphere sphere;
        tf.equirect2sphere(equirect, sphere);
        return isInRangeSphere(sphere);
    }
    // マッチした特徴点の組が境界を跨いでいるか
    bool isStrideBorder
    (const cv::Point3f& forsphere, const cv::Point3f& latsphere) const {
        return forsphere.z * latsphere.z < 0;
    }
    // 推定に使う特徴点か(カメラ前後にあり境界を跨がない)
    bool isValidSpherePair
    (const Sphere& forsphere, const Sphere& latsphere) const {
        return isInRangeSphere(forsphere) && isInRangeSphere(latsphere) &&
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
        Sphere sphereRot;
        tf.rotateSphere(sphere, sphereRot, froMat);
        return isInRangeSphere(sphereRot);
    }
    bool isInRange
    (const Equirect& equirect, const cv::Mat& froMat) const {
        Equirect equirectRot;
        tf.rotateEquirect(equirect, equirectRot, froMat);
        return isInRangeEquirect(equirectRot);
    }
 
    // 回転後にカメラの前後にある特徴点を取り出す
    void extRotFroFeat
    (const std::vector<cv::KeyPoint>& keyPoints, const cv::Mat& descriptors,
     std::vector<cv::KeyPoint>& keyPointsValid, cv::Mat& descriptorsValid,
     const cv::Mat& froMat) const;

private:
    const cv::Size& fs;
    const Transform& tf;
    const float rangeRadius;
};

#endif /* Range_hpp */
