//
//  MatchImagePair.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/15.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "MatchMain.hpp"

MatchMain::MatchMain
(const Transform& origTransform, const Transform& transform,
 const ExtractFeaturePoint& extractFeature,
 const MatchFeaturePoint& matchFeature, const Rotation& rotation):
origTransform(origTransform),transform(transform),
extractFeature(extractFeature),matchFeature(matchFeature), rotation(rotation),
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
    std::vector<cv::Point2f> forequirects, latequirects;
    matchFeature.sortMatchedPair
    (forKeyPoints, latKeyPoints, dMatches, forequirects, latequirects);

    // 三次元空間へのマッピング
    std::vector<cv::Point3f> forspheres, latspheres;
    transform.equirect2sphere(forequirects, forspheres);
    transform.equirect2sphere(latequirects, latspheres);
    
    // 特徴点の座標のユークリッド距離のフィルター
    matchFeature.filterCoordDistance(forspheres, latspheres);
    
    // 正規化画像座標へのマッピング
    std::vector<cv::Point2f> fornormals, latnormals;
    transform.sphere2normal(forspheres, fornormals);
    transform.sphere2normal(latspheres, latnormals);
    
    
    // 回転行列の推定
    cv::Mat estRotMat;
    rotation.estimate3DRotMatSVD(forspheres, latspheres, estRotMat);
    //rotation.estimate3DRotMatEssential(fornormals, latnormals, estRotMat);
    
    // 回転行列を集積
    accMat = accMat * estRotMat;
    // 集積した回転行列を正規化
    rotation.normalRotMat(accMat);
    // 回転行列を使って画像を修正
    origTransform.rotateImgWithRotMat(latImg, modLatImg, accMat);
    
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "# match = " << forspheres.size() << std::endl;
    std::cout << "estRotMat = " << std::endl << estRotMat << std::endl;
//    std::cout << "estRotMatEss = " << std::endl << estRotMatEss << std::endl;
    std::cout << "accRotMat = " << std::endl << accMat << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    
    std::vector<cv::Point2f> lastForequirect, lastLatequirect;
    transform.sphere2equirect(forspheres, lastForequirect);
    transform.sphere2equirect(latspheres, lastLatequirect);
    
    cv::Mat matchImg;
    matchFeature.drawMatchesVertical
    (resForImg, lastForequirect, resLatImg, lastLatequirect, matchImg);
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