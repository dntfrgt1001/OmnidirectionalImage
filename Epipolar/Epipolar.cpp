//
//  Epipolar.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Epipolar.hpp"

cv::Mat Epipolar::getRotMatEssMat
(const std::vector<Normal> &forNormals,
 const std::vector<Normal> &latNormals, cv::Mat &mask) const
{
    // 閾値以下の数の特徴点
    if (forNormals.size() < numThre) {
        std::cout << "there are few matched points" << std::endl;
        return cv::Mat::zeros(3, 3, CV_32FC1);
    }
    
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
