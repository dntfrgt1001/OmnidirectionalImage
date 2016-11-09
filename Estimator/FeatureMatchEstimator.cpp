//
//  FeatureMatchEstimator.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "FeatureMatchEstimator.hpp"

FeatureMatchEstimator::FeatureMatchEstimator
(const Transform& tf, const ExtractFeaturePoint& efp,
 const MatchFeaturePoint& mfp, const Epipolar& epi, const Range& ran):
Estimator(tf), efp(efp), mfp(mfp), epi(epi), ran(ran)
{
    // カメラの正面を変える回転行列
    froChgMats.push_back((cv::Mat_<float>(3,3) << 1,0,0,0,1,0,0,0,1));
    froChgMats.push_back((cv::Mat_<float>(3,3) << 0,0,-1,0,1,0,1,0,0));
    froChgMats.push_back((cv::Mat_<float>(3,3) << 1,0,0,0,0,-1,0,1,0));
    
    float sqrt6 = sqrtf(6.0);
    float sqrt3 = sqrtf(3.0);
    float sqrt2 = sqrtf(2.0);
    froChgMats.push_back((cv::Mat_<float>(3,3) <<
                          2.0/sqrt6, -1.0/sqrt6, -1.0/sqrt6,
                          0.0, 1.0/sqrt2, -1.0/sqrt2,
                          1.0/sqrt3, 1.0/sqrt3, 1.0/sqrt3));
    froChgMats.push_back((cv::Mat_<float>(3,3) <<
                          -2.0/sqrt6, 1.0/sqrt6, -1.0/sqrt6,
                          0.0, 1.0/sqrt2, 1.0/sqrt2,
                          1.0/sqrt3, 1.0/sqrt3, -1.0/sqrt3));
};


cv::Mat FeatureMatchEstimator::getRotMat
(const cv::Mat &forImg, const cv::Mat &latImg) const
{
    // 画像サイズを縮小
    cv::Mat forImgRes, latImgRes;
    tf.resizeImg(forImg, forImgRes);
    tf.resizeImg(latImg, latImgRes);
    
    // グレースケールに変換
    cv::Mat forImgGray, latImgGray;
    cv::cvtColor(forImgRes, forImgGray, CV_BGR2GRAY);
    cv::cvtColor(latImgRes, latImgGray, CV_BGR2GRAY);
    
    // 特徴抽出
    std::vector<cv::KeyPoint> forKeyPointsSet, latKeyPointsSet;
    cv::Mat forDescs, latDescs;
    efp.extractFeaturePoint(forImgGray, forKeyPointsSet, forDescs);
    efp.extractFeaturePoint(latImgGray, latKeyPointsSet, latDescs);
    
    cv::Mat imgKeyPoint1;
    cv::Mat imgKeyPoint2;
    efp.drawKeyPoint(forImg, forKeyPointsSet, imgKeyPoint1);
    efp.drawKeyPoint(latImg, latKeyPointsSet, imgKeyPoint2);
    cv::namedWindow("KeyPoint 1");
    cv::namedWindow("KeyPoint 2");
    cv::imshow("KeyPoint 1", imgKeyPoint1);
    cv::imshow("KeyPoint 2", imgKeyPoint2);
    cv::waitKey();
    
    // マッチング＆フィルタリング
    std::vector<cv::DMatch> matchs;
    mfp.match(forDescs, latDescs, matchs);
    mfp.filterMatchDistance(matchs);
    
    // マッチした特徴点を得る
    std::vector<cv::KeyPoint> forKeyPoints, latKeyPoints;
    mfp.getMatchKeyPoint
    (forKeyPointsSet, latKeyPointsSet, matchs, forKeyPoints, latKeyPoints);
    
    // 画像座標に変換
    std::vector<Equirect> forEquirects(forKeyPoints.size());
    std::vector<Equirect> latEquirects(latKeyPoints.size());
    for (int i = 0; i < forKeyPoints.size(); i++) {
        forEquirects[i] = forKeyPoints[i].pt;
        latEquirects[i] = latKeyPoints[i].pt;
    }
    
    
    // 球面座標に変換
    std::vector<Sphere> forSpheres, latSpheres;
    tf.equirect2sphere(forEquirects, forSpheres);
    tf.equirect2sphere(latEquirects, latSpheres);
    
    mfp.filterCoordDistance(forSpheres, latSpheres);
    
    /*
    std::vector<cv::Point2f> forEquiLast, latEquiLast;
    tf.sphere2equirect(forSpheres, forEquiLast);
    tf.sphere2equirect(latSpheres, latEquiLast);
    cv::Mat imgMatch;
    mfp.drawMatchVert
    (forImg, forEquiLast, latImg, latEquiLast, imgMatch);
    cv::namedWindow("match");
    cv::imshow("match", imgMatch);
    cv::waitKey();
     */
    int maxIdx;
    return getRotMatWeightMax(forSpheres, latSpheres, maxIdx);
}

