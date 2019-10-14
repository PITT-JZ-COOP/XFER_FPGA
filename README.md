### Achieving Super-Linear Speedup across Multi-FPGA for Real-Time DNN Inference

```
@article{jiang2019achieving,
  title={Achieving Super-Linear Speedup across Multi-FPGA for Real-Time DNN Inference},
  author={Jiang, Weiwen and Sha, Edwin H-M and Zhang, Xinyi and Yang, Lei and Zhuge, Qingfeng and Shi, Yiyu and Hu, Jingtong},
  journal={ACM Transactions on Embedded Computing Systems (TECS)},
  volume={18},
  number={5s},
  pages={67},
  year={2019},
  publisher={ACM}
}
```
### This work has beed accepted by Proceeding of International Conference on Hardware/Software Codesign and System Synthesis (CODES+ISSS 2019).

We implement Convolutional Neural Networks (CNNs) on ZCU102 FPGA boards, connected by fiber cables (SFP+) via Aurora IP cores.

The implementations are based on Xilinx HLS and Vivado.

### Instructions:

#### DSE (Design Space Exploration)
This section shows the estimated latency given network layer parameters.

#### HLS
This section shows the work which contains the IP core of convilution accelerator, in which, it includes the interface to communicate with AXI DMA and interface to Xilinx Aurora 64/66B high speed seris transmission IP code

The interface between DRAM and FPGA is further optimized in our work.

#### SDK

This section shows how to schedule the running status of 1) two FPGA handshaking 2) single FPGA running schedule.

#### Vivado

The schematic of the design in Vivado is presented in this section. Please follow the schematic to build you own Vivado project.

Contact: jiang.wwen@pitt.edu, xinyizhang@pitt.edu
