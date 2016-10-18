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

#include <opencv2/core.hpp>

#include "Transform.hpp"

class Range {
public:
    Range(const cv::Size& frameSize, const Transform& tf, const float rangeAngle);
    
    // 特徴点がカメラの前後にあるか
    bool isInFront(const cv::Point3f& sphere) const {
        return (sphere.x*sphere.x + sphere.y*sphere.y) <
                sphere.z*sphere.z * rangeRadius*rangeRadius;
    }
    bool isInFront
    (const cv::Point2f& equirect) const {
        cv::Point3f sphere; return isInFront(sphere);
    }
    // マッチした特徴点の組が境界を跨いでいるか
    bool isStrideBorder
    (const cv::Point3f& forsphere, const cv::Point3f& latsphere) const {
        return forsphere.z * latsphere.z < 0;
    }
    // 推定に使う特徴点か(カメラ前後にあり境界を跨がない)
    bool isInFrontNotStride
    (const cv::Point3f& forsphere, const cv::Point3f& latsphere) const {
        return isInFront(forsphere) && isInFront(latsphere) &&
               !isStrideBorder(forsphere, latsphere);
    }
    
    // 推定に使う特徴点の組を取り出す
    void extFroSphere
    (const std::vector<cv::Point3f>& forspheres,
     const std::vector<cv::Point3f>& latspheres,
     std::vector<cv::Point3f>& forspheresFront,
     std::vector<cv::Point3f>& latspheresFront) const;
    
    // 回転後の特徴点がカメラの前後にあるか
    bool isInFront
    (const cv::Point3f& sphere, const cv::Mat& froMat) const {
        cv::Point3f sphereRot;
        tf.sphereWithRotMat(sphere, sphereRot, froMat);
        return isInFront(sphereRot);
    }
    bool isInFront
    (const cv::Point2f& equirect, const cv::Mat& froMat) const {
        cv::Point2f equirectRot;
        tf.equirectWithRotMat(equirect, equirectRot, froMat);
        return isInFront(equirectRot);
    }
    bool isInFront
    (const cv::KeyPoint& keyPoint, const cv::Mat& froMat) const {
        return isInFront(keyPoint.pt, froMat);
    }
    // 回転後にカメラの前後にある特徴点を取り出す
    void extRotFroFeat
    (const std::vector<cv::KeyPoint>& keyPoints, const cv::Mat& descriptors,
     std::vector<cv::KeyPoint>& keyPointsValid, cv::Mat& descriptorsValid,
     const cv::Mat& froMat) const;
    
    // 画像の正面を塗りつぶす
    

private:
    const cv::Size& fs;
    const Transform& tf;
    const float rangeRadius;
};

#endif /* Range_hpp */
