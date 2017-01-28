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
    
    //画像を縮小
    cv::Mat resForImg, resLatImg;
    tf.resizeImg(forImg, resForImg);
    tf.resizeImg(latImg, resLatImg);
    
    // 透視投影画像を取得 正面
    cv::Mat forPersImgFront, latPersImgFront;
    per.getPersImg(resForImg, forPersImgFront, froChgMat, true);
    per.getPersImg(resLatImg, latPersImgFront, froChgMat, true);
    // 透視投影座標の特徴点 正面
    std::vector<Pers> forPerss, latPerss;
    cof.getOpticalFlow(forPersImgFront, latPersImgFront, forPerss, latPerss);
    
    // 透視投影画像を取得 背面
    cv::Mat forPersImgBack, latPersImgBack;
    per.getPersImg(resForImg, forPersImgBack, froChgMat, false);
    per.getPersImg(resLatImg, latPersImgBack, froChgMat, false);
    // 当地投影座標の特徴点 背面
    std::vector<Pers> forPerssBack, latPerssBack;
    cof.getOpticalFlow(forPersImgBack, latPersImgBack, forPerssBack, latPerssBack);
    
    // 結合
    std::copy(forPerssBack.begin(), forPerssBack.end(),
              std::back_inserter(forPerss));
    std::copy(latPerssBack.begin(), latPerssBack.end(),
              std::back_inserter(latPerss));
    
    // 正規化座標の特徴点
    std::vector<Normal> forNormals, latNormals;
    tf.pers2normal(forPerss, forNormals, per.getInParaMat());
    tf.pers2normal(latPerss, latNormals, per.getInParaMat());
    
    // 回転方向が逆のものを削除
    cof.remRotOutlier(forNormals, latNormals);
    
    // 円周方向にないものを削除
    cof.remOrthOutlier(forNormals, latNormals);
    
    cv::Vec3f curRotVec = Rotation::RotMat2RotVec(curRotMat);
    const float rotAng = cv::norm(curRotVec);
    
    // ノルムがおかしいものを削除
    cof.remNormOutlier(forNormals, latNormals, rotAng);
    
    // 回転行列を推定
    cv::Mat mask;
    cv::Mat rotMat = epi.getRotMatEssMat(forNormals, latNormals, mask);
    
    // オプティカルフロー描画
    std::vector<Pers> forPerssLast, latPerssLast;
    tf.normal2pers(forNormals, forPerssLast, per.getInParaMat());
    tf.normal2pers(latNormals, latPerssLast, per.getInParaMat());
    
    cv::Mat drawImg;
    cof.drawOpticalFlow(forPersImgFront, forPerssLast, latPerssLast, drawImg);
    cv::namedWindow("optical flow");
    cv::imshow("optical flow", drawImg);
    
//    std::stringstream ss;
//    ss << "/Users/masakazu/Desktop/Jack/opt/" << "image" <<
//        std::setw(4) << std::setfill('0') << frameNum << ".jpg";
//    cv::imwrite(ss.str(), drawImg);
    
    // 回転行列の座標系を変更
    return Rotation::chgRotMatCoo(rotMat, froChgMat.inv());
}



