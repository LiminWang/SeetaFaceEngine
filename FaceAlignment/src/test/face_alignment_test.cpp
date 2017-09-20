/*
 *
 * This file is part of the open-source SeetaFace engine, which includes three modules:
 * SeetaFace Detection, SeetaFace Alignment, and SeetaFace Identification.
 *
 * This file is an example of how to use SeetaFace engine for face alignment, the
 * face alignment method described in the following paper:
 *
 *
 *   Coarse-to-Fine Auto-Encoder Networks (CFAN) for Real-Time Face Alignment, 
 *   Jie Zhang, Shiguang Shan, Meina Kan, Xilin Chen. In Proceeding of the
 *   European Conference on Computer Vision (ECCV), 2014
 *
 *
 * Copyright (C) 2016, Visual Information Processing and Learning (VIPL) group,
 * Institute of Computing Technology, Chinese Academy of Sciences, Beijing, China.
 *
 * The codes are mainly developed by Jie Zhang (a Ph.D supervised by Prof. Shiguang Shan)
 *
 * As an open-source face recognition engine: you can redistribute SeetaFace source codes
 * and/or modify it under the terms of the BSD 2-Clause License.
 *
 * You should have received a copy of the BSD 2-Clause License along with the software.
 * If not, see < https://opensource.org/licenses/BSD-2-Clause>.
 *
 * Contact Info: you can send an email to SeetaFace@vipl.ict.ac.cn for any problems.
 *
 * Note: the above information must be kept whenever or wherever the codes are used.
 *
 */
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "face_detection.h"
#include "face_alignment.h"

#define PI 3.14159265

static void show_usage(std::string app) {
  std::cerr << "Usage: " << app << " <option(s)>" << std::endl
    << "Options:" << std::endl
    << "\t-h,--help\t\tShow this help message" << std::endl
    << "\t-i,--input FILE\t\tSpecify the input image for face alignment testing" << std::endl
    << "\t-o,--output FILE\t\tSpecify the output face aligment image" << std::endl
    << "\t-m,--model MODEL_PATH\t\tSpecifiy the models path" << std::endl
    << std::endl;
}

int main(int argc, char** argv)
{
  std::string input_img  = "./data/image_0001.png";
  std::string output_img = "./result_0001.png";
  std::string model_path = "./model";
  std::string model_fd_name = "seeta_fd_frontal_v1.0.bin";
  std::string model_fa_name = "seeta_fa_v1.1.bin";
  std::string model_fd;
  std::string model_fa;

  if (argc < 2) {
    show_usage(argv[0]);
    return 1;
  }

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(argv[0]);
      return 0;
    } else if ((arg == "-i") || (arg == "--input")) {
      if (i + 1 < argc) {
        input_img = argv[++i];
      } else {
        std::cerr << "the option requires one argument." << std::endl;
        return -1;
      }
    } else if ((arg == "-o") || (arg == "--output")) {
      if (i + 1 < argc) {
        output_img = argv[++i];
      } else {
        std::cerr << "the option requires one argument." << std::endl;
        return -1;
      }
    } else if ((arg == "-m") || (arg == "--model")) {
      if (i + 1 < argc) {
        model_path = argv[++i];
      } else {
        std::cerr << "the option requires one argument." << std::endl;
        return -1;
      }
    }
  }

  model_fd = model_path + "/" +  model_fd_name;
  model_fa = model_path + "/" + model_fa_name;

  // Initialize face detection model
  seeta::FaceDetection detector(model_fd.c_str());
  detector.SetMinFaceSize(40);
  detector.SetScoreThresh(2.f);
  detector.SetImagePyramidScaleFactor(0.8f);
  detector.SetWindowStep(4, 4);

  // Initialize face alignment model 
  seeta::FaceAlignment point_detector(model_fa.c_str());

  //load image
  cv::Mat img = cv::imread(input_img, cv::IMREAD_UNCHANGED);
  if (!img.data)
  {
    std::cerr << "Could not open or find the image" << std::endl;
    return -1;
  }

  cv::Mat img_gray;
  if (img.channels() != 1)
    cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
  else
    img_gray = img;

  int pts_num = 5;

  seeta::ImageData img_data;
  img_data.data = img_gray.data;
  img_data.width = img_gray.cols;
  img_data.height = img_gray.rows;
  img_data.num_channels = 1;

  // Detect faces
  std::vector<seeta::FaceInfo> faces = detector.Detect(img_data);
  if (faces.empty())
  {
    return 0;
  }

  for (auto face : faces) {
    // Detect 5 facial landmarks
    seeta::FacialLandmark points[5];
    point_detector.PointDetectLandmarks(img_data, face, points);

    // Visualize the results
    cv::RotatedRect rrect = cv::RotatedRect(cv::Point(face.bbox.x + face.bbox.width / 2, face.bbox.y + face.bbox.height / 2), cv::Size(face.bbox.width, face.bbox.height), atan((points[1].y - points[0].y) / (points[1].x - points[0].x)) * 180 / PI);
    cv::Point2f vertices[4];
    rrect.points(vertices);

    for (int i = 0; i < 4; i++)
      cv::line(img, vertices[i], vertices[(i + 1) % 4], CV_RGB(255, 0, 0), 2, CV_AA);

    for (int i = 0; i < pts_num; i++) {
      cv::circle(img, cvPoint(points[i].x, points[i].y), 2, CV_RGB(0, 255, 0), CV_FILLED);
    }
  }

  cv::namedWindow("Test", cv::WINDOW_AUTOSIZE);
  cv::imshow("Alignment", img);
  cv::waitKey(0);
  cv::destroyWindow("Test");

  return 0;
}
