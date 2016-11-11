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
 const cv::Mat &curRotMat) const
{
    // 直前の回転行列から正面を変更
    cv::Mat froChgMat;
    Rotation::getFroChgMat(curRotMat, froChgMat);
    
    //画像を縮小
    cv::Mat resForImg, resLatImg;
    tf.resizeImg(forImg, resForImg);
    tf.resizeImg(latImg, resLatImg);
    
    // 透視投影画像を取得
    cv::Mat forPersImg, latPersImg;
    per.getPersImg(resForImg, forPersImg, froChgMat, false);
    per.getPersImg(resLatImg, latPersImg, froChgMat, false);
    
    // 透視投影座標の特徴点
    std::vector<Pers> forPerss, latPerss;
    cof.getOpticalFlow(forPersImg, latPersImg, forPerss, latPerss);
    
    // 正規化座標の特徴点
    std::vector<Normal> forNormals, latNormals;
    tf.pers2normal(forPerss, forNormals, per.getInParaMat());
    tf.pers2normal(latPerss, latNormals, per.getInParaMat());
    
    // 回転方向が逆のものを削除
    cof.remRotOutlier(forNormals, latNormals);
    
    // 円周方向にないものを削除
    cof.remOrthOutlier(forNormals, latNormals);
    
    cv::Vec3f curRotVec;
    Rotation::RotMat2RotVec(curRotMat, curRotVec);
    const float rotAng = cv::norm(curRotVec);
    
    // ノルムがおかしいものを削除
    //cof.remNormOutlier(forNormals, latNormals, rotAng);
    
    // 回転行列を推定
    cv::Mat mask;
    cv::Mat rotMat = epi.getRotMatEssMat(forNormals, latNormals, mask);
    
    // オプティカルフロー描画
    std::vector<Pers> forPerssLast, latPerssLast;
    tf.normal2pers(forNormals, forPerssLast, per.getInParaMat());
    tf.normal2pers(latNormals, latPerssLast, per.getInParaMat());
    
    cv::Mat drawImg;
    cof.drawOpticalFlow(forPersImg, forPerssLast, latPerssLast, drawImg);
    cv::namedWindow("optical flow");
    cv::imshow("optical flow", drawImg);
    
    // 回転行列の座標系を変更
    cv::Mat rotMatChg;
    Rotation::chgRotMatCoo(rotMat, froChgMat.inv(), rotMatChg);
    
    return rotMatChg;
}
