#-*- coding: UTF-8 -*-


import SuperLinearBackend
import SuperLinearConfig as config


ConvLayer = SuperLinearBackend.Network(config.B,config.M,config.N,config.R,config.C,config.K,config.S,config.P)
tileInfo = [config.Pb, config.Pr, config.Pc, config.Pm, config.Tm, config.Tn, config.Tr, config.Tc,]
Borad = [config.DSP_BOUND, config.BRAM_BOUND, config.BRAM_SIZE, config.FREQ, config.BITWIDTH, config.BOARD]
XferConfig = [config.W_p, config.A_p, config.I_p, config.O_p]

Predictor = SuperLinearBackend.Performance(ConvLayer, tileInfo, Borad, XferConfig)
layerPerformance = Predictor.layerPerformance()
print('The cycle for this convolution layer is: ', layerPerformance, 'cycles')