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

/*
    extractFeature.extractRectRoiFeaturePoint(forImg, forKeyPoints, forDescriptors);
    extractFeature.extractRectRoiFeaturePoint(latImg, latKeyPoints, latDescriptors);
 */
    extractFeature.extractFeaturePoint(forImg, forKeyPoints, forDescriptors);
    extractFeature.extractFeaturePoint(latImg, latKeyPoints, latDescriptors);
    
    // マッチング
    std::vector<cv::DMatch> dMatches;
    matchFeature.match(forDescriptors, latDescriptors, dMatches);
    
    // 記述子のマッチング距離のフィルター
    matchFeature.filterMatchDistance(dMatches);
    
    // 二次元画像の特徴点の座標のペアを取り出す
    std::vector<cv::Point2f> for2DPoints, lat2DPoints;
    matchFeature.sortMatchedPair
    (forKeyPoints, latKeyPoints, dMatches, for2DPoints, lat2DPoints);
    
//    matchFeature.filterMatchCoordinateDebug(forKeyPoints, latKeyPoints, dMatches);
    
    // 三次元空間へのマッピング
    std::vector<cv::Point3f> for3DPoints, lat3DPoints;
    transform.orth2d2orth3d(for2DPoints, for3DPoints);
    transform.orth2d2orth3d(lat2DPoints, lat3DPoints);
    
    // 特徴点の座標のユークリッド距離のフィルター
    matchFeature.filterMatchCoordinate(for3DPoints, lat3DPoints);
    
    // 回転行列の推定
    cv::Mat estRotMat;
//    affine.estimate3DRotMat(for3DPoints, lat3DPoints, estRotMat);
    affine.estimate3DRotMatSVD(for3DPoints, lat3DPoints, estRotMat);
    // 回転行列を集積    
    accMat = accMat * estRotMat;
    affine.normalizeRotMat(accMat);
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "match num = " << dMatches.size() << std::endl;
    std::cout << "estRotMat = " << std::endl << estRotMat << std::endl;
//    std::cout << "accMat = " << std::endl << accMat << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    

//    transform.rotateImgWithRotMat(latImg, modLatImg, estRotMat);
    transform.rotateImgWithRotMat(latImg, modLatImg, accMat);


    std::vector<cv::Point2f> lastFor2DPoints, lastLat2DPoints;
    transform.orth3d2orth2d(for3DPoints, lastFor2DPoints);
    transform.orth3d2orth2d(lat3DPoints, lastLat2DPoints);
    
    cv::Mat matchImg;
    matchFeature.drawMatchesVertical
    (forImg, lastFor2DPoints, latImg, lastLat2DPoints, matchImg);
    cv::imshow("match", matchImg);

}

void MatchMain::ModifyVideo(VideoReader &vr, VideoWriter &vw)
{
    cv::namedWindow("pre image");
    cv::namedWindow("cur image");
    cv::namedWindow("mod cur image");
    cv::namedWindow("match");

    cv::Mat preImg, curImg;
  
    vr.readImg(curImg);

    int i=0;
    while (vr.hasNext()) {
        preImg = curImg.clone();
        
        vr.readImg(curImg);
        
        cv::Mat curModImg;
        ModifylatterImg(preImg, curImg, curModImg);
        
        cv::imshow("pre image", preImg);
        cv::imshow("cur image", curImg);
        cv::imshow("mod cur image", curModImg);
        
        vw.writeImg(curModImg);
        
        std::cout << i << std::endl;
        
        cv::waitKey(-1);
        
        i++;
        
        
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