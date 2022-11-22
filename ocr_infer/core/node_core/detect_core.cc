#include "ocr_infer/core/node_core/detect_core.h"

#include <thread>

#include "glog/logging.h"

DetectCore::DetectCore(const std::unordered_map<std::string, std::string> &config)
    : img_size_(512, 512) {
  LOG(INFO) << "Detect node init...";

  detector_num_ = std::stoi(Inquire(config, "detector_num"));
  int model_batch_size = std::stoi(Inquire(config, "detect_batch_size"));
  LOG(INFO) << "detector_ = " << detector_num_ << ", model_batch_size = " << model_batch_size;

  // TODO: to be more intelligent
  std::string model_path =
      "/home/chenlei/Documents/cnc/configuration/cnc_data/engines/2022/db_resnet_50.fp16";
  detector_.resize(detector_num_);
  for (int i = 0; i < detector_num_; i++) {
    detector_[i] = std::make_unique<Db>(model_path, model_batch_size);
  }

  LOG(INFO) << "Detect node init over!";
}

// TODO: 1. 节点内部并行
std::shared_ptr<DetOutput> DetectCore::Process(const std::shared_ptr<DetInput> &in) {
  VLOG(1) << "*** Detect node, in size = " << in->images.size();
  if (in->images.size() == 0) {
    return {};
  }

  auto out = std::make_shared<DetOutput>();

  for (cv::Mat &p : in->images) {
    out->images.emplace_back(p);
    out->scales.emplace_back(
        cv::Point2f((float)(img_size_.width) / p.cols, (float)(img_size_.height) / p.rows));
    if (p.depth() != CV_32FC3) {
      p.convertTo(p, CV_32FC3);
    }
    cv::resize(p, p, img_size_, 0, 0, cv::INTER_CUBIC);
  }
  out->names.assign(in->names.begin(), in->names.end());

  // TODO: to optimize
  std::vector<std::shared_ptr<std::thread>> threads;
  // 检测的batch_size最好是detector_num的整数倍，也最好是单个检测器的batch的整数倍
  int one_batch = in->names.size() / detector_num_;  // TODO: 考虑 one_batch 为 0 的情况
  std::vector<std::vector<cv::Mat>> output_result;
  output_result.resize(detector_num_);
  for (int i = 0; i < detector_num_; i++) {
    std::vector<cv::Mat> sub_mat(in->images.begin() + i * one_batch,
                                 in->images.begin() + (i + 1) * one_batch);
    // 注意：由于 i 和 sub_mat 是 for 循环里的局部变量，出了作用于就没了，因此在多线程的 lambda
    // 表达式中不能以引用的形式传递 可以值传递或者添加参数来传递
    threads.emplace_back(std::make_shared<std::thread>([this, &output_result, i, sub_mat]() {
      this->detector_[i]->Forward(sub_mat, &output_result[i]);
    }));
  }
  for (int i = 0; i < detector_num_; i++) {
    threads[i]->join();
    if (!output_result[i].empty()) {  // 考虑 output_result[i] 为空的情况
      out->results.insert(out->results.end(), output_result[i].begin(), output_result[i].end());
    }
  }

  VLOG(1) << "*** Detect node, out size = " << in->images.size();
  return out;
}
