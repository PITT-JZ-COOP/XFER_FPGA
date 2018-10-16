/*
 * Author: Xinyi Zhang and Weiwen Jiang
 * Affiliation: University of Pittsburgh
 *
 */

#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <ap_fixed.h>
#include <ap_int.h>

#define FLOAT 1

const int Tm=8;
const int divided_Tm_2=4;
const int divided_Tm_4=2;
const int Tn=32;
const int divided_Tn_2=16;
const int Tr=13;
const int Tc=13;
const int K=11;
const int R=13;
const int C=13;
const int M=128;
//const int N=192;

//const int M=32;



#if FLOAT==1
	typedef float FPGA_DATA;
	typedef float FPGA_WEIGHTS;
#else
	typedef ap_fixed<32,10,AP_TRN_ZERO, AP_SAT> FPGA_DATA;
	typedef ap_fixed<32,1,AP_TRN_ZERO, AP_SAT> FPGA_WEIGHTS;
#endif



struct DOUBLE{
	FPGA_DATA data1;
	FPGA_DATA data2;
};


struct DMA_DATA{
	DOUBLE data;
	bool last;
};

struct DMA_DATA_B{
	FPGA_DATA data;
	bool last;
};

struct AURORA_DATA{
 DOUBLE data;
};

