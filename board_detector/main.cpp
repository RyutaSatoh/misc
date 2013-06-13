#include <opencv2/opencv.hpp>
#include <stdio.h>

bool getPictureFileName(std::string& fileName, int index) {
  char buf[255];
  sprintf(buf, "./bijin/%d.jpg", index);
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
  cv::erode(dst, tmp, cv::Mat(), cv::Point(-1, -1));
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

  cv::inRange(diff, 0, 100, region);

  return true;
}

bool findConvexHullOfBlackBoard(const cv::Mat& src, cv::Mat& dst) {
  typedef std::vector <std::vector<cv::Point> > Contours;
  Contours contours;

  cv::findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  cv::Mat lineImage = cv::Mat(src.rows, src.cols, CV_8UC1);
  Contours::iterator it = contours.begin();

  while (it != contours.end()) {

    std::vector<cv::Point> hull;
    cv::convexHull(*it, hull);

    double contourArea = cv::contourArea(*it);
    double hullArea = cv::contourArea(*it);

    const cv::Point *ppt[] = {&hull[0]};
    int npt[1] = {hull.size()};
    lineImage = cv::Scalar(0, 0, 0);
    cv::polylines(lineImage, ppt, npt, 1, true, cv::Scalar(255, 255, 255));

    std::vector<cv::Vec4i> houghLines;
    cv::HoughLinesP(lineImage, houghLines, 1, 3.14159/180, 10, 20, 5);

    if (houghLines.size() > 3 && hullArea > 500 && hullArea / contourArea < 1.3) {
      cv::imshow("INPUT", lineImage);
      cv::waitKey(0);
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

    findConvexHullOfBlackBoard(filtered, black);

    cv::imshow(inputWindow, filtered);
    cv::waitKey(0);
  }
  
  return 0;
}
