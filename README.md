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

We implement Convolutional Neural Networks (CNNs) on ZCU102 FPGA boards, connected by fiber cables (SFP+) via Aurora IP cores.

The implementations are based on Xilinx HLS and Vivado.

In the current repository, we open the base-line design (the single-FPGA implementation on ZCU102).

### Instructions:

#### DSE (Design Space Exploration)
This section shows how to get the optimal parameters of each layer on targeted FPGA. The parameters are obtained under constraints such as off-chip/on-chip memory bandwidth, DSP resource and BRAM resource. The optimization goal can be set as latency or throughput.

#### HLS
This section shows the work which is proposed by researcher Cheng Zhang. The work can be found in [Spring-data-jpa 查询  复杂查询陆续完善中](http://www.cnblogs.com/sxdcgaq8080/p/7894828.html)

#### SDK

#### Vivado

Contact: jiang.wwen@pitt.edu, xinyizhang@pitt.edu
