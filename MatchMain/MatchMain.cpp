//
//  MatchImagePair.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/15.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "MatchMain.hpp"

MatchMain::MatchMain
(Transform& transform, ExtractFeaturePoint& extractFeature,
 MatchFeaturePoint& matchFeature, Affine& affine):
transform(transform), extractFeature(extractFeature),
matchFeature(matchFeature), affine(affine)
{
    
}

MatchMain::~MatchMain()
{
    
}

void MatchMain::ModifylatterImg
(const cv::Mat &forImg, const cv::Mat &latImg, cv::Mat &modLatImg)
{
    // 特徴点と記述子の抽出
    std::vector<cv::KeyPoint> forKeyPoints, latKeyPoints;
    cv::Mat forDescriptors, latDescriptors;
    extractFeature.extractRectRoiFeaturePoint
    (forImg, forKeyPoints, forDescriptors);
    extractFeature.extractRectRoiFeaturePoint
    (latImg, latKeyPoints, latDescriptors);
    
    // マッチング
    std::vector<cv::DMatch> dMatches;
    matchFeature.match(forDescriptors, latDescriptors, dMatches);
    
    // 記述子のマッチング距離のフィルター
    matchFeature.filterMatchDistance(dMatches);
    
    // 三次元空間へのマッピング
    std::vector<cv::Point3f> for3DPoints, lat3DPoints;
    affine.get3DPointPair
    (forKeyPoints, latKeyPoints, dMatches, for3DPoints, lat3DPoints);
    
    // 特徴点の座標のユークリッド距離のフィルター
    matchFeature.filterMatchCoordinate(for3DPoints, lat3DPoints);
    
    // 回転行列の推定
    cv::Mat estRotMat;
    affine.estimate3DRotMat(for3DPoints, lat3DPoints, estRotMat);
    
    // 修正画像を出力
    transform.rotateImgWithRotMat(latImg, modLatImg, estRotMat);
}