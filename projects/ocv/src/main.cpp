#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <gfx/file.hpp>

int main(int argc, char** argv)
{
    gfx::file f("brick.png");
    cv::Mat img = cv::imread(f.path.string(), cv::COLOR_BGR2YCrCb);
    if (img.empty())
    {
        fprintf(stderr, "Can not load image %s\n", f.path.string().c_str());
        return -1;
    }
    cv::Mat grayscale;
    cv::cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(grayscale, grayscale, { 5, 5 }, 2.f);
    cv::equalizeHist(grayscale, grayscale);
    cv::imshow("Out", grayscale);
    cv::waitKey();
    return 0;
}
