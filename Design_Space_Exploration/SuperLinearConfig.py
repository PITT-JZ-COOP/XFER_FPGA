#-*- coding: UTF-8 -*-


# define your board resource limitation here
DSP_BOUND = 2520   # number of DSPs
BRAM_BOUND = 1824  # number of block RAMs
BRAM_SIZE = 18000  # depth of a single block RAM
FREQ = 100         # working frequency (MHz)
BITWIDTH = 32	   # tensor and weight bitwidth (32 or 16)
BOARD = 1		   # number of boards 

# define a layer parameters
B = 2  # batch size
M = 128 # output channel
N = 192  #input channel
R = 13 # output row
C = 13 # output col
K = 3 # weight kernel size
S = 1  # stride
P = 1  # padding

# define your system parameters here valid when 2 FPGAs are in evaluation
Pb=1   # level of batch split
Pr=1  # level of tensor row split
Pc=1   # level o tensor rol split
Pm=1   # level of output channel splt 

# data transfer parallelism e.g W_p=2, 2 single weight is transferd simultaneously
W_p=2 # weight
I_p=2 # input tensor
O_p=2 # output tensor
A_p=2 # to Aurorap IP 

# tile cutting parameters
Tm=8
Tn=32
Tr=13
Tc=13