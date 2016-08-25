//
//  Rotation.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/07/04.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Rotation.hpp"

Rotation::Rotation(const Transform& transform, float fieldAngle, int matchThre):
transform(transform), fieldRadius(tanf(fieldAngle)), matchThre(matchThre)
{
    // RANSACの乱数用
    srand((unsigned int) time(NULL));
    
    // カメラの前後の特徴点を同時に使う
    // 座標軸の向きに変換
    rotMats.push_back((cv::Mat_<float>(3,3) << 1,0,0,0,1,0,0,0,1));
//rotMats.push_back((cv::Mat_<float>(3,3) << -1,0,0,0,1,0,0,0,-1));
    rotMats.push_back((cv::Mat_<float>(3,3) << 0,0,-1,0,1,0,1,0,0));
//rotMats.push_back((cv::Mat_<float>(3,3) << 0,0,1,0,1,0,-1,0,0));
    rotMats.push_back((cv::Mat_<float>(3,3) << 1,0,0,0,0,-1,0,1,0));
//rotMats.push_back((cv::Mat_<float>(3,3) << 1,0,0,0,0,1,0,-1,0));
    
    /*
    // 斜め方向に変換
    float sqrt6 = sqrtf(6.0); float sqrt3 = sqrtf(3.0);
    rotMats.push_back((cv::Mat_<float>(3,3) << 2.0/sqrt6, -1.0/sqrt6, -1.0/sqrt6,
                                               0.0, 1.0/sqrt6, -1.0/sqrt6,
                                               1.0/sqrt3, 1.0/sqrt3, 1.0/sqrt3));
    rotMats.push_back((cv::Mat_<float>(3,3) << -2.0/sqrt6, 1.0/sqrt6, -1.0/sqrt6,
                                               0.0, 1.0/sqrt6, 1.0/sqrt6,
                                               1.0/sqrt3, 1.0/sqrt3, -1.0/sqrt3));
*/
}

Rotation::~Rotation(){}

void Rotation::estimate3DRotMatSVD
(const std::vector<cv::Point3f> &forspheres,
 const std::vector<cv::Point3f> &latspheres, cv::Mat& estRotMat) const
{
    int sampleSize = 6;
    cv::Mat betRotMat = cv::Mat(3, 3, CV_32FC1);
    size_t betInlierSize = 0;
    std::vector<cv::Point3f> betInForPoints, betInLatPoints;
    
//    float betError = FLT_MAX;
    
    // ランダムにサンプリングした点から推定を行う
    for (int i=0; i<sampleSize; i++) {
        std::vector<cv::Point3f> forPointx, latPointx;
        getRondomPoint(forspheres, latspheres, forPointx, latPointx, sampleSize);
        
        cv::Mat curEstRotMat;
        estimate3DRotMatSVDPartial(forPointx, latPointx, curEstRotMat);
        
        float error;
        error = evalEstRotMat(forspheres, latspheres, curEstRotMat);
//        std::cout << "current error = " << curError << std::endl;
//        std::cout << "current mat = " << std::endl <<  curEstRotMat << std::endl;
        
        std::vector<cv::Point3f> inForPoints, inLatPoints;
        size_t inLierSize =  removeOutlier(forspheres, latspheres, inForPoints, inLatPoints, error/forspheres.size(), curEstRotMat);
        
        if (inLierSize > betInlierSize) {
            betInlierSize = inLierSize;
            curEstRotMat.copyTo(betRotMat);
            betInForPoints.clear();
            std::copy
            (inForPoints.begin(), inForPoints.end(),
             std::back_inserter(betInForPoints));
            betInLatPoints.clear();
            std::copy
            (inLatPoints.begin(), inLatPoints.end(),
             std::back_inserter(betInLatPoints));
        }
        
        //std::cout <<  i << "-th sample (error, inlier) = ("
        //<< error << ", " << inLierSize << ")" << std::endl;
        //std::cout << "mat = " << std::endl << curEstRotMat << std::endl;
        
        /*
        if (curError < betError) {
            betError = curError;
            curEstRotMat.copyTo(betRotMat);
        }
         */
    }
    /*
    // 最も高いスコアの行列を用いてインライアーを求める
    std::vector<cv::Point3f> inForPoints, inLatPoints;
    removeOutlier
    (forPoints, latPoints, inForPoints, inLatPoints,
     betError/forPoints.size(), betRotMat);
    */
    
    //std::cout << "(all, inlier) = (" << forPoints.size() << ", "
    //          << betInlierSize << ")" << std::endl;
    
    // インライアーから回転行列を推定する
    estRotMat = cv::Mat(3, 3, CV_32FC1);
    estimate3DRotMatSVDPartial(betInForPoints, betInLatPoints, estRotMat);
}

