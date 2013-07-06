#ifndef __BOARD_DETECTION_HPP__
#define __BOARD_DETECTION_HPP__

#include <opencv2/opencv.hpp>
#include <stdio.h>

namespace BoardDetection {
  /**
   * 2値画像に対する前処理
   * @param src 入力画像
   * @param dst 出力画像
   */
  bool preprocess(const cv::Mat& src, cv::Mat& dst);

  /**
   * 黒板候補領域の抽出
   * @param input 入力画像
   * @param region 候補領域
   */
  bool getBlackRegion(const cv::Mat& input, cv::Mat& region);

  
  /**
   * 黒板の凸包を探す
   * @param src 入力画像
   * @param result 結果のOBB
   */
  bool findConvexHullOfBlackBoard(const cv::Mat& src, cv::Mat& dst, cv::RotatedRect& result);
}

#endif // __BOARD_DETECTION_HPP__
