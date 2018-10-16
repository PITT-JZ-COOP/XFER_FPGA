# HLS source code (Verified on Xilinx ZCU102 board)

### FPGA working frequency is set as 100MHz.

### 32-bit float-point data is adopted in this project.

### All data structures are created in "source.h".

### DMA is adopted in this project to load/off-load IFM, WEIGHT, BIAS, and OFM.

### Double buffer design to redece execution stall.

## LOAD_WEIGHT_DMA
1. 64 bit stram data (2 float point data) to WEIGHT(float point) in PL.

2. Stream data is split in this function

3. #pragma HLS PIPELINE II=1 is adopted to enable pipeline transfer.

## LOAD_IFM
1. 64 bit stram data (2 float point data) to WEIGHT(float point) in PL.

2. Stream data is split in this function

3. #pragma HLS PIPELINE II=1 is adopted to enable pipeline transfer.

## FIRE
1. 6 loops in a convolution kernal.

2. The last two loops are unrolled and executed in parallel.

3. #pragma HLS PIPELINE II=1 is adopted to enable 2-loop unroll.

## store

similar to LOAD_WEIGHT_DMA and LOAD_IFM

## Load_Fire
Double buffer deisgn for "load IFM, WEIGHT and BIAS" and "Fire"

## top
Double buffer deisgn for "load IFM, WEIGHT and BIAS + Fire" and "store"

## BRAM instantion

1. WEIGHT, IFM, OFM and BIAS are double buffered

2. WEIGHT dimension 1 and 2 are completely partitioned to support parallel computation.

3. IFM dimension 1 is completely partitioned to support parallel computation

## variables

1. Variables such as row, col, N, custom_k... can be customized tuned according to different FPGA.
