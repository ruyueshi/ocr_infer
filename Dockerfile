FROM nvidia/cuda:11.1.1-cudnn8-devel-ubuntu20.04
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Shanghai
RUN apt update && apt install -y vim git python3 python3-pip libtbb-dev libopencv-dev
  && apt install -y software-properties-common && add-apt-repository "deb https://developer.download.nvidia.cn/compute/machine-learning/repos/ubuntu1804/x86_64/ /"
  && apt update && apt install -y libnvinfer-dev libnvinfer-plugin-dev libnvonnxparsers-dev
  && apt install cmake

# install gtest and glog by building source code
RUN cd /home && git clone https://github.com/google/googletest.git -b v1.13.0
  && cmake -S . -B build && cmake --build build && cmake --build build --target install
RUN cd /home && git clone https://github.com/google/glog.git
  && cmake -S . -B build && cake --build build && cmake --build build --target install
