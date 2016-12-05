//
//  Rotation.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/07/04.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Rotation.hpp"

cv::Mat Rotation::Quat2RotMat
(const Quaternion &quat)
{
    return cv::Mat_<float>(3, 3) <<
                1 - 2*(quat.y*quat.y + quat.z*quat.z),
                2*(quat.x*quat.y + quat.w*quat.z),
                2*(quat.x*quat.z - quat.w*quat.y),
                2*(quat.x*quat.y - quat.w*quat.z),
                1 - 2*(quat.x*quat.x + quat.z*quat.z),
                2*(quat.y*quat.z + quat.w*quat.x),
                2*(quat.x*quat.z + quat.w*quat.y),
                2*(quat.y*quat.z - quat.w*quat.x),
                1 - 2*(quat.x*quat.x + quat.y*quat.y);
    
    /*
    rotMat.at<float>(0, 0) = 1 - 2*(quat.y*quat.y + quat.z*quat.z);
    rotMat.at<float>(1, 0) = 2*(quat.x*quat.y + quat.t*quat.z);
    rotMat.at<float>(2, 0) = 2*(quat.x*quat.z - quat.t*quat.y);
    rotMat.at<float>(0, 1) = 2*(quat.x*quat.y - quat.t*quat.z);
    rotMat.at<float>(1, 1) = 1 - 2*(quat.x*quat.x + quat.z*quat.z);
    rotMat.at<float>(2, 1) = 2*(quat.y*quat.z + quat.t*quat.x);
    rotMat.at<float>(0, 2) = 2*(quat.x*quat.z + quat.t*quat.y);
    rotMat.at<float>(1, 2) = 2*(quat.y*quat.z - quat.t*quat.x);
    rotMat.at<float>(2, 2) = 1 - 2*(quat.x*quat.x + quat.y*quat.y);
     */
}

Quaternion Rotation::RotMat2Quat(const cv::Mat &rotMat)
{
    /*
    float trace = (cv::trace(rotMat))(0);
    float t = sqrtf(trace + 1) / 2.0;
    return Quaternion(t,
                      (rotMat.at<float>(2,1)-rotMat.at<float>(1,2))/(4*t),
                      (rotMat.at<float>(0,2)-rotMat.at<float>(2,0))/(4*t),
                      (rotMat.at<float>(1,0)-rotMat.at<float>(0,1))/(4*t));
    */
    
    // cv::traceの返り値はScalar型
    //float trace = (cv::trace(rotMat))(0);
    
    std::vector<float> diag{rotMat.at<float>(0,0),
                            rotMat.at<float>(1,1),
                            rotMat.at<float>(2,2),
                            1.0};

    // 四元数の最大要素のインデックスを求める
    int maxIndex = (int) std::distance(diag.begin(),
                                       std::max_element(diag.begin(),
                                                        diag.end()));
    
    // 最大要素に従いクォータニオンの要素を求める
    float x, y, z, w;
    switch (maxIndex) {
        case 0: // xが最大
            x = sqrtf(+ diag[0] - diag[1] - diag[2] + 1.0) / 2.0;
            y = (rotMat.at<float>(0,1) - rotMat.at<float>(1,0)) / (4 * x);
            z = (rotMat.at<float>(2,0) + rotMat.at<float>(0,2)) / (4 * x);
            w = (rotMat.at<float>(1,2) - rotMat.at<float>(2,1)) / (4 * x);
            break;
        case 1: // yが最大
            y = sqrtf(- diag[0] + diag[1] - diag[2] + 1.0) / 2.0;
            x = (rotMat.at<float>(0,1) + rotMat.at<float>(1,0)) / (4 * y);
            z = (rotMat.at<float>(1,2) + rotMat.at<float>(2,1)) / (4 * y);
            w = (rotMat.at<float>(2,0) - rotMat.at<float>(0,2)) / (4 * y);
            break;
        case 2: // zが最大
            z = sqrtf(- diag[0] - diag[1] + diag[2] + 1.0) / 2.0;
            x = (rotMat.at<float>(2,0) + rotMat.at<float>(0,2)) / (4 * z);
            y = (rotMat.at<float>(1,2) + rotMat.at<float>(2,1)) / (4 * z);
            w = (rotMat.at<float>(0,1) - rotMat.at<float>(1,0)) / (4 * z);
            break;
        case 3: // wが最大
            w = sqrtf(+ diag[0] + diag[1] + diag[2] + 1.0) / 2.0;
            x = (rotMat.at<float>(1,2) - rotMat.at<float>(2,1)) / (4 * w);
            y = (rotMat.at<float>(2,0) - rotMat.at<float>(0,2)) / (4 * w);
            z = (rotMat.at<float>(0,1) - rotMat.at<float>(1,0)) / (4 * w);
        default:
            break;
    }
    
    return Quaternion(w, x, y, z);
}

