#-*- coding: UTF-8 -*-

import os
import sys
import math
from math import *
import csv
import time

class Network:
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

    def getPara(self):
        return float(self.B),float(self.M),float(self.N),float(self.R),float(self.C),float(self.K),float(self.S),float(self.P)

    def getMAC(self):
        return self.B*self.R*self.C*self.M*self.K*self.K*self.N

    def getOperations(self):
        return self.getMAC()*2

class Performance:
    def __init__(self, layer, TileInfo, Board, XferConfig):
        self.layer = layer
        self.TileInfo = TileInfo
        self.Board = Board
        self.XferConfig = XferConfig

    def layerPerformance(self):
        [B,M,N,R,C,K,S,P] = self.layer.getPara()
        [Pb,Pr,Pc,Pm,Tm,Tn,Tr,Tc] = self.TileInfo
        [DSP_BOUND, BRAM_BOUND, BRAM_SIZE, FREQ, BITWIDTH, BOARD] = self.Board
        [W_p,A_p,I_p,O_p] = self.XferConfig
        OP = self.layer.getOperations()


        if BOARD ==1:
            tW_mem=Tm*Tn*K*K/W_p
            tW_net=0
            tI_mem=Tn*Tr*Tc/I_p
            tI_net=0
            tComp=K*K*Tr*Tc
            tO_mem=Tm*Tr*Tc/O_p

        EachLat = []
        Single_Result = []
        # single FPGA design
        Lat1 = max(tI_mem,tW_mem,tComp)
        Lat2 = max(ceil(N/Tn)*Lat1,tO_mem)
        Lat = B * ceil(R/Tr) * ceil(C/Tc) * ceil(M/Tm) * Lat2 #+ (tO_mem + Lat1)
        TH = float(OP)/(Lat)*FREQ*(10**6)
        EachLat = [tI_mem,tW_mem,tW_net,tComp,tO_mem/ceil(N/Tn)]  

        bI = 2 * Tn * ceil(Tr*Tc*BITWIDTH/BRAM_SIZE)
        bO = 2 * Tm * ceil(Tr*Tc*BITWIDTH/BRAM_SIZE)
        if BITWIDTH ==32:
            bW = 2* Tm * Tn * ceil(K*K*BITWIDTH/BRAM_SIZE) #float
        elif BITWIDTH ==16:  
            bW = Tm * Tn * ceil(4*K*K*BITWIDTH/BRAM_SIZE)  #fix point             
        BRAM = bI+bO+bW
        BRAM_R=BRAM/BRAM_BOUND
        BRAM_R=BRAM
   
        if(BITWIDTH == 16):
            DSP = Tm*Tn                                        
        elif(BITWIDTH==32):
            DSP = 5*Tm*Tn                                        
        else:
            DSP = 0
        DSP_R=DSP/DSP_BOUND
        DSP_R=DSP

        Single_Result = [Lat, TH, DSP_R, BRAM_R]

        if BOARD == 1:
            return Single_Result[0]


