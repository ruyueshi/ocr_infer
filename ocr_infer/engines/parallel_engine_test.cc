#include "ocr_infer/engines/parallel_engine.h"

#include <iostream>
#include <string>

#include "glog/logging.h"
#include "gtest/gtest.h"

TEST(TestParallelEngine, test_parallel_engine) {
  ParallelEngine parallel_engine;
  std::string base_dir = "/home/chenlei/Documents/cnc/";
  std::string config_file = base_dir + "ocr_infer/data/config_cnc.ini";
  std::string image_dir = base_dir + "testdata/image/";
  std::string output_dir = base_dir + "output";

  auto callback_func = [](const std::string &out, const cv::Mat det_res,
                          void *other) { std::cout << out << std::endl; };
  ASSERT_EQ(
      parallel_engine.Init(config_file, callback_func, nullptr, output_dir), 0);
  ASSERT_EQ(parallel_engine.Run(image_dir, 500, 1500, 2000), 0);
}
