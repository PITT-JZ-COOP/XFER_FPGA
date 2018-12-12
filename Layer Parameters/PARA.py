import os
import sys
import math
from math import *
import csv
import time

class NN:
    def __init__(self,B,M,N,R,C,K,S,P):
        self.B = B
        self.M = M
        self.N = N
        self.R = R
        self.C = C
        self.K = K
        self.S = S
        self.P = P
        
    def printf(self):
        print (self.B,self.M,self.N,self.R,self.C,self.K,self.S,self.P)

    def get_para(self):
        return float(self.B),float(self.M),float(self.N),float(self.R),float(self.C),float(self.K),float(self.S),float(self.P)

    def get_MAC(self):
        return self.B*self.R*self.C*self.M*self.K*self.K*self.N

    def get_operations(self):
        return self.get_MAC()*2

def layer_performance(ori_nn,Pb,Pr,Pc,Pm,Tm,Tn,Tr,Tc,BOARD,BITWIDTH,FREQ):
    [B,M,N,R,C,K,S,P] = ori_nn.get_para()
    OP = ori_nn.get_operations()
    PS=1
# AXI in_stream to IP, in_stream is 32 bit (bitwidth) by default.      
    if BOARD==2:
        tW_mem=Tm*Tn*K*K/W_p
        tW_net=Tm*Tn*K*K/A_p
        tI_mem=Tn*Tr*Tc/I_p
        tI_net=Tn*Tr*Tc/A_p
        tComp=K*K*Tr*Tc
        tO_mem=Tm*Tr*Tc/O_p   
    elif BOARD ==1:
        tW_mem=Tm*Tn*K*K/W_p
        tW_net=0
        tI_mem=Tn*Tr*Tc/I_p
        tI_net=0
        tComp=K*K*Tr*Tc
        tO_mem=Tm*Tr*Tc/O_p   
    	# tW_mem=0
    	# tW_net=0
    	# tI_mem=0
    	# tI_net=0
    	# tComp=K*K*Tr*Tc
    	# tO_mem=0

    EachLat = []
    Single_Result = []
    # single FPGA design
    Lat1 = max(tI_mem,tW_mem,tComp)
    Lat2 = max(ceil(N/Tn)*Lat1,tO_mem)
    Lat = B * ceil(R/Tr) * ceil(C/Tc) * ceil(M/Tm) * Lat2 #+ (tO_mem + Lat1)
    TH = float(OP)/(Lat)*FREQ*(10**6)
    EachLat = [tI_mem,tW_mem,tW_net,tComp,tO_mem/ceil(N/Tn)]  

    XFER_EachLat = []
    XFER_Result = []
    # XFER Design
    if(Pb*Pr*Pc==1 and Pm>1):
        XFER_Lat1 = max(tI_mem/Pm,tI_net/Pm,tW_mem,tComp)
        XFER_EachLat  = [tI_mem/Pm,tW_mem,tComp,tI_net/Pm,tO_mem/ceil(N/Tn)]
    elif(Pb*Pr*Pc>1 and Pm==1):
        XFER_Lat1 = max(tI_mem,tW_mem/(Pb*Pr*Pc),tW_net/(Pb*Pr*Pc),tComp)
        XFER_EachLat  = [tI_mem,tW_mem/(Pb*Pr*Pc),tW_net/(Pb*Pr*Pc),tComp,tO_mem/ceil(N/Tn)]
    elif(Pb*Pr*Pc>1 and Pm>1):
        XFER_Lat1 = max(tI_mem/Pm,tI_net/Pm,tW_mem/(Pb*Pr*Pc),tW_net/(Pb*Pr*Pc),tComp)
        XFER_EachLat  = [tI_mem/Pm,tW_mem/(Pb*Pr*Pc),tI_net/Pm,tComp,tO_mem/ceil(N/Tn)]
    else:
        XFER_Lat1 = max(tI_mem,tW_mem,tW_net,tComp)
    XFER_Lat2 = max(ceil(N/Tn)*XFER_Lat1,tO_mem)
    XFER_Lat = ceil(B/Pb) * ceil(ceil(R/Pr)/Tr) * ceil(ceil(C/Pc)/Tc) * ceil(ceil(M/Pm)/Tm) * XFER_Lat2# + (tO_mem + XFER_Lat1)
    XFER_TH = float(OP)/XFER_Lat*FREQ*(10**6) 

    bI = 2 * Tn * ceil(Tr*Tc*BITWIDTH/BRAM_SIZE)
    bO = 2 * Tm * ceil(Tr*Tc*BITWIDTH/BRAM_SIZE)
    if BITWIDTH ==32:
        bW = 2* Tm * Tn * ceil(K*K*BITWIDTH/BRAM_SIZE) #float
    elif BITWIDTH ==32:  
        bW = Tm * Tn * ceil(4*K*K*BITWIDTH/BRAM_SIZE)  #fix point             
    BRAM = bI+bO+bW
    BRAM_R=BRAM/BRAM_BOUND
   
    if(BITWIDTH == 16):
        DSP = Tm*Tn                                        
    elif(BITWIDTH==32):
        DSP = 5*Tm*Tn                                        
    else:
        DSP = 0
    DSP_R=DSP/DSP_BOUND

    Single_Result = [Lat, TH, DSP_R, BRAM_R]
    XFER_Result = [XFER_Lat, XFER_TH, DSP_R, BRAM_R]

    if BOARD == 1:
        return Single_Result
    elif BOARD == 2:
        return XFER_Result


