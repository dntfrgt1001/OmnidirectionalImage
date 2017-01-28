//
//  Epipolar.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Epipolar.hpp"

cv::Mat Epipolar::getRotMatEssMat
(const std::vector<Normal> &forNormals,
 const std::vector<Normal> &latNormals, cv::Mat &mask) const
{
    // 閾値以下の数の特徴点
    if (forNormals.size() < numThre) {
        std::cout << "there are few matched points" << std::endl;
        return cv::Mat::zeros(3, 3, CV_32FC1);
    }
    
    const double focal = 1.0;
    const cv::Point2d pp(0.0, 0.0);
    const int method = cv::RANSAC;
    //int method = cv::LMEDS;
    double prob = 0.999;
    double threshold = 0.005;
    
    // 後フレーム->前フレームの基本行列
    //cv::Mat E = cv::findEssentialMat(latnormals, fornormals, focal, pp, method);
    
    std::vector<cv::Point2f> forPoints, latPoints;
    Transform::normal2point(forNormals, forPoints);
    Transform::normal2point(latNormals, latPoints);
    
    cv::Mat E =
    cv::findEssentialMat
    (latPoints, forPoints, focal, pp, method, prob, threshold, mask);
    
    
    // 後フレーム->前フレームの回転行列
    cv::Mat R, t;
    cv::recoverPose(E, latPoints, forPoints, R, t);
    
    cv::Mat rotMat;
    R.convertTo(rotMat, CV_32FC1);
    
    return rotMat;
}

