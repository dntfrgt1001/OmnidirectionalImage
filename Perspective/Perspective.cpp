//
//  Perspective.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/07/13.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Perspective.hpp"


Perspective::Perspective
(const int persRad, const float rangeAngle):
normalRad(tanf(rangeAngle)),
inParaMat(getInParaMat(persRad, tanf(rangeAngle))),
pfs(cv::Size(persRad*2, persRad*2))
{
    std::cout << "InParaMat = " << std::endl << inParaMat << std::endl;
}

cv::Mat Perspective::getInParaMat
(const int persRad, const float ratio) const
{
    return (cv::Mat_<float>(3, 3) << persRad/ratio, 0, persRad,
                                     0, persRad/ratio, persRad,
                                     0, 0, 1);
}

template <class Dot>
void Perspective::getPersImg
(const cv::Mat &img, const cv::Mat &froMat,
 const bool isFront, cv::Mat &persImg) const
{
    cv::Mat resImg;
    Map::resizeImg(img, resImg);
    
    // 逆回転を計算
    const cv::Mat froMatInv = froMat.inv();
    
    // 出力画像
    persImg = cv::Mat::zeros(pfs, img.type());
    
    // 画素ごとに計算
    for (int v = 0; v < pfs.height; v++) {
        // 行の先頭のポインタ
        Dot* row = persImg.ptr<Dot>(v);
        
        // 画素を計算
        for (int u = 0; u < pfs.width; u++) {
            Pers pers(u, v);
            
            if (isInRange(pers, normalRad)) {
                //              // 回転前のカメラ座標
                Sphere sphere =
                Map::normal2sphere(Map::pers2normal(pers, inParaMat), isFront);
                
                // 回転後のカメラ座標
                Sphere sphereRot = Map::rotateSphere(sphere, froMatInv);
                
                // 画素の計算
                row[u] =
                Map::getBiliPixel<Dot>
                (resImg, Map::sphere2equirect(sphereRot));
            }
        }
    }
}

void Perspective::getPersImg
(const cv::Mat &img, const cv::Mat &froMat,
 const bool isFront, cv::Mat &persImg) const
{
    if (img.type() == CV_8UC1) {
        getPersImg<uchar>(img, froMat, isFront, persImg);
    } else if (img.type() == CV_8UC3) {
        getPersImg<cv::Vec3b>(img, froMat, isFront, persImg);
    }
}

cv::Mat Perspective::getMask(const float margin) const
{
    cv::Mat mask = cv::Mat::zeros(pfs, CV_8UC1);
    
    for (int u = 0; u < pfs.width; u++) {
        for (int v = 0; v < pfs.height; v++) {
            Pers pers(u, v);
            
            if (isInRange(pers, normalRad - margin)) {
                mask.at<uchar>(v, u) = 255;
            }
        }
    }
    
    return mask;
}

/*
void Perspective::persProjImg
(const cv::Mat &img, float rtheta, float rphi, cv::Mat &persedImg)
{
    float dud, dvd;
    calcPersLength(rtheta, rphi, dud, dvd);
    setPersCenter(dud / 2.0, dvd / 2.0);
    
    std::cout << -rtheta/2.0 << " <= " << rtheta/2.0 << std::endl;
    std::cout << -rphi/2.0 << " <= " << rphi/2.0 << std::endl;
    std::cout << "camera matrix = " << cameraMat << std::endl;
    
    int dudInt = (int) floorf(dud);
    int dvdInt = (int) floorf(dvd);

    std::cout << "width = " << dudInt << std::endl;
    std::cout << "height = " << dvdInt << std::endl;
    
    persedImg = cv::Mat(cv::Size(dudInt, dvdInt), CV_8UC3);
    
    for (int ud = 0; ud < dudInt; ud++) {
        for (int vd = 0; vd < dvdInt; vd++) {
            cv::Point2f pers(ud, vd);
            
            //float x, y;
            cv::Point2f regular;
            //pers2regular(ud, vd, x, y);
            pers2regular(pers, regular);
            
            //float theta, phi;
            cv::Point2f psphere;
            //regular2psphere(x, y, theta, phi);
            regular2psphere(regular, psphere);
            
            //float u, v;
            cv::Point2f equirect;
            //transform.psphere2equirect(theta, phi, u, v);
            transform.psphere2equirect(psphere, equirect);
            
            cv::Vec3b dot;
            //transform.getDotBilinear(img, u, v, dot);
            transform.getDotBilinear(img, equirect, dot);
            persedImg.at<cv::Vec3b>(vd, ud) = dot;
            
 
//            if (ud == 0 && vd == 0) {
//                std::cout << "(ud, vd) = (0, 0), " << "(theta, phi) = (" << theta << ", " << phi << ")" << std::endl;
//            }
//            if (ud == (int) dudInt/2 && vd == (int)dvdInt/2) {
//                std::cout << "(ud, vd) = (rud/2, dvd/2), " << "(theta, phi) = (" << theta << ", " << phi << ")" << std::endl;
//            }
//            if (ud == dudInt - 1 && vd == dvdInt - 1) {
//                std::cout << "(ud, vd) = (rud/2, dvd/2), " << "(theta, phi) = (" << theta << ", " << phi << ")" << std::endl;
//            }
 
        }
    }
    
    cv::Mat distCoeffs = (cv::Mat_<float>(5, 1) <<
                          -0.2107004, 0.06231665, -0.0008263, -0.00171503, -0.0109820);
    
    cv::Mat tmp = persedImg.clone();
    
   // cv::undistort(tmp, persedImg, cameraMat, distCoeffs);
}
*/

