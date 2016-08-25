//
//  MatchImagePair.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/15.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "MatchMain.hpp"

MatchMain::MatchMain
(const Transform& otf, const Transform& tf, const ExtractFeaturePoint& efp,
 const MatchFeaturePoint& mfp, const Rotation& rot):
otf(otf), tf(tf), efp(efp), mfp(mfp), rot(rot),
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
    tf.resizeImg(forImg, resForImg);
    tf.resizeImg(latImg, resLatImg);
    
    // 特徴点と記述子の抽出
    std::vector<cv::KeyPoint> forKeyPoints, latKeyPoints;
    cv::Mat forDescriptors, latDescriptors;

    
    if (tmpKeyPoints.size() == 0) {
        // 初期フレームの場合
        efp.extractFeaturePoint(resForImg, forKeyPoints, forDescriptors);
    } else {
        // 初期フレーム以外
        forKeyPoints = tmpKeyPoints;
        forDescriptors = tmpDescriptors;
    }
    
//    efp.extractFeaturePoint(resForImg, forKeyPoints, forDescriptors);
    efp.extractFeaturePoint(resLatImg, latKeyPoints, latDescriptors);
    
    // マッチング
    std::vector<cv::DMatch> dMatches;
    mfp.match(forDescriptors, latDescriptors, dMatches);
    
    // 記述子のマッチング距離のフィルター
    mfp.filterMatchDistance(dMatches);
    
    // 二次元画像の特徴点の座標のペアを取り出す
    std::vector<cv::Point2f> forequirects, latequirects;
    mfp.sortMatchedPair
    (forKeyPoints, latKeyPoints, dMatches, forequirects, latequirects);
    
    // 球面座標へのマッピング
    std::vector<cv::Point3f> forspheres, latspheres;
    tf.equirect2sphere(forequirects, forspheres);
    tf.equirect2sphere(latequirects, latspheres);
    
    // 特徴点の座標のユークリッド距離のフィルター
    mfp.filterCoordDistance(forspheres, latspheres);

    /*
    // 回転行列の推定
    cv::Mat estRotMat;
    //rotation.estimate3DRotMatSVD(forspheres, latspheres, estRotMat);
    rotation.estimate3DRotMatEssential(fornormals, latnormals, estRotMat);
    */
    
    cv::Mat estRotMat;
    rot.estimateRotMat(forspheres, latspheres, estRotMat);
    
    /*
    // 境界をまたぐマッチングを削除
    for (int i=0; i<forspheres.size(); ) {
        if (forspheres[i].z * latspheres[i].z < 0) {
            forspheres.erase(forspheres.begin()+i);
            latspheres.erase(latspheres.begin()+i);
        } else {
            i++;
        }
    }
     // 正規化画像座標へのマッピング
    std::vector<cv::Point2f> fornormals, latnormals;
    tf.sphere2normal(forspheres, fornormals);
    tf.sphere2normal(latspheres, latnormals);
    cv::Mat estRotMat;
    rot.estimate3DRotMatEssential(fornormals, latnormals, estRotMat);
    */
    
    
    // 回転行列を集積
    accMat = accMat * estRotMat;
    // 集積した回転行列を正規化
    rot.normalRotMat(accMat);
    // 回転行列を使って画像を修正
    otf.rotateImgWithRotMat(latImg, modLatImg, accMat);
    
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "# match = " << forspheres.size() << std::endl;
    std::cout << "curRotMat = " << std::endl << estRotMat << std::endl;
    std::cout << "accRotMat = " << std::endl << accMat << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    
    std::vector<cv::Point2f> lastForequirect, lastLatequirect;
    tf.sphere2equirect(forspheres, lastForequirect);
    tf.sphere2equirect(latspheres, lastLatequirect);
    
    cv::Mat matchImg;
    mfp.drawMatchesVertical
    (resForImg, lastForequirect, resLatImg, lastLatequirect, matchImg);
    cv::imshow("match", matchImg);
    
    // 次フレームのための特徴点と特徴量の保存
    tmpKeyPoints.clear();
    std::copy
    (latKeyPoints.begin(), latKeyPoints.end(), std::back_inserter(tmpKeyPoints));
    latDescriptors.copyTo(tmpDescriptors);
    std::cout << "vector size = " << tmpKeyPoints.size() << std::endl;
    std::cout << "mat size = " << tmpDescriptors.rows << ", " << tmpDescriptors.cols << std::endl;
}

void MatchMain::ModifyVideo(VideoReader &vr, VideoWriter &vw)
{
    cv::namedWindow("previous image");
    cv::namedWindow("current image");
    cv::namedWindow("modified current image");
    cv::namedWindow("match");

    cv::Mat preImg, curImg;
  
    vr.readImg(curImg);
    vw.writeImg(curImg);
    std::cout << "0-th frame finished" <<std::endl;
    
    int i=1;
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