x#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <stdio.h>

#define MOVIE_FILE "./irvpd002.mov"

int main(int argc,  char* argv[]) {
  CvCapture *capture = NULL;

  if (argc > 1) {
    capture  = cvCaptureFromFile(argv[1]);
  }

  if (capture == NULL) {
    fprintf(stderr, "cannot open the file: %s¥n", argv[1]);
    return 1;
  }
      
  cvNamedWindow("test");

  while (1) {
    IplImage* frame = cvQueryFrame(capture);

    if (frame == NULL) {
      return 1;
    }
    cvShowImage("test", frame);
    cvWaitKey(10);
  }  
}
