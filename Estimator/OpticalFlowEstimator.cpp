//
//  OpticalFlowEstimator.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "OpticalFlowEstimator.hpp"

cv::Mat OpticalFlowEstimator::getRotMat
(const cv::Mat &forImg, const cv::Mat &latImg, const State &state)
{
    return getRotMat(forImg, latImg, state.curRotMat);
}

cv::Mat OpticalFlowEstimator::getRotMat
(const cv::Mat &forImg, const cv::Mat &latImg, const cv::Mat &curRotMat)
{
    // 直前の回転行列から正面を変更
    cv::Mat froChgMat = Rotation::getFroChgMat(curRotMat);
    
    // -----------------------------------------------------
    // 前方の処理
    const bool front = true;
    cv::Mat forPersImgFront, latPersImgFront;
    per.getPersImg(forImg, froChgMat, front, forPersImgFront);
    per.getPersImg(latImg, froChgMat, front, latPersImgFront);
    
    std::vector<Pers> forPerss, latPerss;
    cof.getOpticalFlow
    (forPersImgFront, latPersImgFront, forPerss, latPerss);
    // -----------------------------------------------------
    // 後方の処理
    const bool back = false;
    cv::Mat forPersImgBack, latPersImgBack;
    per.getPersImg(forImg, froChgMat, back, forPersImgBack);
    per.getPersImg(latImg, froChgMat, back, latPersImgBack);

    std::vector<Pers> forPerssBack, latPerssBack;
    cof.getOpticalFlow
    (forPersImgBack, latPersImgBack, forPerssBack, latPerssBack);
    // -----------------------------------------------------
    
    // 座標を統合
    std::copy(forPerssBack.begin(), forPerssBack.end(),
              std::back_inserter(forPerss));
    std::copy(latPerssBack.begin(), latPerssBack.end(),
              std::back_inserter(latPerss));
    
    // 正規化座標の特徴点
    std::vector<Normal> forNormals, latNormals;
    Map::pers2normal(forPerss, forNormals, per.getInParaMat());
    Map::pers2normal(latPerss, latNormals, per.getInParaMat());
    
    // 回転方向が逆のものを削除
    cof.remRotOutlier(forNormals, latNormals);
    
    // 円周方向にないものを削除
    cof.remOrthOutlier(forNormals, latNormals);

    // ノルムがおかしいものを削除
    //cv::Vec3f curRotVec = Rotation::RotMat2RotVec(curRotMat);
    //const float rotAng = cv::norm(curRotVec);
    //cof.remNormOutlier(forNormals, latNormals, rotAng);
    
    // 回転行列を推定
    cv::Mat mask;
    cv::Mat rotMat = epi.getRotMatEssMat(forNormals, latNormals, mask);
    
    float weight = epi.getWeight(mask);
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "weight = " << weight << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    
    // オプティカルフロー描画
    std::vector<Pers> forPerssLast, latPerssLast;
    Map::normal2pers(forNormals, forPerssLast, per.getInParaMat());
    Map::normal2pers(latNormals, latPerssLast, per.getInParaMat());
    
    cv::Mat drawImg;
    cof.drawOpticalFlow(forPersImgFront, forPerssLast, latPerssLast, drawImg);
    cv::namedWindow("optical flow");
    cv::imshow("optical flow", drawImg);
    
    // 回転行列の座標系を変更
    return Rotation::chgRotMatCoo(rotMat, froChgMat.inv());
}



