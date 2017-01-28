//
//  JackInHeadEstimator.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2017/01/15.
//  Copyright © 2017年 masakazu. All rights reserved.
//

#include "JackInHeadEstimator.hpp"

cv::Mat JackInHeadEstimator::getRotMat
(const cv::Mat &forImg, const cv::Mat &latImg, const State &state)
{
    return getRotMat(forImg, latImg);
}

cv::Mat JackInHeadEstimator::getRotMat
(const cv::Mat &forImg, const cv::Mat &latImg)
{
    // リサイズ
    cv::Mat forResImg, latResImg;
    tf.resizeImg(forImg, forResImg);
    tf.resizeImg(latImg, latResImg);
    
    // グレースケールに変換
    cv::Mat forGrayImg, latGrayImg;
    cv::cvtColor(forResImg, forGrayImg, CV_BGR2GRAY);
    cv::cvtColor(latResImg, latGrayImg, CV_BGR2GRAY);
    
    // オプティカルフロー計算領域のマスク
    cv::Mat mask;
    getOptflowMask(mask);
    cv::namedWindow("mask");
    cv::imshow("mask", mask);

    // 追跡用の特徴点を抽出
    std::vector<cv::Point2f> forPoints;
    cv::goodFeaturesToTrack(forGrayImg, forPoints, 400, 0.01, 1, mask);

    // オプティカルフローの計算
    std::vector<cv::Point2f> latPoints;
    std::vector<uchar> statuss;
    std::vector<float> errors;
    cv::calcOpticalFlowPyrLK
    (forGrayImg, latGrayImg, forPoints, latPoints, statuss, errors);
    
    // エラーを除去
    for (int i = 0; i < forPoints.size(); ) {
        if (statuss[i] == 0) {
            forPoints.erase(forPoints.begin() + i);
            latPoints.erase(latPoints.begin() + i);
            statuss.erase(statuss.begin() + i);
        } else {
            i++;
        }
    }
    
    // Equirect型に変換
    std::vector<Equirect> forEquis(forPoints.size()), latEquis(latPoints.size());
    for (int i = 0 ; i < forPoints.size(); i++) {
        forEquis[i] = Equirect(forPoints[i]);
        latEquis[i] = Equirect(latPoints[i]);
    }
    
    cv::Mat drawImg;
    drawOptflow(forImg, forEquis, latEquis, drawImg);
    cv::namedWindow("flow");
    cv::imshow("flow", drawImg);
    
    std::vector<Sphere> forSphere(forEquis.size()), latSphere(latEquis.size());
    for (int i = 0; i < forEquis.size(); i++) {
        forSphere[i] = tf.equirect2sphere(forEquis[i]);
        latSphere[i] = tf.equirect2sphere(latEquis[i]);
    }
    
    return getRotMatAffine(forSphere, latSphere);
}

cv::Mat JackInHeadEstimator::getRotMatAffine
(const std::vector<Sphere> &forSpheres, const std::vector<Sphere> &latSpheres)
{
    std::vector<cv::Point3f> forPoints(forSpheres.size());
    std::vector<cv::Point3f> latPoints(latSpheres.size());
    for (int i = 0; i < forSpheres.size(); i++) {
        forPoints[i] = forSpheres[i];
        latPoints[i] = latSpheres[i];
    }
    
    // ３次元アフィン変換の推定
    cv::Mat affine3d;
    std::vector<uchar> inliners;
    double ransacThreshold = 0.01;
    cv::estimateAffine3D
    (latPoints, forPoints, affine3d, inliners, ransacThreshold);
    // 平行移動の成分を除去
    cv::Mat affine3x3 = affine3d(cv::Rect(0, 0, 3, 3)).clone();
    
    // 特異値分解で回転成分の抽出
    cv::Mat u, w, vt;
    cv::SVD::compute(affine3x3, w, u, vt);
    cv::Mat rotMatd = u * vt;
    cv::Mat rotMat;
    rotMatd.convertTo(rotMat, CV_32FC1);
    
//    std::cout << "affine = " << std::endl << affine3d << std::endl;
//    std::cout << "rot = " << std::endl << u * vt << std::endl;
    
    return rotMat;
    
    /*
    // テストデータ
    std::vector<Sphere> forSphereTest, latSphereTest;
    std::copy
    (forSpheres.begin(), forSpheres.end(), std::back_inserter(forSphereTest));
    std::copy
    (latSpheres.begin(), latSpheres.end(), std::back_inserter(latSphereTest));
    
    // 訓練データ
    std::vector<Sphere> forSphereTrain, latSphereTrain;
    
    // 全データを訓練データとテストデータに分割
    extTrainFromAll
    (forSphereTest, latSphereTest, forSphereTrain, latSphereTrain);
    
    
    return cv::Mat::eye(3, 3, 3);
     */
}


void JackInHeadEstimator::extTrainFromAll
(std::vector<Sphere> &forAll, std::vector<Sphere> &latAll,
 std::vector<Sphere> &forTrain, std::vector<Sphere> &latTrain)
{
    int trainNum = 3;
    for (int i = 0; i < trainNum; i++) {
        // 残りの集合からランダムなインデックスを選ぶ
        int tmpIndex = rand() % forAll.size();
        
        // 訓練要素を追加
        forTrain.push_back(forAll[i]);
        latTrain.push_back(latAll[i]);
        
        // 訓練要素を削除
        forAll.erase(forAll.begin() + tmpIndex);
        latAll.erase(latAll.begin() + tmpIndex);
    }
}

void JackInHeadEstimator::getOptflowMask(cv::Mat &mask)
{
    // 有効な緯度の範囲
    const float validRad = M_PI_2;
    
    // 有効な画像の高さ
    const float validHeight = tf.dphi2v(validRad);
    
    // マスクをゼロで初期化
    mask = cv::Mat::zeros(tf.fs, CV_8UC1);
    
    for (int v = (tf.fs.height - validHeight) / 2;
         v < (tf.fs.height + validHeight) / 2; v++) {
        for (int u = 0; u < tf.fs.width; u++) {
            mask.at<uchar>(v, u) = 255;
        }
    }
}

void JackInHeadEstimator::drawOptflow
(const cv::Mat &img, const std::vector<Equirect> forEquis,
 const std::vector<Equirect> latEquis, cv::Mat &drawImg)
{
    drawImg = img.clone();
    
    for (int i = 0; i < forEquis.size(); i++) {
        cv::line(drawImg, forEquis[i], latEquis[i],
                 cv::Scalar(rand()%256, rand()%256, rand()%256), 2, CV_AA);
        cv::circle(drawImg, latEquis[i], 4, CV_AA, -1);

    }
}



