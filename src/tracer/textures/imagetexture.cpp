// #include "imagetexture.hpp"

// #include<util/util.hpp>

// #include<opencv2/core.hpp>
// #include<opencv2/imgcodecs.hpp>

// ImageTexture::ImageTexture(){}

// ImageTexture& ImageTexture::Set_Imgfile(std::string filename){
//   cv::samples::addSamplesDataSearchPath("./");
//   cv::samples::addSamplesDataSearchPath("../");
//   cv::samples::addSamplesDataSearchPath("../image/");
//   cv::samples::addSamplesDataSearchPath("../img/");
//   cv::samples::addSamplesDataSearchPath("./image/");
//   cv::samples::addSamplesDataSearchPath("./img/");

//   std::string filepath = cv::samples::findFile(filename.c_str());
//   Mat img_8u = cv::imread(filepath, cv::IMREAD_COLOR);

//   if(img_8u.empty()) throw std::runtime_error(std::format("FILE ERROR: cannot
//   open {} for image source {}", filepath, filename));

//   img_8u.convertTo(img, CV_64FC3, 1.0/255);

//   return *this;
// }

// Color ImageTexture::ColorAt(const Point2& pos){
//   int v = static_cast<int>(pos.y() * img.rows);
//   int u = static_cast<int>(pos.x() * img.cols);

//   double r = img.at<cv::Vec3d>(v,u)[2];
//   double g = img.at<cv::Vec3d>(v,u)[1];
//   double b = img.at<cv::Vec3d>(v,u)[0];

//   return Color(r,g,b);
// }
