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
matchFeature(matchFeature), affine(affine),
accMat(cv::Mat::eye(3, 3, CV_32FC1))
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
    
    // 回転行列を集積
    accMat = accMat * estRotMat;
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "estRotMat = " << std::endl << estRotMat << std::endl;
    std::cout << "accMat = " << std::endl << accMat << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    
    // 修正画像を出力
//    transform.rotateImgWithRotMat(latImg, modLatImg, estRotMat);
    transform.rotateImgWithRotMat(latImg, modLatImg, accMat);
    
}

void MatchMain::ModifyVideo(VideoReader &vr, VideoWriter &vw)
{
    const int stride = 3;
    
    int i = 0;
    
    cv::Mat stdImg;
    vr.hasNext();
//    vr.readImg(stdImg);
    
//    cv::namedWindow("standard image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
//    cv::namedWindow("current image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
//    cv::namedWindow("current modified image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
  
    cv::namedWindow("pre image");
    cv::namedWindow("cur image");
    cv::namedWindow("mod cur image");
    
    cv::Mat prevImg, curImg;
  
    vr.readImg(curImg);
    
    while (vr.hasNext()) {
        i++;
      
        prevImg = curImg.clone();
        
        vr.readImg(curImg);
        
        cv::Mat curModImg;
        ModifylatterImg(prevImg, curImg, curModImg);
        
        cv::imshow("pre image", prevImg);
        cv::imshow("cur image", curImg);
        cv::imshow("mod cur image", curModImg);
        
        vw.writeImg(curModImg);
        
//        cv::waitKey(-1);
        
        /*
        // 標準画像に合わせて現在の画像を修正
        cv::Mat curImg, modCurImg;
        vr.readImg(curImg);
        ModifylatterImg(stdImg, curImg, modCurImg);
        
        // 修正した現在の画像を出力
        vw.writeImg(modCurImg);
        
        // strideの間隔で標準画像を変更
        if (i % stride == 0) {
            modCurImg.copyTo(stdImg);
        }
        
        cv::imshow("standard image", stdImg);
        cv::imshow("current image", curImg);
        cv::imshow("current modified image", modCurImg);
         */
    }
}