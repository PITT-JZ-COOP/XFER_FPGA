# This is a script to auto generate layer tiling parameters

This script consider design in the following circustance 1) off-chip <-> on-chip communication via DMA 2) the given FPGA bandwidth between DRAM and on-chip memory (targeted on Xilinx ZYNQ system ) 3) the given DSP and BRAM resource 4) the Xilinx Aurora 64/66B chip-chip tranmission protocol. 

The script can generate parameters for both single FPGA design and XFER design.

The optimal target can be switched among latency, throughput.
