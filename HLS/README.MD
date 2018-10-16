# HLS source code

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
