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
 const MatchFeaturePoint& mfp, const Estimate& est):
otf(otf), tf(tf), efp(efp), mfp(mfp), est(est),
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
    /*
    // 回転行列の推定
    cv::Mat estRotMat;
//    bool isEstimated = rot.estimateRotMat(forspheres, latspheres, estRotMat);
    bool isEstimated = est.estimateRotMat(forspheres, latspheres, estRotMat);
     */
    
    // 重み最大の方向のカメラ前後の特徴点を取得
    cv::Mat estRotMatMax;
    int maxIdx;
    est.estRotMatWeightMax(forspheres, latspheres, estRotMatMax, maxIdx);
    
    std::cout << "max weight = " << maxIdx << std::endl;
    
    cv::Mat estRotMat;
    if (maxIdx == -1) {
        std::cout << "*** There are few features." <<
        " Estimation failed. ***" << std::endl;
        preMat.copyTo(estRotMat);
    } else {
        cv::Vec3f estRotVecMax;
        Rotation::RotMat2RotVec(estRotMatMax, estRotVecMax);
        
        cv::Mat froRotMat;
        Rotation::vec2zDirMat(estRotVecMax, froRotMat);
        
        // 回転させて正面の特徴点を抽出
        std::vector<cv::KeyPoint> forKeyPointsMax, latKeyPointsMax;
        cv::Mat forDescriptorsMax, latDescriptorsMax;
        est.extRotFrontFeature
        (forKeyPoints, forDescriptors,
         forKeyPointsMax, forDescriptorsMax, froRotMat);
        est.extRotFrontFeature
        (latKeyPoints, latDescriptors,
         latKeyPointsMax, latDescriptorsMax, froRotMat);
        
        // 重み最大の方向でマッチング
        std::vector<cv::DMatch> dMatchesMax;
        mfp.match(forDescriptorsMax, latDescriptorsMax, dMatchesMax);
        mfp.filterMatchDistance(dMatchesMax);
    
        std::vector<cv::Point2f> forequirectsMax, latequirectsMax;
        mfp.sortMatchedPair
        (forKeyPointsMax, latKeyPointsMax, dMatchesMax,
         forequirectsMax, latequirectsMax);
    
        std::vector<cv::Point3f> forspheresMax, latspheresMax;
        tf.equirect2sphere(forequirectsMax, forspheresMax);
        tf.equirect2sphere(latequirectsMax, latspheresMax);
        mfp.filterCoordDistance(forspheresMax, latspheresMax);
    
        // 重み最大の方向で回転行列推定
        float weightMax;
        est.estRotMatSpecDir
        (forspheresMax, latspheresMax, froRotMat, estRotMat, weightMax);
        
        cv::Mat matchImgMax;
        mfp.drawMatchesVertical
        (resForImg, forequirectsMax, resLatImg, latequirectsMax, matchImgMax);
        cv::imshow("match max", matchImgMax);
    }
    
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
    Rotation::normalRotMat(accMat);
    // 回転行列を使って画像を修正
    otf.rotateImgWithRotMat(latImg, modLatImg, accMat);
    std::cout << "======================================" << std::endl;
    std::cout << "accMat = " << std::endl << accMat << std::endl;
    std::cout << "======================================" << std::endl;
    
    // マッチング表示用
    std::vector<cv::Point2f> lastForequirect, lastLatequirect;
    tf.sphere2equirect(forspheres, lastForequirect);
    tf.sphere2equirect(latspheres, lastLatequirect);
    
    cv::Mat matchImg;
    mfp.drawMatchesVertical
    (resForImg, lastForequirect, resLatImg, lastLatequirect, matchImg);
    cv::imshow("match", matchImg);
    
    // 次フレームのための特徴点と特徴量と回転行列の保存
    tmpKeyPoints.clear();
    std::copy(latKeyPoints.begin(), latKeyPoints.end(),
              std::back_inserter(tmpKeyPoints));
    latDescriptors.copyTo(tmpDescriptors);
    estRotMat.copyTo(preMat);
}

void MatchMain::ModifyVideo(VideoReader &vr, VideoWriter &vw)
{
    cv::namedWindow("previous image");
    cv::namedWindow("current image");
    cv::namedWindow("modified current image");
    cv::namedWindow("match");
    cv::namedWindow("match max");
    
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
        
        std::cout << i++ << "-th frame finished" << std::endl;
        
        cv::waitKey(-1);
    }
}

void MatchMain::ModifyVideoMid
(VideoReader &vrOriginal, VideoReader &vrPreFixed, VideoWriterPic &vw,
 const int frameNum, const cv::Mat &curRotMat)
{
    // 初期化処理
    for (int i=0; i<frameNum; i++) {
        // 姿勢修正済みのフレームを書き出す
        cv::Mat tmpMat;
        vrPreFixed.readImg(tmpMat);
        vw.writeImg(tmpMat);
        
        // 元動画から先頭フレームを取り出す
        vrOriginal.readImg(tmpMat);
    }
    
    // 直前の集積行列をセット
    curRotMat.copyTo(accMat);
    
    ModifyVideo(vrOriginal, vw);
}
