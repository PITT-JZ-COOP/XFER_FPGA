# Block diagram of the AlexNet FPGA accelator system

## 4 DMA is instantiated to support IFM, WEIGHT, BIAS and OFM tranmission.
Raw data is transmitted from DDR via ZYNQ AXI HP port(MAX 4 ports * 128 bit in ZCU102).
DMA memory stream data width are set to 64 to sopport 2 AXI-STREAM(32 bit float) transmission (MAX stream data width is 128 for each DMA).
