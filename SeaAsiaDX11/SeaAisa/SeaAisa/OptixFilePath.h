#pragma once


#define SAMPLES_DIR "./OptiX SDK 6.0.0"
#define SAMPLES_PTX_DIR ".\\OptiX SDK 6.0.0"

// Include directories
#define SAMPLES_RELATIVE_INCLUDE_DIRS \
  "/sutil", \
  "/cuda", 
#define SAMPLES_ABSOLUTE_INCLUDE_DIRS \
  "./OptiX SDK 6.0.0/include", \
  "./OptiX SDK 6.0.0/include/optixu", \
  "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.0/include", 

// Signal whether to use NVRTC or not
#define CUDA_NVRTC_ENABLED 1

// NVRTC compiler options
#define CUDA_NVRTC_OPTIONS  \
  "-arch", \
  "compute_30", \
  "-use_fast_math", \
  "-lineinfo", \
  "-default-device", \
  "-rdc", \
  "true", \
  "-D__x86_64", \
  0,