void Rotation::getRondomPoint
(const std::vector<cv::Point3f> &forPoints,
 const std::vector<cv::Point3f> &latPoints,
 std::vector<cv::Point3f> &forPointx, std::vector<cv::Point3f> &latPointx,
 size_t getSize)
{
    forPointx.clear();
    latPointx.clear();
    
    for (int i=0; i<getSize; i++) {
        int randIdx = rand() % forPoints.size();
        
        forPointx.push_back(forPoints[randIdx]);
        latPointx.push_back(latPoints[randIdx]);
    }
}

void Rotation::estimate3DRotMatSVDPartial
(const std::vector<cv::Point3f> &forPointx,
 const std::vector<cv::Point3f> &latPointx, cv::Mat &estRotMatPart)
{
    cv::Mat corMat = cv::Mat::zeros(3, 3, CV_32FC1);
    for (int i=0; i<forPointx.size(); i++) {
        cv::Vec3f forVec = cv::Mat1f(forPointx[i]);
        cv::Vec3f latVec = cv::Mat1f(latPointx[i]);
        
        corMat = corMat + cv::Mat(forVec * latVec.t());
    }
    
    cv::Mat u, w, vt;
    cv::SVD::compute(corMat, w, u, vt);
    cv::Mat est = u * vt;
    
    estRotMatPart = cv::Mat(3, 3, CV_32FC1);
    est.copyTo(estRotMatPart);
}

float Rotation::evalEstRotMat
(const std::vector<cv::Point3f> &forPoints,
 const std::vector<cv::Point3f> &latPoints, const cv::Mat &estRotMat) const
{
    float accErr = 0.0;
    for (int i=0; i<forPoints.size(); i++) {
        cv::Point3f modLatPoint;
        transform.sphereWithRotMat(latPoints[i], modLatPoint, estRotMat);
        
        accErr += cv::norm(forPoints[i] - modLatPoint);
    }
    
    return accErr;
}

size_t Rotation::removeOutlier
(const std::vector<cv::Point3f> &forPoints,
 const std::vector<cv::Point3f> &latPoints,
 std::vector<cv::Point3f> &inForPoints, std::vector<cv::Point3f> &inLatPoints,
 float normThre, const cv::Mat &estRotMat) const
{
    inForPoints.clear();
    inLatPoints.clear();
    
    std::cout << "thre = " << normThre << std::endl;
    
    for (int i=0; i<forPoints.size(); i++) {
        cv::Point3f modLatPoint;
        transform.sphereWithRotMat(latPoints[i], modLatPoint, estRotMat);
        
        // 閾値以下ならインライアーに加える
        if (cv::norm(forPoints[i] - modLatPoint) < normThre) {
            inForPoints.push_back(forPoints[i]);
            inLatPoints.push_back(latPoints[i]);
        }
    }
    
    return inForPoints.size();
}

void Rotation::normalRotMat(cv::Mat &rotMat)
{
    Quarternion quart;
    Quarternion::RotMat2Quart(rotMat, quart);
    Quarternion::normalQuart(quart);
    Quarternion::Quart2RotMat(quart, rotMat);
}

