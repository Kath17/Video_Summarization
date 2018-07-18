BIN := vsumm-gpu

CC	  = g++ -std=c++11 -O3 -fopenmp

CUDA_INSTALL_PATH ?= /usr/local/cuda
NVCC ?= $(CUDA_INSTALL_PATH)/bin/nvcc
NVCCFLAGS := -arch=sm_30

LIBS      = -L/home/kat/opencv-2.4.13.5/release/lib /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so /usr/lib/x86_64-linux-gnu/libopencv_contrib.so /usr/lib/x86_64-linux-gnu/libopencv_core.so /usr/lib/x86_64-linux-gnu/libopencv_features2d.so /usr/lib/x86_64-linux-gnu/libopencv_flann.so /usr/lib/x86_64-linux-gnu/libopencv_gpu.so /usr/lib/x86_64-linux-gnu/libopencv_highgui.so /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so /usr/lib/x86_64-linux-gnu/libopencv_legacy.so /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so /usr/lib/x86_64-linux-gnu/libopencv_ocl.so /usr/lib/x86_64-linux-gnu/libopencv_photo.so /usr/lib/x86_64-linux-gnu/libopencv_stitching.so /usr/lib/x86_64-linux-gnu/libopencv_superres.so /usr/lib/x86_64-linux-gnu/libopencv_ts.a /usr/lib/x86_64-linux-gnu/libopencv_video.so /usr/lib/x86_64-linux-gnu/libopencv_videostab.so /usr/local/cuda/lib64/libcufft.so /usr/local/cuda/lib64/libcublas.so /usr/local/cuda/lib64/libnpps.so /usr/local/cuda/lib64/libnppi.so /usr/local/cuda/lib64/libnppc.so /usr/local/cuda/lib64/libcudart.so -lrt -lpthread -lm -ldl
INCLUDES  = -I/home/kat/opencv-2.4.13.5/release/include -I"$(CUDA_INSTALL_PATH)/include"

CPP_SRCS  := $(wildcard *.cpp)
CU_SRCS   := $(wildcard *.cu)
HEADERS   := $(wildcard *.h)
CPP_OBJS  := $(patsubst %.cpp, %.o, $(CPP_SRCS))
CU_OBJS  := $(patsubst %.cu, %.cu_o, $(CU_SRCS))

%.cu_o: %.cu
	$(NVCC) $(NVCCFLAGS) -c $(INCLUDES) -o $@ $<

%.o: %.cpp
	$(CC) $(INCLUDES) -c $< -o $@

$(BIN): $(CPP_OBJS) $(CU_OBJS)
	$(CC) -o $(BIN) $(CU_OBJS) $(CPP_OBJS) $(LIBS) $(CFLAGS)
clean:
	\rm *.o *.cu_o $(BIN)
