# Block diagram of the AlexNet FPGA accelator system

## 4 DMA is instantiated to support IFM, WEIGHT, BIAS and OFM tranmission.
1. Raw data is transmitted from DDR via ZYNQ AXI HP port(MAX 4 ports * 128 bit in ZCU102).

2. DMA memory stream data width are set to 64 to sopport 2 AXI-STREAM(32 bit float) transmission (MAX stream data width is 128 for each DMA).

3. In DMA, M_AXIS_MM2S is for data from PS to PL, S_AXIS_S2MM is for data from PL to PS.

4. AXI_STREAM data split is done in IP generated from HLS.

5. MAX burst size is set to 256 to support max linear transfer.

## DMA AXI_STREAM data
1. DMA data transfer is parallel data transfer.

2. In ZCU 102, 4 float data can be transmitted in parallel from one ZYNQ AXI HP port(ZYNQ limitation).

3. Data to/from DMA has to be split and combined to form parallel transmission.

4. A DMA can enable bidirection function data stream to minimize # ofof DMA. 