void Rotation::estimateRotMat
(const std::vector<cv::Point3f> &forspheres,
 const std::vector<cv::Point3f> &latspheres, cv::Mat &estRotMat) const
{
    std::vector<float> angles(rotMats.size());
    std::vector<cv::Vec3f> axiss(rotMats.size());
    std::vector<int> inNums(rotMats.size());
    for (int i=0; i<rotMats.size(); i++) {
        // カメラの正面を変更
        std::vector<cv::Point3f> forspheresRot, latspheresRot;
        transform.rotateSphere(forspheres, forspheresRot, rotMats[i]);
        transform.rotateSphere(latspheres, latspheresRot, rotMats[i]);

        // カメラの正面の特徴点を取り出す
        std::vector<cv::Point3f> forspheresFront, latspheresFront;
        extractFrontFeature
        (forspheresRot, latspheresRot, forspheresFront, latspheresFront);

        // 正規化画像座標に変換
        std::vector<cv::Point2f> fornormalsFront, latnormalsFront;
        transform.sphere2normal(forspheresFront, fornormalsFront);
        transform.sphere2normal(latspheresFront, latnormalsFront);
        
        /*
        for (int j=0; j<forspheresRot.size(); j++) {
            if (isInFront(forspheresRot[j], latspheresRot[j])) {
                cv::Point2f fornormal;
                transform.sphere2normal(forspheresRot[j], fornormal);
                fornormals.push_back(fornormal);
                cv::Point2f latnormal;
                transform.sphere2normal(latspheresRot[j], latnormal);
                latnormals.push_back(latnormal);
                std::cout << j << " ";
            }
        }*/

        
        inNums[i] = (fornormalsFront.size() > matchThre)?
                    (int)fornormalsFront.size(): 0;
        
        // カメラ正面の特徴点で回転行列推定
        cv::Mat estRotMatFront;
        estimate3DRotMatEssential(fornormalsFront, latnormalsFront, estRotMatFront);
        
        // 回転角と回転軸に分解し，回転軸を元のカメラ座標に
        cv::Vec3f axisRot;
        Quarternion::RotMat2Rodrigues(estRotMatFront, angles[i], axisRot);
        axiss[i] = cv::Vec3f(cv::Mat1f(rotMats[i].inv() *
                                       cv::Mat1f(axisRot)));
        
        std::cout << "match num = " << fornormalsFront.size() << std::endl;
        std::cout << "camera R = " << std::endl << rotMats[i] << std::endl;
        std::cout << "R = "  << std::endl << estRotMatFront << std::endl;
        std::cout << "angle = " << angles[i] << std::endl;
        std::cout << "(rotated axis = " << axisRot << ")" << std::endl;
        std::cout << "axis = " << axiss[i] << std::endl;
        std::cout << "--------------------------------------" << std::endl;
        
    }
    
    float estAngle;
    cv::Vec3f estAxis;
    integrateAngleAxis(angles, axiss, inNums, estAngle, estAxis);
    std::cout << "estAngle = " << estAngle << std::endl;
    std::cout << "estAxis = " << estAxis << std::endl;
    Quarternion::Rodrigues2RotMat(estAngle, estAxis, estRotMat);
    std::cout << "estRotMat = " << std::endl << estRotMat << std::endl;
}

void Rotation::estimate3DRotMatEssential
(const std::vector<cv::Point2f> &fornormals,
 const std::vector<cv::Point2f> &latnormals, cv::Mat &estRotMat) const
{
    // 特徴点の数が不十分
    if (fornormals.size() < matchThre) {
        estRotMat = (cv::Mat_<float>(3, 3) << 1,0,0,0,1,0,0,0,1);
        return;
    }
    
    double focal = 1.0;
    cv::Point2d pp(0.0, 0.0);
    //int method = cv::RANSAC;
    int method = cv::LMEDS;
    //double prob = 0.999;
    //double threshold = 0.01;
    //cv::Mat mask;
    
    // 後フレーム->前フレームの基本行列
    cv::Mat E = cv::findEssentialMat(latnormals, fornormals, focal, pp, method);
    //cv::Mat E = cv::findEssentialMat(fornormals, latnormals, focal, pp, method, prob, threshold, mask);
    //std::cout << "mask = " << std::endl << mask << std::endl;
    //std::cout << (mask.type() == CV_8UC1) << std::endl;
    
    // 後フレーム->前フレームの回転行列
    cv::Mat R, t;    
    cv::recoverPose(E, latnormals, fornormals, R, t);
    
    estRotMat = cv::Mat(3, 3, CV_32FC1);
    for (int u=0; u<3; u++) {
        for (int v=0; v<3; v++) {
            // 行列の要素の型変換
            estRotMat.at<float>(v, u) = R.at<double>(v, u);
        }
    }
}

void Rotation::extractFrontFeature
(const std::vector<cv::Point3f> &forspheres,
 const std::vector<cv::Point3f> &latspheres,
 std::vector<cv::Point3f> &forspheresFront,
 std::vector<cv::Point3f> &latspheresFront) const
{
    std::cout << "front = ";
    for (int i=0; i<forspheres.size(); i++) {
        if (isInFront(forspheres[i], latspheres[i])) {
            forspheresFront.push_back(forspheres[i]);
            latspheresFront.push_back(latspheres[i]);
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
}

void Rotation::integrateAngleAxis
(std::vector<float> &angles, std::vector<cv::Vec3f> &axiss,
 std::vector<int> &inNums, float &angle, cv::Vec3f &axis) const
{
    // 推定に使用された特徴点の数で重みをつける
    int inSum = 0;
    for (auto inNum: inNums) {
        inSum += inNum;
    }
    
    // 回転角を決定
    for (int i=0; i<angles.size(); i++) {
        angle+= ((float) inNums[i] / inSum) * angles[i];
    }
    
    // 回転軸を決定
    for (int i=0; i<axiss.size(); i++) {
        axis += ((float) inNums[i] / inSum) * axiss[i];
    }
}