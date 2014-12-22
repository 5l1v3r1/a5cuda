#ifndef __A5_CUDA_SLICE__
#define __A5_CUDA_SLICE__

#include "A5Cuda.h"
#include "kernel/A5CudaKernel.h"

#define MEMORY_ALIGNMENT 4096
#define ALIGN_UP(x, size) (((size_t)x + (size - 1)) & (~(size - 1)))

#define DATA_SIZE 4096
#define BLOCK_SIZE 512

#define CHAINSTATE_RUNNING  0
#define CHAINSTATE_FINISHED 1
#define CHAINSTATE_LASTROUND   2

class A5Cuda;

class A5CudaSlice {
    private:
        enum eState {
            ePopulate,  // initial state, to zeroing all slot
            eKernel,    // kernel is running
            eProcess    // CPU does its job after GPU has done its part
        };
        eState state;

        cudaStream_t stream;
        // parameter slice
        uint64_t finishedMask;
        unsigned int mMaxCycles;
        unsigned int mMaxRound;
        unsigned int mCycles;
        unsigned int mDp;
        bool isRunning;

        // pull model, controller is to pull data
        A5Cuda* controller;

        // chain's data stored on host side
        // using only on host side
        A5Cuda::JobPiece_s* jobs;

        // chain's data frequently changed
        // stored on pinned host memory, mapped to device memory
        // hm = host mappe; d = device
        uint64_t* hm_states;
        uint64_t* d_states;
        uint64_t* hm_advances;
        uint64_t* d_advances;
        unsigned int* hm_control;
        unsigned int* d_control;

        uint64_t ReverseBits(uint64_t);
        void populate();
        void invokeKernel();
        void process();

    public:
        A5CudaSlice(A5Cuda* controller, int deviceId, int dp, unsigned int maxRound);
        ~A5CudaSlice();

        int getTotalSlots();
        void tick();
};

#endif
