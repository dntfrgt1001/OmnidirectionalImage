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
Estimator(tf), efp(efp), mfp(mfp), epi(epi), ran(ran), prevFrameNum(-1)
{
    // カメラの正面を変える回転行列
    froChgMats.push_back((cv::Mat_<float>(3,3) << 1,0,0,0,1,0,0,0,1));  // z 0
    froChgMats.push_back((cv::Mat_<float>(3,3) << 0,0,-1,0,1,0,1,0,0)); // y pi/2
    froChgMats.push_back((cv::Mat_<float>(3,3) << 1,0,0,0,0,-1,0,1,0)); // x pi/2
//    froChgMats.push_back((cv::Mat_<float>(3,3) << -1,0,0,0,-1,0,0,0,1));// z pi
//    froChgMats.push_back((cv::Mat_<float>(3,3) << -1,0,0,0,1,0,0,0,-1));// y pi
//    froChgMats.push_back((cv::Mat_<float>(3,3) << 1,0,0,0,-1,0,0,0,-1));// x pi
    
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
(const cv::Mat &forImg, const cv::Mat &latImg, const State &state)
{
    return getRotMat(forImg, latImg, state.frameNum);
}

cv::Mat FeatureMatchEstimator::getRotMat
(const cv::Mat &forImg, const cv::Mat &latImg, const int frameNum)
{
    // 画像サイズを縮小
    cv::Mat forImgRes, latImgRes;
    tf.resizeImg(forImg, forImgRes);
    tf.resizeImg(latImg, latImgRes);
    
    // グレースケールに変換
    cv::Mat forImgGray, latImgGray;
    cv::cvtColor(forImgRes, forImgGray, CV_BGR2GRAY);
    cv::cvtColor(latImgRes, latImgGray, CV_BGR2GRAY);

    
    struct timeval featStart, featEnd;
    gettimeofday(&featStart, NULL);
    
    
    // 特徴抽出
    std::vector<cv::KeyPoint> forKeyPointSet, latKeyPointSet;
    cv::Mat forDescSet, latDescSet;
    // 直前も特徴マッチ法なら直前フレームの特徴を使いまわし
    if (frameNum - prevFrameNum == 1) {
        forKeyPointSet = curKeyPointSet;
        forDescSet = curDescSet;
    } else {
        efp.extFeat(forImgGray, forKeyPointSet, forDescSet);
    }
    
    // 後フレームの特徴抽出
    efp.extFeat(latImgGray, latKeyPointSet, latDescSet);
    
    
    
    gettimeofday(&featEnd, NULL);
    double featTime = (double) (featEnd.tv_sec - featStart.tv_sec) +
                      (featEnd.tv_usec - featStart.tv_usec) * 1e-6;
    std::cout << "feat time = " << featTime << std::endl;
    
    /*
    cv::Mat imgKeyPoint1;
    cv::Mat imgKeyPoint2;
    efp.drawKeyPoint(forImgRes, forKeyPointsSet, imgKeyPoint1);
    efp.drawKeyPoint(latImgRes, latKeyPointsSet, imgKeyPoint2);
    cv::namedWindow("KeyPoint 1");
    cv::namedWindow("KeyPoint 2");
    cv::imshow("KeyPoint 1", imgKeyPoint1);
    cv::imshow("KeyPoint 2", imgKeyPoint2);
    cv::waitKey();
    */
    
    
    struct timeval matchStart, matchEnd;
    gettimeofday(&matchStart, NULL);
    
    
    // マッチング＆フィルタリング
    std::vector<cv::DMatch> matchs;
    mfp.match(forDescSet, latDescSet, matchs);
    mfp.filterMatchDistance(matchs);
    
    // マッチした特徴点を並べる
    std::vector<cv::KeyPoint> forKeyPoints, latKeyPoints;
    mfp.getMatchKeyPoint
    (forKeyPointSet, latKeyPointSet, matchs,
     forKeyPoints, latKeyPoints);
    
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
    
    // ３次元空間上の距離でフィルタ
    mfp.filterCoordDistance(forSpheres, latSpheres);
    
    gettimeofday(&matchEnd, NULL);
    double matchTime = (double)(matchEnd.tv_sec - matchStart.tv_sec) +
                       (matchEnd.tv_usec - matchStart.tv_usec) * 1e-6;
    std::cout << "match time = " << matchTime << std::endl;
    
/*
    std::vector<Equirect> forEquiLast, latEquiLast;
    tf.sphere2equirect(forSpheres, forEquiLast);
    tf.sphere2equirect(latSpheres, latEquiLast);
    cv::Mat imgMatch;
    mfp.drawMatchVert
    (forImg, forEquiLast, latImg, latEquiLast, imgMatch);
    cv::namedWindow("match");
    cv::imshow("match", imgMatch);
    cv::waitKey();
  */
    
    // 現在フレーム番号の保存
    prevFrameNum = frameNum;
    // 現在フレームの特徴を保存
    //curKeyPointSet.clear();
    //std::copy(latKeyPointSet.begin(), latKeyPointSet.end(),
    //          std::back_inserter(curKeyPointSet));
    curKeyPointSet = latKeyPointSet;
    latDescSet.copyTo(curDescSet);

    
    /*
    std::vector<Sphere> fs1, fs2, fs3, fs4;
    std::vector<Sphere> ls1, ls2, ls3, ls4;
    for (int i = 0; i < forSpheres.size(); i++) {
        if (forSpheres[i].z > 0 && latSpheres[i].z > 0) {
            fs1.push_back(forSpheres[i]);
            ls1.push_back(latSpheres[i]);
        } else if (forSpheres[i].z < 0 && latSpheres[i].z < 0) {
            fs2.push_back(forSpheres[i]);
            ls2.push_back(latSpheres[i]);
        } else if (forSpheres[i].z > 0 && latSpheres[i].z < 0) {
            fs3.push_back(forSpheres[i]);
            ls3.push_back(latSpheres[i]);
        } else if (forSpheres[i].z < 0 && latSpheres[i].z > 0) {
            fs4.push_back(forSpheres[i]);
            ls4.push_back(latSpheres[i]);
        }
    }
    
    std::vector<Normal> forNormals, latNormals;
    tf.sphere2normal(forSpheres, forNormals);
    tf.sphere2normal(latSpheres, latNormals);
    
    std::vector<Normal> fn1, fn2, fn3, fn4;
    std::vector<Normal> ln1, ln2, ln3, ln4;
    tf.sphere2normal(fs1, fn1);
    tf.sphere2normal(ls1, ln1);
    
    tf.sphere2normal(fs2, fn2);
    tf.sphere2normal(ls2, ln2);
    
    
    tf.sphere2normal(fs3, fn3);
    tf.sphere2normal(ls3, ln3);
    tf.sphere2normal(fs4, fn4);
    tf.sphere2normal(ls4, ln4);

    epi.getRotMatEssMat2
    (forNormals, latNormals, fn1, ln1, fn2, ln2, fn3, ln3, fn4, ln4);

    
    cv::Mat mask1, mask2, mask3, mask4;
    cv::Mat m1 = epi.getRotMatEssMat(fn1, ln1, mask1);
    cv::Mat m2 = epi.getRotMatEssMat(fn2, ln2, mask2);
    cv::Mat m3 = epi.getRotMatEssMat(fn3, ln3, mask3);
    cv::Mat m4 = epi.getRotMatEssMat(fn4, ln4, mask4);
    
    cv::Vec3f v1 = Rotation::RotMat2RotVec(m1);
    cv::Vec3f v2 = Rotation::RotMat2RotVec(m2);
    cv::Vec3f v3 = Rotation::RotMat2RotVec(m3);
    cv::Vec3f v4 = Rotation::RotMat2RotVec(m4);
    
    std::cout  << m1 << std::endl;
    std::cout  << m2 << std::endl;
    std::cout  << m3 << std::endl;
    std::cout  << m4 << std::endl;
    
    std::cout << "angle = " << cv::norm(v1) << ", normal = " << v1/cv::norm(v1) << std::endl;
    std::cout << "angle = " << cv::norm(v2) << ", normal = " << v2/cv::norm(v2) << std::endl;
    std::cout << "angle = " << cv::norm(v3) << ", normal = " << v3/cv::norm(v3) << std::endl;
    std::cout << "angle = " << cv::norm(v4) << ", normal = " << v4/cv::norm(v4) << std::endl;
*/
     
//    float weight;
//    return getRotMatSpecDir(forSpheres, latSpheres, froChgMats[0], weight);
    
    int maxIdx;
    return getRotMatWeightMax(forSpheres, latSpheres, maxIdx);
}

cv::Mat FeatureMatchEstimator::getRotMatWeightMax
(const std::vector<Sphere> &forSpheres,
 const std::vector<Sphere> &latSpheres, int &maxIdx) const
{
    struct timeval essStart, essEnd;
    gettimeofday(&essStart, NULL);
    
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
    
    gettimeofday(&essEnd, NULL);
    double essTime = (double) (essEnd.tv_sec - essStart.tv_sec) +
                      (essEnd.tv_usec - essStart.tv_usec) * 1e-6;
    std::cout << "ess time = " << essTime << std::endl;
    
    return rotMats[maxIdx];
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
        cv::Mat rotMatChg = Rotation::chgRotMatCoo(rotMat, froChgMat.inv());

        std::cout << "--------------------------------------" << std::endl;
        std::cout << "weight = " << weight << std::endl;
        std::cout << "rotMat = " <<std::endl<< rotMat << std::endl;
        std::cout << "--------------------------------------" << std::endl;
    
        return rotMatChg;
    }
}

