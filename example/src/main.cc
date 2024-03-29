#include <iostream>

#include "ocr_api.h"

void Show(const std::string &res, void *other) {
  std::cout << res << std::endl;
}

int main() {
  std::string config_file =
      "/home/ocr_infer/data/config.ini";
  std::string image_dir =
      "/home/ocr_infer/testdata/e2e/image/";
  OcrInfer of;
  of.Init(config_file, Show, nullptr);
  of.Run(image_dir);
  return 0;
}
