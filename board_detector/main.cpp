#include <opencv2/opencv.hpp>
#include <stdio.h>

bool getPictureFileName(std::string& fileName, int index) {
  char buf[255];
  sprintf(buf, "./temp/%d.jpg", index);
  fileName = buf;
  return true;
}

bool preprocess(const cv::Mat& src, cv::Mat& dst) {
  cv::Mat tmp = src.clone();
  
  cv::erode(tmp, dst, cv::Mat(), cv::Point(-1, -1));
  cv::erode(dst, tmp, cv::Mat(), cv::Point(-1, -1));
  cv::dilate(tmp, dst, cv::Mat(), cv::Point(-1, -1));
  cv::dilate(dst, tmp, cv::Mat(), cv::Point(-1, -1));
  
  
  cv::dilate(tmp, dst, cv::Mat(), cv::Point(-1, -1));
  cv::dilate(dst, tmp, cv::Mat(), cv::Point(-1, -1));
  cv::erode(tmp, dst, cv::Mat(), cv::Point(-1, -1));
  cv::erode(tmp, dst, cv::Mat(), cv::Point(-1, -1));
  dst = tmp;

  return true;
}

bool getBlackRegion(const cv::Mat& input, cv::Mat& region) {
  cv::Mat black = cv::Mat::zeros(input.rows, input.cols, CV_8UC3);
  cv::Mat diffColor = cv::Mat(input.rows, input.cols, CV_8UC3);
  cv::Mat diff = cv::Mat::zeros(input.rows, input.cols, CV_8UC1);

  // 黒色との差分(3チャンネル分)
  cv::absdiff(input, black, diffColor);

  // 各チャンネルを分割
  cv::Mat diffB = cv::Mat(input.rows, input.cols, CV_8UC1);
  cv::Mat diffG = cv::Mat(input.rows, input.cols, CV_8UC1);
  cv::Mat diffR = cv::Mat(input.rows, input.cols, CV_8UC1);
  std::vector<cv::Mat> splitted;
  splitted.push_back(diffB);
  splitted.push_back(diffG);
  splitted.push_back(diffR);
  cv::split(diffColor, splitted);

  // 各チャンネルの差分を合計する
  diff = diffB + diffG + diffR;

  // cv::threshold(diff, region, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
  cv::inRange(diff, 0, 150, region);

  return true;
}


bool findConvexHullOfBlackBoard(const cv::Mat& src, cv::Mat& dst, cv::RotatedRect& result) {
  // 輪郭
  typedef std::vector <std::vector<cv::Point> > Contours;
  Contours contours;

  // 輪郭を探す
  cv::findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  cv::Mat lineImage = cv::Mat(src.rows, src.cols, CV_8UC1);

  Contours::iterator it = contours.begin();
  while (it != contours.end()) {
    // 面積があまりに小さい場合は却下
    double contourArea = cv::contourArea(*it);
    if (contourArea < 100) {
      it++;
      continue;
    }

    // 凸包を算出
    std::vector<cv::Point> hull;
    cv::convexHull(*it, hull);

    // 凸包よりOBBを算出
    cv::RotatedRect bb = cv::minAreaRect(*it);
    if (bb.size.width == 0 || bb.size.height == 0) {
      // OBBが算出できなかった場合
      it++;
      continue;
    }

    // 凸包/OBBの面積
    double hullArea = cv::contourArea(hull);
    double obbArea  = bb.size.width * bb.size.height;

    // デバッグ用/暫定
    const cv::Point *ppt[] = {&hull[0]};
    int npt[1] = {hull.size()};
    lineImage = cv::Scalar(0, 0, 0);
    cv::polylines(lineImage, ppt, npt, 1, true, cv::Scalar(255, 255, 255));
    std::vector<cv::Vec4i> houghLines;
    cv::HoughLinesP(lineImage, houghLines, 1, 3.14159/180, 10, 20, 5);

    // houghLines.size() &&
    if (hullArea > 10000 &&
        obbArea / hullArea < 1.1 &&
        hullArea/contourArea < 1.5) {
      // 凸包面積/凸包とOBBの面積比/輪郭と凸包の面積比が規定内

      // アスペクト比
      double aspect1 = (float)bb.size.height / bb.size.width;
      double aspect2 = (float)bb.size.width / bb.size.height;
      if (bb.size.height > 0 && bb.size.width > 0 &&
          ((1.1 < aspect1 && aspect1 < 1.5) ||
           (1.1 < aspect2 && aspect2 < 1.5))) {

        printf("area: %lf\n", hullArea);
        result = bb;
        break;
      }
    }
    
    it++;
  }

  return true;
}

int main(int argc, char* argv) {
  cv::Mat input;

  const char* inputWindow = "INPUT";

  cv::namedWindow(inputWindow);

  for (int i=1; i<10; i++) {
    std::string fileName;
    getPictureFileName(fileName, i);

    std::cout << fileName << std::endl;

    try {
      input = cv::imread(fileName);
    }
    catch ( ... ) {
      break;
    }

    cv::Mat black = cv::Mat(input.rows, input.cols, CV_8UC1);
    cv::Mat filtered = cv::Mat(input.rows, input.cols, CV_8UC1);
    
    getBlackRegion(input, black);
    preprocess(black, filtered);

    // cv::imshow(inputWindow, black);
    // cv::waitKey(0);

    // cv::imshow(inputWindow, filtered);
    // cv::waitKey(0);
    cv::RotatedRect bb;
    findConvexHullOfBlackBoard(filtered, black, bb);
    printf("%lf %lf\n", bb.size.width, bb.size.height);

    if (!(bb.size.width == 0 || bb.size.height == 0)) {
      cv::Point2f vtx[4];
      bb.points(vtx);
      for(int i=0; i<4; ++i)
        cv::line(input, vtx[i], vtx[i<3?i+1:0], cv::Scalar(255, 0, 0), 2, CV_AA);

      printf("%5.2f\n", (float)bb.size.width / bb.size.height);
      printf("%5.2f\n", bb.size.height / (float)bb.size.width);
      cv::ellipse(input, bb, cv::Scalar(255, 0 ,0));
    }
    cv::imshow("INPUT", input);
    cv::waitKey(0);

  }
  
  return 0;
}
