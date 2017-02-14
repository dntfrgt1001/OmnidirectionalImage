//
//  Core.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Core.hpp"

void Map::polar2equirect
(const std::vector<Polar>& polars, std::vector<Equirect>& equirects)
{
    points2points<Polar, Equirect, &polar2equirect>(polars, equirects);
}

void Map::equirect2polar
(const std::vector<Equirect> &equirects, std::vector<Polar> &polars)
{
    points2points<Equirect, Polar, &equirect2polar>(equirects, polars);
}

void Map::polar2sphere
(const std::vector<Polar> &polars, std::vector<Sphere> &spheres)
{
    points2points<Polar, Sphere, &polar2sphere>(polars, spheres);
}

void Map::sphere2polar
(const std::vector<Sphere> &spheres, std::vector<Polar> &polars)
{
    points2points<Sphere, Polar, &sphere2polar>(spheres, polars);
}

void Map::equirect2sphere
(const std::vector<Equirect> &equirects, std::vector<Sphere> &spheres)
{
    points2points<Equirect, Sphere, &equirect2sphere>(equirects, spheres);
}

void Map::sphere2equirect
(const std::vector<Sphere> &spheres, std::vector<Equirect> &equirects)
{
    points2points<Sphere, Equirect, &sphere2equirect>(spheres, equirects);
}

void Map::sphere2normal
(const std::vector<Sphere> &spheres, std::vector<Normal> &normals)
{
    points2points<Sphere, Normal, &sphere2normal>(spheres, normals);
}

void Map::normal2pers
(const std::vector<Normal> &normals, std::vector<Pers> &perss,
 const cv::Mat &inParaMat)
{
    points2points<Normal, Pers, &normal2pers>(normals, perss, inParaMat);
}

void Map::pers2normal
(const std::vector<Pers> &perss, std::vector<Normal> &normals,
 const cv::Mat &inParaMat)
{
    points2points<Pers, Normal, &pers2normal>(perss, normals, inParaMat);
}

void Map::normal2point
(const std::vector<Normal> &normals, std::vector<cv::Point2f> &points)
{
    points2points<Normal, cv::Point2f, &normal2point>(normals, points);
}


template <class forTp, class latTp, static latTp func(const forTp&)>
void Map::points2points
(const std::vector<forTp>& forPoints, std::vector<latTp>& latPoints)
{
    latPoints = std::vector<latTp>(forPoints.size());
    
    for (int i = 0; i < forPoints.size(); i++) {
        latPoints[i] = func(forPoints[i]);
    }
}

template
<class forTp, class latTp, static latTp func(const forTp&, const cv::Mat&)>
void Map::points2points
(const std::vector<forTp>& forPoints, std::vector<latTp>& latPoints,
 const cv::Mat& mat)
{
    latPoints = std::vector<latTp>(forPoints.size());
    
    for (int i = 0; i < forPoints.size(); i++) {
        latPoints[i] = func(forPoints[i], mat);
    }
}

void Map::rotateSphere
(const std::vector<Sphere> &spheres, std::vector<Sphere> &spheresRot,
 const cv::Mat &rotMat)
{
    points2points<Sphere, Sphere, rotateSphere>(spheres, spheresRot, rotMat);
}

template <class Dot>
void Map::rotateImg(const cv::Mat &img, const cv::Mat &rotMat, cv::Mat &rotImg)
{
    // 入力画像と同じタイプ，サイズの出力画像を生成
    rotImg = cv::Mat_<Dot>(img.size());
    
    // 逆回転を計算
    cv::Mat invRotMat = rotMat.inv();
    
    // 画素ごとに計算
    for (int vr = 0; vr < fs.height; vr++) {
        // 行の先頭のポインタ
        Dot* row = rotImg.ptr<Dot>(vr);
        
        // 回転後の座標から回転前の座標を計算し，線形補間で色を計算
        for (int ur = 0; ur < fs.width; ur++) {
            row[ur] =
            getBiliPixel<Dot>(img, rotateEquirect(Equirect(ur, vr), invRotMat));
        }
    }
}

void Map::rotateImg
(const cv::Mat &img, const cv::Mat &rotMat, cv::Mat &rotImg)
{
    if (img.type() == CV_8UC1) {
        // グレースケール
        rotateImg<uchar>(img, rotMat, rotImg);
    } else if (img.type() == CV_8UC3) {
        // RGB
        rotateImg<cv::Vec3b>(img, rotMat, rotImg);
    }
}

template <class Dot>
void Map::rotateImgVert
(const cv::Mat &img, const float angle, const cv::Rect &rect, cv::Mat &rotImg)
{
    // 入力画像と同じタイプ，サイズの出力画像を生成
    rotImg = cv::Mat_<Dot>(img.size());
    
    // 逆回転を計算
    const float invAngle = - angle;
    
    // 有効範囲の幅の座標を計算
    const int rectBottom = rect.y + rect.height;
    const int rectRight = rect.x + rect.width;
    
    // 画素ごとに計算
    for (int vr = rect.y; vr < rectBottom; vr++) {
        // 行の先頭のポインタ
        Dot* row = rotImg.ptr<Dot>(vr);
        
        // 回転後の座標から回転前の座標を計算し，線形補間で色を計算
        for (int ur = rect.x; ur < rectRight; ur++) {
            row[ur] =
            getBiliPixel<Dot>(img, rotateEquirectVert(Equirect(ur, vr),
                                                      invAngle));
        }
    }
}

void Map::rotateImgVert
(const cv::Mat &img, const float angle, const cv::Rect &rect, cv::Mat &rotImg)
{
    if (img.type() == CV_8UC1) {
        // グレースケール
        rotateImgVert<uchar>(img, angle, rect, rotImg);
    } else if (img.type() == CV_8UC3) {
        // RGB
        rotateImgVert<cv::Vec3b>(img, angle, rect, rotImg);
    }
}

cv::Size Map::fs;
cv::Size Map::fsProc;
cv::Size Map::fsOut;

