//
//  FeatureMatchEstimator.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "FeatureMatchEstimator.hpp"

cv::Mat FeatureMatchEstimator::getRotMat
(const cv::Mat &forImg, const cv::Mat &latImg) const
{
    // 画像サイズを縮小
    cv::Mat forImgRes, latImgRes;
    tf.resizeImg(forImg, forImgRes);
    tf.resizeImg(latImg, latImgRes);
    
    // グレースケールに変換
    cv::Mat forImgGray, latImgGray;
    cv::cvtColor(forImgRes, forImgGray, CV_BGR2GRAY);
    cv::cvtColor(latImgRes, latImgGray, CV_BGR2GRAY);
    
    // 特徴抽出
    std::vector<cv::KeyPoint> forKeyPointsSet, latKeyPointsSet;
    cv::Mat forDescs, latDescs;
    efp.extractFeaturePoint(forImgGray, forKeyPointsSet, forDescs);
    efp.extractFeaturePoint(latImgGray, latKeyPointsSet, latDescs);
    
    // マッチング＆フィルタリング
    std::vector<cv::DMatch> matchs;
    mfp.match(forDescs, latDescs, matchs);
    mfp.filterMatchDistance(matchs);
    
    // マッチした特徴点を得る
    std::vector<cv::KeyPoint> forKeyPoints, latKeyPoints;
    mfp.getMatchKeyPoint
    (forKeyPointsSet, latKeyPointsSet, matchs, forKeyPoints, latKeyPoints);
    
    // 画像座標に変換
    std::vector<cv::Point2f> forEquirects(forKeyPoints.size());
    std::vector<cv::Point2f> latEquirects(latKeyPoints.size());
    for (int i = 0; i < forKeyPoints.size(); i++) {
        forEquirects[i] = forKeyPoints[i].pt;
        latEquirects[i] = latKeyPoints[i].pt;
    }
    
    // 球面座標に変換
    std::vector<cv::Point3f> forSpheres, latSpheres;
    tf.equirect2sphere(forEquirects, forSpheres);
    tf.equirect2sphere(latEquirects, latSpheres);
    
    int maxIdx;
    return epi.getRotMatWeightMax(forSpheres, latSpheres, maxIdx);
}