cv::Mat FeatureMatchEstimator::getRotMatSpecDir
(const std::vector<Sphere> &forSpheres,
 const std::vector<Sphere> &latSpheres,
 const cv::Mat &froChgMat, float &weight) const
{
    // カメラ正面を変更
    std::vector<Sphere> forSpheresRot, latSpheresRot;
    tf.rotateSphere(forSpheres, forSpheresRot, froChgMat);
    tf.rotateSphere(latSpheres, latSpheresRot, froChgMat);
    
    // 正面の特徴点を抽出
    std::vector<Sphere> forSpheresFro, latSpheresFro;
    ran.extFroSphere
    (forSpheresRot, latSpheresRot, forSpheresFro, latSpheresFro);
    
    // 正規化画像座標に変換
    std::vector<Normal> forNormals, latNormals;
    tf.sphere2normal(forSpheresFro, forNormals);
    tf.sphere2normal(latSpheresFro, latNormals);
    
    // 回転行列を推定
    cv::Mat mask;
    cv::Mat rotMat = epi.getRotMatEssMat(forNormals, latNormals, mask);
    weight = epi.getWeight(mask);
    
    // 重みで推定できているか判定
    if (weight <= 0) { return cv::Mat::zeros(3, 3, CV_32FC1); }
    else {
        cv::Mat rotMatChg;
        Rotation::chgRotMatCoo(rotMat, froChgMat.inv(), rotMatChg);

        std::cout << "--------------------------------------" << std::endl;
        std::cout << "weight = " << weight << std::endl;
        std::cout << "rotMat = " <<std::endl<< rotMat << std::endl;
        std::cout << "--------------------------------------" << std::endl;
    
        return rotMatChg;
    }
}

cv::Mat FeatureMatchEstimator::getRotMatWeightMax
(const std::vector<Sphere> &forSpheres,
 const std::vector<Sphere> &latSpheres, int &maxIdx) const
{
    std::vector<cv::Mat> rotMats(froChgMats.size());
    std::vector<float> weights(froChgMats.size());
    
    bool sucFlag = false;
    
    // ５方向で推定
    for (int i = 0; i < froChgMats.size(); i++) {
        rotMats[i] =
        getRotMatSpecDir
        (forSpheres, latSpheres, froChgMats[i], weights[i]);
        
        if (0 < weights[i]) sucFlag = true;
    }
    
    // どの方向でも推定に失敗
    if (!sucFlag) {
        maxIdx = -1;
        return cv::Mat::zeros(3, 3, CV_32FC1);
    }
    
    maxIdx = epi.getMaxWeightIndex(weights);
    
    /*
    // 回転軸方向で推定
    cv::Mat froChgMatAxis;
    Rotation::getFroChgMat(rotMats[maxIdx], froChgMatAxis);
    cv::Mat rotMatAxis;
    float weightAxis;
    rotMatAxis =
    getRotMatSpecDir(forSpheres, latSpheres, froChgMatAxis, weightAxis);
    */
    
    return rotMats[maxIdx];
}