cv::Mat Epipolar::getRotMatEssMat2
(const std::vector<Normal> &forNormals, const std::vector<Normal> &latNormals,
 const std::vector<Normal> &fn1, const std::vector<Normal> &ln1,
 const std::vector<Normal> &fn2, const std::vector<Normal> &ln2,
 const std::vector<Normal> &fn3, const std::vector<Normal> &ln3,
 const std::vector<Normal> &fn4, const std::vector<Normal> &ln4) const
{
    const double focal = 1.0;
    const cv::Point2d pp(0.0, 0.0);
    const int method = cv::RANSAC;
    //int method = cv::LMEDS;
    double prob = 0.999;
    double threshold = 0.005;
    
    // 後フレーム->前フレームの基本行列
    //cv::Mat E = cv::findEssentialMat(latnormals, fornormals, focal, pp, method);
    
    std::vector<cv::Point2f> forPoints, latPoints;
    Transform::normal2point(forNormals, forPoints);
    Transform::normal2point(latNormals, latPoints);
    
    cv::Mat E =
    cv::findEssentialMat
    (latPoints, forPoints, focal, pp, method, prob, threshold);

    cv::Mat Rp, Rn, t;
    cv::decomposeEssentialMat(E, Rp, Rn, t);
    cv::Mat Rpf, Rnf;
    Rp.convertTo(Rpf, CV_32FC1);
    Rn.convertTo(Rnf, CV_32FC1);
    
    cv::Vec3f rv1 = Rotation::RotMat2RotVec(Rpf);
    cv::Vec3f rv2 = Rotation::RotMat2RotVec(Rnf);
    
    std::cout << cv::norm(rv1) << ", " << rv1 / cv::norm(rv1) << std::endl;
    std::cout << cv::norm(rv2) << ", " << rv2 / cv::norm(rv2) << std::endl;
    
    
    std::cout << t << std::endl;

    // 後フレーム->前フレームの回転行列
    cv::Mat R1, t1;
    std::vector<cv::Point2f> forPoint1, latPoint1;
    Transform::normal2point(fn1, forPoint1);
    Transform::normal2point(ln1, latPoint1);
    cv::recoverPose(E, latPoint1, forPoint1, R1, t1);

    cv::Mat R2, t2;
    std::vector<cv::Point2f> forPoint2, latPoint2;
    Transform::normal2point(fn2, forPoint2);
    Transform::normal2point(ln2, latPoint2);
    cv::recoverPose(E, latPoint2, forPoint2, R2, t2);
    
    cv::Mat R3, t3;
    std::vector<cv::Point2f> forPoint3, latPoint3;
    Transform::normal2point(fn3, forPoint3);
    Transform::normal2point(ln3, latPoint3);
    cv::recoverPose(E, latPoint3, forPoint3, R3, t3);
    
    cv::Mat R4, t4;
    std::vector<cv::Point2f> forPoint4, latPoint4;
    Transform::normal2point(fn4, forPoint4);
    Transform::normal2point(ln4, latPoint4);
    cv::recoverPose(E, latPoint4, forPoint4, R4, t4);
    
    cv::Mat R1f, R2f, R3f, R4f;
    
    R1.convertTo(R1f, CV_32FC1);
    R2.convertTo(R2f, CV_32FC1);
    R3.convertTo(R3f, CV_32FC1);
    R4.convertTo(R4f, CV_32FC1);
    
    std::cout << "R1f = " << std::endl << R1f << std::endl;
    std::cout << "R2f = " << std::endl << R1f << std::endl;
    std::cout << "R3f = " << std::endl << R3f << std::endl;
    std::cout << "R4f = " << std::endl << R4f << std::endl;
    
    cv::Mat E1 = cv::findEssentialMat(latPoint1, forPoint1, focal, pp, method, prob, threshold);
    cv::Mat E2 = cv::findEssentialMat(latPoint2, forPoint2, focal, pp, method, prob, threshold);
    cv::Mat E3 = cv::findEssentialMat(latPoint3, forPoint3, focal, pp, method, prob, threshold);
    cv::Mat E4 = cv::findEssentialMat(latPoint4, forPoint4, focal, pp, method, prob, threshold);
    
    std::vector<cv::Point2f> forPoint1h, latPoint1h;
    for (int i = 0; i < forPoint1.size() / 2; i++) {
        forPoint1h.push_back(forPoint1[i]);
        latPoint1h.push_back(latPoint1[i]);
    }
    cv::Mat E1h = cv::findEssentialMat(latPoint1h, forPoint1h, focal, pp, method, prob, threshold);
    
    std::cout << "E1h = "  << std::endl << E1h << std::endl;
    std::cout << "E1 = "  << std::endl << E1 << std::endl;
    std::cout << "E2 = "  << std::endl << E2 << std::endl;
    std::cout << "E3 = "  << std::endl << E3 << std::endl;
    std::cout << "E4 = "  << std::endl << E4 << std::endl;


    cv::Mat R11, R12, R21, R22, R31, R32, R41, R42, R11h, R12h;
    cv::Mat t11, t22, t33, t44, t11h;
    cv::decomposeEssentialMat(E1, R11, R12, t11);
    cv::decomposeEssentialMat(E1, R11, R12, t11);
    cv::decomposeEssentialMat(E2, R21, R22, t22);
    cv::decomposeEssentialMat(E3, R31, R32, t33);
    cv::decomposeEssentialMat(E4, R41, R42, t44);
    cv::decomposeEssentialMat(E1h, R11h, R12h, t11h);
    
    
    cv::Mat R11f, R12f;
    R11.convertTo(R11f, CV_32FC1);
    R12.convertTo(R12f, CV_32FC1);
    cv::Vec3f vec11 = Rotation::RotMat2RotVec(R11f);
    cv::Vec3f vec12 = Rotation::RotMat2RotVec(R12f);
    std::cout << cv::norm(vec11) << ", " << vec11/cv::norm(vec11) << std::endl;
    std::cout << cv::norm(vec12) << ", " << vec12/cv::norm(vec12) << std::endl;
    
    
    std::cout << std::endl << std::endl;
    std::cout << "R11h = " << std::endl << R11h << std::endl;
    std::cout << "R12h = " << std::endl << R12h << std::endl;
    std::cout << "R11 = " << std::endl << R11 << std::endl;
    std::cout << "R12 = " << std::endl << R12 << std::endl;
    
    std::cout << "R21 = " << std::endl << R21 << std::endl;
    std::cout << "R22 = " << std::endl << R22 << std::endl;
    
    std::cout << "R31 = " << std::endl << R31 << std::endl;
    std::cout << "R32 = " << std::endl << R32 << std::endl;

    std::cout << "R41 = " << std::endl << R41 << std::endl;
    std::cout << "R42 = " << std::endl << R42 << std::endl;
    std::cout << "t11 = " << std::endl << t11 << std::endl;
    std::cout << "t22 = " << std::endl << t22 << std::endl;
    std::cout << "t33 = " << std::endl << t33 << std::endl;
    std::cout << "t44 = " << std::endl << t44 << std::endl;
    
    
    cv::Mat R1rec, R2rec, R3rec, R4rec, t1rec, t2rec, t3rec, t4rec;
    cv::recoverPose(E1, latPoint1, forPoint1, R1rec, t1rec);
    cv::recoverPose(E2, latPoint2, forPoint2, R2rec, t2rec);
    cv::recoverPose(E3, latPoint3, forPoint3, R3rec, t3rec);
    cv::recoverPose(E4, latPoint4, forPoint4, R4rec, t4rec);
    
    std::cout << "R1rec = " << std::endl << R1rec << std::endl;
    std::cout << "R2rec = " << std::endl << R2rec << std::endl;
    std::cout << "R3rec = " << std::endl << R3rec << std::endl;
    std::cout << "R4rec = " << std::endl << R4rec << std::endl;
    
    
    float angle = M_PI / 4.0;
    cv::Vec3f stdvec(1.0, 2.0, 3.0);
    cv::Vec3f rotvec = stdvec / cv::norm(stdvec) * (angle + M_PI);
    std::cout << rotvec/cv::norm(rotvec) << std::endl;
    cv::Vec3f rotvec2 = Rotation::RotMat2RotVec(Rotation::RotVec2RotMat(rotvec));
    std::cout << rotvec2 / cv::norm(rotvec2) << std::endl;
    
//    std::cout << t1 << std::endl;
//    std::cout << t2 << std::endl;
//    std::cout << t3 << std::endl;
//    std::cout << t4 << std::endl;
    
    
    
    cv::Vec3f v1 = Rotation::RotMat2RotVec(R1f);
    cv::Vec3f v2 = Rotation::RotMat2RotVec(R2f);
    cv::Vec3f v3 = Rotation::RotMat2RotVec(R3f);
    cv::Vec3f v4 = Rotation::RotMat2RotVec(R4f);
    
    std::cout << "angle = " << cv::norm(v1) << ", normal = " << v1/cv::norm(v1) << std::endl;
    std::cout << "angle = " << cv::norm(v2) << ", normal = " << v2/cv::norm(v2) << std::endl;
    std::cout << "angle = " << cv::norm(v3) << ", normal = " << v3/cv::norm(v3) << std::endl;
    std::cout << "angle = " << cv::norm(v4) << ", normal = " << v4/cv::norm(v4) << std::endl;
    
//    std::cout << acos( v1.dot(v3) / (cv::norm(v1)*cv::norm(v3))) << std::endl;

    
    
    cv::Mat rotMat;
    R1.convertTo(rotMat, CV_32FC1);
    
    return rotMat;
}


float Epipolar::getWeight(const cv::Mat &mask) const
{
    int count = 0;
    for (int v = 0; v < mask.rows; v++) {
        if (mask.at<uchar>(v, 0) != 0) count++;
    }
    
    std::cout << "(all, inlier) = (" << mask.rows
              << ", " << count << ")" << std::endl;
    
    return (float) count;
}

int Epipolar::getMaxWeightIndex(const std::vector<float> &weights) const
{
    std::cout << "weights = ";
    for (int i = 0; i < weights.size(); i++) {
        std::cout << weights[i] << " ";
    }
    std::cout << std::endl;
    
    std::vector<float>::const_iterator itr =
    std::max_element(weights.begin(), weights.end());
    
    return (int) std::distance(weights.begin(), itr);
}
