//
//  Estimate.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/09/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Estimate_hpp
#define Estimate_hpp

#include <stdio.h>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>

#include "Transform.hpp"
#include "Rotation.hpp"

class Estimate {
public:
    Estimate(const Transform& tf, float fieldAngle, int numThre);

    // 回転行列の推定
    bool estimateRotMat
    (const std::vector<cv::Point3f>& forspheres,
     const std::vector<cv::Point3f>& latspheres, cv::Mat& estRotMat) const;
    // 基本行列の分解により回転行列を推定する
    void estimate3DRotMatEssential
    (const std::vector<cv::Point2f>& fornormals,
     const std::vector<cv::Point2f>& latnormals, cv::Mat& estRotMat,
     cv::Mat& mask) const;
    // カメラの前後の特徴点を取り出す
    void extractFrontFeature
    (const std::vector<cv::Point3f>& forspheres,
     const std::vector<cv::Point3f>& latspheres,
     std::vector<cv::Point3f>& forspheresFront,
     std::vector<cv::Point3f>& latspheresFront) const;
    
    // 特徴点(の組)がカメラの前後にあるか
    bool isInFront
    (const cv::Point3f& forsphere, const cv::Point3f& latsphere)const {
        return (forsphere.z * latsphere.z > 0) &&
                isInFront(forsphere) && isInFront(latsphere);
    };
    // 特徴点(単体)がカメラの前後にあるか
    bool isInFront(const cv::Point3f& sphere) const {
        return (sphere.x*sphere.x + sphere.y*sphere.y) <
               sphere.z*sphere.z * fieldRadius*fieldRadius;
    }
    
    // 最大スコアの方向に特徴点が含まれるか
    bool isInFrontRotFeature
    (const cv::KeyPoint& keyPoint, const int rotIdx) const {
        cv::Point3f sphere, sphereRot;
        tf.equirect2sphere(keyPoint.pt, sphere);
        tf.rotateSphere(sphere, sphereRot, rotMats[rotIdx]);

        return isInFront(sphereRot);
    }
    
    // 最大スコアの方向の特徴点と記述子を抽出する
    void extRotFrontFeature
    (const std::vector<cv::KeyPoint>& keyPoints,
     const cv::Mat& descriptors,
     std::vector<cv::KeyPoint>& keyPointsRotFront,
     cv::Mat& descriptorsRotFront,
     const int rotIdx) const {
        descriptorsRotFront = cv::Mat(0, 128, CV_32F);
        
        for (int i=0; i<keyPoints.size(); i++) {
            if (isInFrontRotFeature(keyPoints[i], rotIdx)) {
                // 特徴点を抽出
                keyPointsRotFront.push_back(keyPoints[i]);
                // 記述子を抽出
                cv::vconcat
                (descriptorsRotFront, descriptors.row(i),
                 descriptorsRotFront);
            }
        }
    }
    
    
    // 最終的な回転角，回転軸を決定
    void integrateRotVec
    (const std::vector<cv::Vec3f>& rotVecs, std::vector<float>& weights,
     cv::Vec3f& rotVec) const;

    // それぞれの方向で推定された回転の重みを返す
    float getWeight(cv::Mat& mask) const;
    
    // 重み最大の方向を返す
    int getMaxWeightIndex(std::vector<float>& weights) const;
    
    // 重み最大の方向を返す
    int getMaxWeightIndex
    (const std::vector<cv::Point3f>& forspheres,
     const std::vector<cv::Point3f>& latspheres) const;
    
    void estimateRotMatSpecDir
    (const std::vector<cv::Point3f>& forspheres,
     const std::vector<cv::Point3f>& latspheres,
     const int rotIdx, cv::Mat estRotMat) const;
    
private:
    std::vector<cv::Mat> rotMats;
    const Transform& tf;
    const float fieldRadius;
    const int numThre;
    
    /*
     // 特異値分解により回転行列を推定する [forP] = [R]*[latP]
     void estimate3DRotMatSVD
     (const std::vector<cv::Point3f>& forspheres,
     const std::vector<cv::Point3f>& latspheres, cv::Mat& estRotMat) const;
     // ランダムに点を取り出す
     static void getRondomPoint
     (const std::vector<cv::Point3f>& forPoints,
     const std::vector<cv::Point3f>& latPoints,
     std::vector<cv::Point3f>& forPointx, std::vector<cv::Point3f>& latPointx,
     size_t getSize);
     // ランダムに取り出した少数の点から回転行列を推定
     static void estimate3DRotMatSVDPartial
     (const std::vector<cv::Point3f>& forPointx,
     const std::vector<cv::Point3f>& latPointx, cv::Mat& estRotMatPart);
     // 推定した回転行列を評価
     float evalEstRotMat
     (const std::vector<cv::Point3f>& forPoints,
     const std::vector<cv::Point3f>& latPoints,
     const cv::Mat& estRotMat) const;
     // 閾値以下のサンプルを取り出す
     size_t removeOutlier
     (const std::vector<cv::Point3f>& forPoints,
     const std::vector<cv::Point3f>& latPoints,
     std::vector<cv::Point3f>& inForPoints,
     std::vector<cv::Point3f>& inLatPoints,
     float normThre, const cv::Mat& estRotMat) const;
     */
};

#endif /* Estimate_hpp */
