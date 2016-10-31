//
//  OpticalFlowEstimator.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "OpticalFlowEstimator.hpp"

cv::Mat OpticalFlowEstimator::getRotMat
(const cv::Mat &forImg, const cv::Mat &latImg) const
{
    return getRotMat(forImg, latImg, cv::Mat::eye(3, 3, CV_32FC1));
}

cv::Mat OpticalFlowEstimator::getRotMat
(const cv::Mat &forImg, const cv::Mat &latImg,
 const cv::Mat &froChgMat) const
{
    // 透視投影画像を取得
    cv::Mat forPersImg, latPersImg;
    per.getPersImg(forImg, forPersImg, froChgMat, true);
    per.getPersImg(latImg, latPersImg, froChgMat, true);
    
    // 透視投影座標の特徴点
    std::vector<cv::Point2f> forPerss, latPerss;
    cof.getOpticalFlow(forPersImg, latPersImg, forPerss, latPerss);
    
    // 正規化座標の特徴点
    std::vector<cv::Point2f> forNormals, latNormals;
    tf.pers2normal(forPerss, forNormals, per.getInParaMat());
    tf.pers2normal(latPerss, latNormals, per.getInParaMat());
    
    // 回転行列を推定
    cv::Mat mask;
    cv::Mat rotMat = epi.getRotMatEssMat(forNormals, latNormals, mask);
    
    // 回転行列の座標系を変更
    cv::Mat rotMatChg;
    Rotation::chgRotMatCoo(rotMat, froChgMat.inv(), rotMatChg);
    
    return rotMatChg;
}
