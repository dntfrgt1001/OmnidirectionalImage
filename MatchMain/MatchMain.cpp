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
 const MatchFeaturePoint& mfp, const Estimate& est, const Range& rg,
 const OpticalFlow& of):
otf(otf), tf(tf), efp(efp), mfp(mfp), est(est), rg(rg), of(of),
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
    
    // グレースケール
    cv::Mat forGrayImg, latGrayImg;
    cv::cvtColor(resForImg, forGrayImg, CV_BGR2GRAY);
    cv::cvtColor(resLatImg, latGrayImg, CV_BGR2GRAY);
    
    // 特徴の抽出
    std::vector<cv::KeyPoint> forKeyPoints, latKeyPoints;
    cv::Mat forDescriptors, latDescriptors;
    
    if (tmpKeyPoints.size() == 0) {
        // 初期フレームの場合
        //efp.extractFeaturePoint(resForImg, forKeyPoints, forDescriptors);
        efp.extractFeaturePoint(forGrayImg, forKeyPoints, forDescriptors);
    } else {
        // 初期フレーム以外
        forKeyPoints = tmpKeyPoints;
        forDescriptors = tmpDescriptors;
    }

    // 次フレームの特徴の抽出
    //efp.extractFeaturePoint(resLatImg, latKeyPoints, latDescriptors);
    efp.extractFeaturePoint(latGrayImg, latKeyPoints, latDescriptors);

    
    // 推定に使用する特徴点座標を取り出す
    std::vector<cv::Point3f> forSpheres, latSpheres;
    getMatchCoord
    (forKeyPoints, forDescriptors, latKeyPoints, latDescriptors,
     forSpheres, latSpheres);
    
    // 重み最大の方向のカメラ前後の特徴点を取得
    cv::Mat estRotMatMax;
    int maxIdx;
    est.estRotMatWeightMax(forSpheres, latSpheres, estRotMatMax, maxIdx);
    
    std::cout << "max weight = " << maxIdx << std::endl;
    std::cout << "max RotMat = " << std::endl
              << estRotMatMax << std::endl;
    
    cv::Mat estRotMat;
    if (maxIdx == -1) {
        std::cout << "*** There are few features." <<
        " Estimation failed. ***" << std::endl;
        preMat.copyTo(estRotMat);
    } else {
        // 回転ベクトル取得
        cv::Vec3f estRotVecMax;
        Rotation::RotMat2RotVec(estRotMatMax, estRotVecMax);
        // 回転軸方向の回転行列
        cv::Mat froRotMat;
        Rotation::vec2zDirMat(estRotVecMax, froRotMat);
        
        std::cout << "front Matrix = " << std::endl <<
                    froRotMat << std::endl;
        
        // 回転させて正面の特徴点を抽出
        std::vector<cv::KeyPoint> forKeyPointsMax, latKeyPointsMax;
        cv::Mat forDescriptorsMax, latDescriptorsMax;
        
        rg.extRotFroFeat
        (forKeyPoints, forDescriptors,
         forKeyPointsMax, forDescriptorsMax, froRotMat);
        rg.extRotFroFeat
        (latKeyPoints, latDescriptors,
         latKeyPointsMax, latDescriptorsMax, froRotMat);
        
        std::vector<cv::Point3f> forspheresMax, latspheresMax;
        getMatchCoord
        (forKeyPointsMax, forDescriptorsMax, latKeyPointsMax, latDescriptorsMax,
         forspheresMax, latspheresMax);
        
        // 重み最大の方向で回転行列推定
        float weightMax;
        est.estRotMatSpecDir
        (forspheresMax, latspheresMax, froRotMat, estRotMat, weightMax);
        
        std::vector<cv::Point2f> forequirectsMaxLast, latequirectsMaxLast;
        tf.sphere2equirect(forspheresMax, forequirectsMaxLast);
        tf.sphere2equirect(latspheresMax, latequirectsMaxLast);
        
        cv::Mat matchImgMax;
        mfp.drawMatchesVertical
        (resForImg, forequirectsMaxLast,
         resLatImg, latequirectsMaxLast, matchImgMax);
        cv::imshow("match max", matchImgMax);
        
        cv::Mat estRotMatOpt;
        of.estRotMatFromOpticalFlow
        (resForImg, resLatImg, froRotMat, estRotMatOpt);
        
        std::cout << "opt rot mat = " << std::endl << estRotMatOpt <<
        std::endl;
        
        estRotMatOpt.copyTo(estRotMat);
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
    otf.rotateImg(latImg, modLatImg, accMat);
    std::cout << "======================================" << std::endl;
    std::cout << "accMat = " << std::endl << accMat << std::endl;
    std::cout << "======================================" << std::endl;
    
    // マッチング表示用
    std::vector<cv::Point2f> lastForequirect, lastLatequirect;
    tf.sphere2equirect(forSpheres, lastForequirect);
    tf.sphere2equirect(latSpheres, lastLatequirect);
    
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
//    cv::namedWindow("previous image");
//    cv::namedWindow("current image");
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
        
//        cv::imshow("previous image", preImg);
//        cv::imshow("current image", curImg);
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


void MatchMain::getMatchCoord
(const std::vector<cv::KeyPoint> &forKeyPoints, const cv::Mat &forDescriptors,
 const std::vector<cv::KeyPoint> &latKeyPoints, const cv::Mat &latDescriprots,
 std::vector<cv::Point3f> &forSpheres, std::vector<cv::Point3f> &latSpheres)
{
    // マッチング
    std::vector<cv::DMatch> dMatchs;
    mfp.match(forDescriptors, latDescriprots, dMatchs);
    
    // マッチング距離のフィルタリング
    mfp.filterMatchDistance(dMatchs);
    
    // 画像座標の特徴点座標の組を取り出す
    std::vector<cv::Point2f> forEquirects, latEquirects;
    mfp.sortMatchedPair
    (forKeyPoints, latKeyPoints, dMatchs, forEquirects, latEquirects);
    
    // 球面座標へマッピング
    tf.equirect2sphere(forEquirects, forSpheres);
    tf.equirect2sphere(latEquirects, latSpheres);
    
    // 特徴点座標の組のユークリッド距離のフィルタリング
    mfp.filterCoordDistance(forSpheres, latSpheres);
}