if __name__ == "__main__":
    '''
    FPGA sprcifications are defined here.
    By default: 1) The BRAM unit size is 18K
                2) The frequency is in MHz
                3) Bitwidth can switch between 16 or 32
                4) if BOARD=1, it denotes single FPGA design; if BOARD=2, it denotes XFERdesign
                5) OPT_MODE=1 denotes throughput optimization, OPT_MODE=1 denotes latency optimization
    '''
    DSP_BOUND = 2520
    BRAM_BOUND = 1824
    BRAM_SIZE = 18000
    FREQ = 100
    BITWIDTH = 32
    BOARD = 1
    OPT_MODE = 1

    B=1
    Pb=1
    Pr=2
    Pc=1
    Pm=1

    W_p=4
    I_p=4
    O_p=4
    A_p=4

    ori_nn1 = NN(B,96,3,55,55,11,1,1) 
    ori_nn2 = NN(B,128,48,27,27,5,1,1)
    ori_nn3 = NN(B,384,256,13,13,3,1,1)
    ori_nn4 = NN(B,192,192,13,13,3,1,1)
    ori_nn5 = NN(B,128,192,13,13,3,1,1)

    layers=[ori_nn1, ori_nn2, ori_nn3, ori_nn4, ori_nn5]
    LAYER_PARA = []
    LAYER_LAT = []
    LAYER_TH = []


    for i in range(0,1):
        print(i)
        [B,M,N,R,C,K,S,P] = layers[i].get_para()
        MIN_LAT = sys.maxsize
        MAX_TH = 0
        para = []
        for Tr in range(1,int(R)+1,1): #R
            print(Tr)
            for Tc in range (1,int(C)+1,1): #C
                for Tm in range (1,int(M)+1): # Tm
                    for Tn in range(1,int(N)+1): # Tn
                        proposed_layer = layer_performance(layers[i],Pb,Pr,Pc,Pm,Tm,Tn,Tr,Tc,BOARD,BITWIDTH,FREQ)
                        
                        if proposed_layer[2] >= 1 or proposed_layer[3] >= 1:
                            continue
                        if OPT_MODE == 0: # latency optimization
                            if  proposed_layer[0]< MIN_LAT:
                                MIN_LAT = proposed_layer[0]
                                para=[]
                                para.append(Tm)
                                para.append(Tn)
                                para.append(Tr)
                                para.append(Tc)
                        elif OPT_MODE == 1: # throughtput optimization
                            if  proposed_layer[1]> MAX_TH:
                                MAX_TH = proposed_layer[1]
                                para=[]
                                para.append(Tm)
                                para.append(Tn)
                                para.append(Tr)
                                para.append(Tc)
        LAYER_PARA.append(para)
        if OPT_MODE == 0:
            LAYER_LAT.append(MIN_LAT)
        elif OPT_MODE == 1:
            LAYER_TH.append(MAX_TH)

    print(LAYER_PARA)
    print(LAYER_LAT)
    print(LAYER_TH)
    




   




