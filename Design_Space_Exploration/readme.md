# This is a script to auto generate layer tiling parameters

### The method to explore the NN design space on FPGA is referring to "Optimizing FPGA-based Accelerator Design for Deep Convolutional Neural Networks"

Base on the reference methodology, this script considers design in the following circustance 1) off-chip&on-chip communicates via DMA 2) FPGA bandwidth between DRAM and on-chip memory is given (targeted on Xilinx ZYNQ system ) 3) DSP and BRAM resource is goven 4) the Xilinx Aurora 64/66B chip-chip tranmission protocol is adopted. 

The script can generate parameters for both single FPGA design and XFER design. The optimal target can be switched between latency and throughput.

The script can return the best performance in through/latency for a certain Neural Network layer.
