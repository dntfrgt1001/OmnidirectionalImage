//
//  MatchImagePair.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/15.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "MatchMain.hpp"

MatchMain::MatchMain
(const Transform& origTransform, const Transform& transform, const ExtractFeaturePoint& extractFeature,
 const MatchFeaturePoint& matchFeature, const Rotation& rotation):
origTransform(origTransform),
transform(transform), extractFeature(extractFeature),
matchFeature(matchFeature), rotation(rotation),
accMat(cv::Mat::eye(3, 3, CV_32FC1))
{
    
}

MatchMain::~MatchMain()
{
    
}

void MatchMain::ModifylatterImg
(const cv::Mat &forImg, const cv::Mat &latImg, cv::Mat &modLatImg)
{
    // 画像サイズを縮小
    cv::Mat resForImg, resLatImg;
    transform.resizeImg(forImg, resForImg);
    transform.resizeImg(latImg, resLatImg);
    
    // 特徴点と記述子の抽出
    std::vector<cv::KeyPoint> forKeyPoints, latKeyPoints;
    cv::Mat forDescriptors, latDescriptors;

    extractFeature.extractFeaturePoint(resForImg, forKeyPoints, forDescriptors);
    extractFeature.extractFeaturePoint(resLatImg, latKeyPoints, latDescriptors);
    
    // マッチング
    std::vector<cv::DMatch> dMatches;
    matchFeature.match(forDescriptors, latDescriptors, dMatches);
    
    // 記述子のマッチング距離のフィルター
    matchFeature.filterMatchDistance(dMatches);
    
    // 二次元画像の特徴点の座標のペアを取り出す
    std::vector<cv::Point2f> for2DPoints, lat2DPoints;
    matchFeature.sortMatchedPair
    (forKeyPoints, latKeyPoints, dMatches, for2DPoints, lat2DPoints);
    
    
    
    // 正規化画像座標へマッピング
    std::vector<cv::Point2f> forNorm2DPoints, latNorm2DPoints;
    
    
    
    
    // 三次元空間へのマッピング
    std::vector<cv::Point3f> for3DPoints, lat3DPoints;
    transform.orth2D2orth3D(for2DPoints, for3DPoints);
    transform.orth2D2orth3D(lat2DPoints, lat3DPoints);
    
    // 特徴点の座標のユークリッド距離のフィルター
    matchFeature.filterCoordDistance(for3DPoints, lat3DPoints);
    
    // 回転行列の推定
    cv::Mat estRotMat;
    rotation.estimate3DRotMatSVD(for3DPoints, lat3DPoints, estRotMat);
    // 回転行列を集積    
    accMat = accMat * estRotMat;
    // 集積した回転行列を正規化
    rotation.normalRotMat(accMat);
    // 回転行列を使って画像を修正
    origTransform.rotateImgWithRotMat(latImg, modLatImg, accMat);
    
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "# match = " << for3DPoints.size() << std::endl;
    std::cout << "estRotMat = " << std::endl << estRotMat << std::endl;
    std::cout << "accRotMat = " << std::endl << accMat << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    
    std::vector<cv::Point2f> lastFor2DPoints, lastLat2DPoints;
    transform.orth3D2orth2D(for3DPoints, lastFor2DPoints);
    transform.orth3D2orth2D(lat3DPoints, lastLat2DPoints);
    
    cv::Mat matchImg;
    matchFeature.drawMatchesVertical
    (resForImg, lastFor2DPoints, resLatImg, lastLat2DPoints, matchImg);
    cv::imshow("match", matchImg);
}

void MatchMain::ModifyVideo(VideoReader &vr, VideoWriter &vw)
{
    cv::namedWindow("previous image");
    cv::namedWindow("current image");
    cv::namedWindow("modified current image");
    cv::namedWindow("match");

    cv::Mat preImg, curImg;
  
    vr.readImg(curImg);

    int i=0;
    while (vr.hasNext()) {
        preImg = curImg.clone();
        
        vr.readImg(curImg);
        
        cv::Mat curModImg;
        ModifylatterImg(preImg, curImg, curModImg);
        
        cv::imshow("previous image", preImg);
        cv::imshow("current image", curImg);
        cv::imshow("modified current image", curModImg);
        
        vw.writeImg(curModImg);
        
        std::cout << i++ << "-th frame finished" <<std::endl;
        
        cv::waitKey(-1);
    }
}