cv::Mat Rotation::RotVec2RotMat(const cv::Vec3f& rotVec)
{
    // 回転しないとき
    const float angle = cv::norm(rotVec);
    if (angle == 0.0) return cv::Mat::eye(3, 3, CV_32FC1);
    
    cv::Mat infRot = getInfRot(rotVec);
    
    return cv::Mat::eye(3, 3, CV_32FC1) +
           sinf(angle)/angle * infRot +
           (1 - cosf(angle))/(angle*angle) * infRot*infRot;
    
    /*
    cv::Mat rotMat = cv::Mat(3, 3, CV_32FC1);
    cv::Rodrigues(rotVec, rotMat);
    return rotMat;
     */
}

cv::Vec3f Rotation::RotMat2RotVec(const cv::Mat &rotMat)
{
    const float angle = acosf(((cv::trace(rotMat))[0] - 1) /2);

    // 無回転ならゼロベクトル
    if (angle == 0.0) return cv::Vec3f(0, 0, 0);
    
    cv::Mat omega = (rotMat - rotMat.t()) / (2*sinf(angle));
    
    return cv::Vec3f(omega.at<float>(2, 1) * angle,
                     omega.at<float>(0, 2) * angle,
                     omega.at<float>(1, 0) * angle);

    /*
    cv::Vec3f rotVec;
    cv::Rodrigues(rotMat, rotVec);
    return rotVec;
    */
}

cv::Mat Rotation::getFroChgMat(const cv::Mat &rotMat)
{
    // 単位回転ベクトルの計算
    cv::Vec3f rotVec = RotMat2RotVec(rotMat);
    
    // 回転ベクトルのノルム
    float normRotVec = cv::norm(rotVec);
    
    // 無回転なら単位行列
    if (normRotVec == 0.0) return cv::Mat::eye(3, 3, CV_32FC1);
    
    cv::Vec3f rotVecUni = rotVec/cv::norm(rotVec);
    
    // 回転角
    float angle = acosf(rotVecUni[2]);
    // 回転軸
    float normAxis = sqrtf(rotVecUni[0]*rotVecUni[0] +
                           rotVecUni[1]*rotVecUni[1]);
    cv::Vec3f axis = cv::Vec3f(rotVecUni[1], -rotVecUni[0], 0) / normAxis;
    
    return RotVec2RotMat(angle * axis);
}

cv::Mat Rotation::chgRotMatCoo
(const cv::Mat &rotMat, const cv::Mat &froChgMat)
{
    // 回転ベクトルに変換
    cv::Vec3f rotVec = RotMat2RotVec(rotMat);
    
    // 回転ベクトルを回転させ行列に変換
    return RotVec2RotMat(cv::Vec3f(cv::Mat1f(froChgMat*cv::Mat1f(rotVec))));
}

cv::Mat Rotation::getInfRot(const cv::Vec3f &angVel)
{
    return (cv::Mat_<float>(3,3) << 0, -angVel[2], angVel[1],
                                    angVel[2], 0, -angVel[0],
                                    -angVel[1], angVel[0], 0);
}

cv::Vec3f Rotation::getAngVel(const cv::Mat &infRot)
{
    return cv::Vec3f(infRot.at<float>(2, 1),
                     infRot.at<float>(0, 2),
                     infRot.at<float>(1, 0));
}

void Rotation::normalRotMat(cv::Mat &rotMat)
{
    Quaternion quat = RotMat2Quat(rotMat);
    Quaternion::normalQuart(quat);
    rotMat = Quat2RotMat(quat);
}
