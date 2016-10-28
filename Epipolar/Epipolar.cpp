//
//  Epipolar.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Epipolar.hpp"

Epipolar::Epipolar
(const Transform& tf, const Range& rg, int numThre):
tf(tf), rg(rg), numThre(numThre)
{
    // カメラの正面を変える回転行列
    froChgMats.push_back((cv::Mat_<float>(3,3) << 1,0,0,0,1,0,0,0,1));
    froChgMats.push_back((cv::Mat_<float>(3,3) << 0,0,-1,0,1,0,1,0,0));
    froChgMats.push_back((cv::Mat_<float>(3,3) << 1,0,0,0,0,-1,0,1,0));
    
    // 斜め方向に変換
    float sqrt6 = sqrtf(6.0);
    float sqrt3 = sqrtf(3.0);
    float sqrt2 = sqrtf(2.0);
    froChgMats.push_back((cv::Mat_<float>(3,3) <<
                          2.0/sqrt6, -1.0/sqrt6, -1.0/sqrt6,
                          0.0, 1.0/sqrt2, -1.0/sqrt2,
                          1.0/sqrt3, 1.0/sqrt3, 1.0/sqrt3));
    froChgMats.push_back((cv::Mat_<float>(3,3) <<
                          -2.0/sqrt6, 1.0/sqrt6, -1.0/sqrt6,
                          0.0, 1.0/sqrt2, 1.0/sqrt2,
                          1.0/sqrt3, 1.0/sqrt3, -1.0/sqrt3));
}

cv::Mat Epipolar::getRotMatEssMat
(const std::vector<cv::Point2f> &forNormals,
 const std::vector<cv::Point2f> &latNormals, cv::Mat &mask) const
{
    const double focal = 1.0;
    const cv::Point2d pp(0.0, 0.0);
    const int method = cv::RANSAC;
    //int method = cv::LMEDS;
    double prob = 0.999;
    double threshold = 0.01;
    
    // 後フレーム->前フレームの基本行列
    //cv::Mat E = cv::findEssentialMat(latnormals, fornormals, focal, pp, method);
    cv::Mat E =
    cv::findEssentialMat
    (latNormals, forNormals, focal, pp, method, prob, threshold, mask);
    
    // 後フレーム->前フレームの回転行列
    cv::Mat R, t;
    cv::recoverPose(E, latNormals, forNormals, R, t);
    
    cv::Mat rotMat;
    R.convertTo(rotMat, CV_32FC1);
    
    return rotMat;
}

cv::Mat Epipolar::getRotMatSpecDir
(const std::vector<cv::Point3f> &forSpheres,
 const std::vector<cv::Point3f> &latSpheres,
 const cv::Mat &froChgMat, float &weight) const
{
    // カメラ正面を変更
    std::vector<cv::Point3f> forSpheresRot, latSpheresRot;
    tf.rotateSphere(forSpheres, forSpheresRot, froChgMat);
    tf.rotateSphere(latSpheres, latSpheresRot, froChgMat);
    
    // 正面の特徴点を抽出
    std::vector<cv::Point3f> forSpheresFro, latSpheresFro;
    rg.extFroSphere(forSpheresRot, latSpheresRot, forSpheresFro, latSpheresFro);
    
    // 正規化画像座標に変換
    std::vector<cv::Point2f> forNormals, latNormals;
    tf.sphere2normal(forSpheresFro, forNormals);
    tf.sphere2normal(latSpheresFro, latNormals);
    
    if (forNormals.size() < numThre) {
        weight = -1.0;
        return cv::Mat::zeros(3, 3, CV_32FC1);
    } else {
        std::cout << "--------------------------------------" << std::endl;
        
        // カメラ正面変更後の回転行列
        cv::Mat rotMatFro, mask;
        rotMatFro = getRotMatEssMat(forNormals, latNormals, mask);
        weight = getWeight(mask);
        
        // 回転ベクトルを介して元カメラ座標系における回転行列に修正
        cv::Vec3f rotVecFro;
        Rotation::RotMat2RotVec(rotMatFro, rotVecFro);
        cv::Vec3f rotVec;
        rotVec = cv::Vec3f(cv::Mat1f(froChgMat.inv() * cv::Mat1f(rotVecFro)));
        cv::Mat rotMat;
        Rotation::RotVec2RotMat(rotVec, rotMat);
        
        std::cout << "weight = " << weight << std::endl;
        std::cout << "rotMat = " <<std::endl<< rotMat << std::endl;
        std::cout << "--------------------------------------" << std::endl;
        
        return rotMat;
    }
}

cv::Mat Epipolar::getRotMatWeightMax
(const std::vector<cv::Point3f> &forSpheres,
 const std::vector<cv::Point3f> &latSpheres, int &maxIdx) const
{
    std::vector<cv::Mat> rotMats(froChgMats.size());
    std::vector<float> weights(froChgMats.size());
    
    bool sucFlag = false;
    
    for (int i = 0; i < froChgMats.size(); i++) {
        rotMats[i] =
        getRotMatSpecDir(forSpheres, latSpheres, froChgMats[i], weights[i]);
        
        if (0 <= weights[i]) sucFlag = true;
    }
    
    if (!sucFlag) {
        maxIdx = -1;
        return cv::Mat::zeros(3, 3, CV_32FC1);
    }
    
    maxIdx = getMaxWeightIndex(weights);
    return rotMats[maxIdx];
}

float Epipolar::getWeight(const cv::Mat &mask) const
{
    int count = 0;
    for (int v = 0; v < mask.rows; v++) {
        if (mask.at<uchar>(v, 0) != 0) count++;
    }
    
    std::cout << "(all, inlier) = (" << mask.rows
              << ", " << count << ")" << std::endl;
    
    return (float) count;
}

int Epipolar::getMaxWeightIndex(const std::vector<float> &weights) const
{
    std::cout << "weights = ";
    for (int i = 0; i < weights.size(); i++) {
        std::cout << weights[i] << " ";
    }
    std::cout << std::endl;
    
    std::vector<float>::const_iterator itr =
    std::max_element(weights.begin(), weights.end());
    
    return (int) std::distance(weights.begin(), itr);
}