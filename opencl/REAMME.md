./amdgpu-pro-install -y

sudo apt install ocl-icd-opencl-dev
sudo apt install opencl-headers
sudo apt install opencl-rocr-amdgpu-pro

sudo apt install clinfo

g++ -std=c++0x main.cpp -o main.out -lOpenCL -D CL_HPP_TARGET_OPENCL_VERSION=200
