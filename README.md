### XFER: A Novel Design to Achieve Super-Linear Performance on Multiple FPGAs for Real-Time AI (Abstact Only)

```
@inproceedings{jiang2019XFER,  
  title={XFER: A Novel Design to Achieve Super-Linear Performance on Multiple FPGAs for Real-Time AI (Abstact Only)},  
  author={Weiwen Jiang, Xinyi Zhang, and others},  
  booktitle={Proceedings of the 2019 ACM/SIGDA International Symposium on Field-Programmable Gate Arrays},  
  year={2019},  
  organization={ACM}  
}
```
### This work has beed accepted by Proceeding of  International Conference on Hardware/Software Codesign and System Synthesis (CODES+ISSS 2019).

We implement Convolutional Neural Networks (CNNs) on ZCU102 FPGA boards, connected by fiber cables (SFP+) via Aurora IP cores.

The implementations are based on Xilinx HLS and Vivado.

### Instructions:

#### DSE (Design Space Exploration)
This section shows how to get the optimal parameters of each layer on targeted FPGA. The parameters are obtained under constraints such as off-chip/on-chip memory bandwidth, DSP resource and BRAM resource. The optimization goal can be set as latency or throughput.

#### HLS
This section shows the work which contains the IP core of convilution accelerator, in which, it includes the interface to communicate with AXI DMA and interface to Xilinx Aurora 64/66B high speed seris transmission IP code

The interface between DRAM and FPGA is further optimized in our work.

#### SDK

This section shows how to schedule the running status of 1) two FPGA handshaking 2) single FPGA running schedule.

#### Vivado

The schematic of the design in Vivado is presented in this section. Please follow the schematic to build you own Vivado project.

Contact: jiang.wwen@pitt.edu, xinyizhang@pitt.edu
