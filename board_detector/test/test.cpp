#include <stdio.h>
#include <BoardDetection.hpp>


bool getPictureFileName(std::string& fileName, int index) {
  char buf[255];
  sprintf(buf, "../temp/%d.jpg", index);
  fileName = buf;
  return true;
}

using namespace BoardDetection;

int main(int argc, char* argv) {
  cv::Mat input;

  const char* inputWindow = "INPUT";

  cv::namedWindow(inputWindow);

  for (int i=1; i<10; i++) {
    std::string fileName;
    getPictureFileName(fileName, i);
    cv::RotatedRect bb;
    std::cout << fileName << std::endl;

    // デバッグ表示用
    try {
      input = cv::imread(fileName);
    }
    catch ( ... ) {
      break;
    }

    // 黒板領域抽出
    findBlackBoardRegion(fileName, bb);

    // 黒板領域描画
    if (!(bb.size.width == 0 || bb.size.height == 0)) {
      cv::Point2f vtx[4];
      bb.points(vtx);
      for(int i=0; i<4; ++i)
        cv::line(input, vtx[i], vtx[(i+1)%4], cv::Scalar(255, 0, 0), 2, CV_AA);

      printf("%5.2f\n", (float)bb.size.width / bb.size.height);
      printf("%5.2f\n", bb.size.height / (float)bb.size.width);
      cv::ellipse(input, bb, cv::Scalar(255, 0 ,0));
    }
    cv::imshow("INPUT", input);
    cv::waitKey(0);

  }
  
  return 0;
}
