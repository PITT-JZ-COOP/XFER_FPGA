/*
 * Author: Xinyi Zhang and Weiwen Jiang
 * Affiliation: University of Pittsburgh
 *
 */
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xaxidma.h"
#include "xtop.h"

#include "AxiTimer.h"



#include <iostream>


using namespace std;

XAxiDma do_axi_dma[4];
XAxiDma_Config *do_axi_dma_cfg[4];


XTop do_top_simple;
XTop_Config * do_top_simple_cfg;

//
//AxiTimer timer;
//double timeInterval;


//define data structure

typedef float FPGA_DATA;
typedef short FPGA_DATA_16;

struct TWO{
	FPGA_DATA data1;
	FPGA_DATA data2;
};

struct ONE{
	FPGA_DATA data1;
};

struct DMA_DATA_64B_32{
	TWO data;
};

struct DMA_DATA_32B_32{
	ONE data;
};

//same as HLS
//const int Tm=128;
//const int half_Tm=64;
//const int quart_Tm=32;
//const int divided_Tm_8=16;
//const int divided_half_Tm_8=8;
//const int divided_quart_Tm_8=4;
//const int Tn=10;
//const int divided_Tn_8=2;
//const int Tn_8=divided_Tn_8*8;

//exp1
const int Tm=8;
const int divided_Tm_2=4;
const int divided_Tm_4=2;
const int Tn=32;
const int divided_Tn_2=16;
const int Tr=13;
const int Tc=13;

//need to be modified, L5
const int M=128;
const int N=192;
const int loop_N=6;
const int R=13;
const int C=13;
const int custom_k=3;
const int custom_Tr=13;
const int custom_Tc=13;



const int fix_W_len= loop_N*divided_Tm_2*Tn*custom_k*custom_k;
const int fix_I_len= loop_N*divided_Tn_2*custom_Tr*custom_Tc;
const int fix_O_len= divided_Tm_2*custom_Tr*custom_Tc;
const int fix_B_len= Tm;

static DMA_DATA_64B_32 fix_WEIGHT[fix_W_len];
static DMA_DATA_64B_32 fix_IFM[fix_I_len];
static DMA_DATA_64B_32 fix_OFM[fix_O_len];
static DMA_DATA_32B_32 fix_BIAS[fix_B_len];


DMA_DATA_64B_32 * fix_W_start_addr = fix_WEIGHT;
DMA_DATA_64B_32 * fix_I_start_addr = fix_IFM;
DMA_DATA_64B_32 * fix_O_start_addr = fix_OFM;
DMA_DATA_32B_32 * fix_B_start_addr = fix_BIAS;

void init(){



	for(int i=0;i<4;i++){
			switch(i){
			case 0:
				do_axi_dma_cfg[i] = XAxiDma_LookupConfig(XPAR_AXIDMA_0_DEVICE_ID); //AXI_DMA_B
				break;
			case 1:
				do_axi_dma_cfg[i] = XAxiDma_LookupConfig(XPAR_AXIDMA_1_DEVICE_ID); //AXI_DMA_I
				break;
			case 2:
				do_axi_dma_cfg[i] = XAxiDma_LookupConfig(XPAR_AXIDMA_2_DEVICE_ID); //AXI_DMA_O
				break;
			case 3:
				do_axi_dma_cfg[i] = XAxiDma_LookupConfig(XPAR_AXIDMA_3_DEVICE_ID); //AXI_DMA_W
				break;
			default:
				break;
			}

			if(do_axi_dma_cfg[i]){
				int status = XAxiDma_CfgInitialize(&do_axi_dma[i],do_axi_dma_cfg[i]);
				if (status != XST_SUCCESS){
					cout<<"Error initializing AxiDMA, ID is "<<i<<endl;
					}
				XAxiDma_IntrDisable(&do_axi_dma[i],XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);
				XAxiDma_IntrDisable(&do_axi_dma[i],XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
				}
			}


	do_top_simple_cfg=XTop_LookupConfig(XPAR_TOP_0_DEVICE_ID);
	if(do_top_simple_cfg){
		int status = XTop_CfgInitialize(&do_top_simple,do_top_simple_cfg);
				if (status != XST_SUCCESS){
					cout<<"Error initializing IP"<<endl;
					}
				}

	cout<<"All init done!"<<endl;

	}








int main()
{
	cout<<"================================================================="<<endl;
	cout<<"===========================start Test=========================="<<endl;
	cout<<"================================================================="<<endl;

    init_platform();
    init();
//

    AxiTimer timer;
    double timeInterval;



//weight
	for(int i=0;i<fix_W_len;i++){
		fix_WEIGHT[i].data.data1=222;
		fix_WEIGHT[i].data.data2=555;
		}

	for(int i=0;i<fix_I_len;i++){
		fix_IFM[i].data.data1=1;
		fix_IFM[i].data.data2=1;
		}

	for(int i=0;i<fix_B_len;i++){
		fix_BIAS[i].data.data1=222;
		}

	cout<<"Ready to send."<<endl;


	//shake hands

	int num=0;

	XTop_Set_custom_k(&do_top_simple,custom_k);
	XTop_Set_custom_Tr(&do_top_simple,custom_Tr);
	XTop_Set_custom_Tc(&do_top_simple,custom_Tc);


	timer.startTimer();

	for(int row=0;row<R;row+=custom_Tr){
		for(int col=0;col<C;col+=custom_Tc){
			for(int to=0;to<M;to+=Tm){


				XTop_Set_row(&do_top_simple,row);
				XTop_Set_col(&do_top_simple,col);
				XTop_Set_num(&do_top_simple,num);
				XTop_Set_N(&do_top_simple,N);
				num++;
				XTop_Start(&do_top_simple);

//				cout<<"start"<<endl;

//				Xil_DCacheFlushRange((INTPTR)fix_W_start_addr,fix_W_len*sizeof(DMA_DATA_128B_16));
				XAxiDma_SimpleTransfer(&do_axi_dma[3],(INTPTR)fix_W_start_addr,fix_W_len*sizeof(DMA_DATA_64B_32),XAXIDMA_DMA_TO_DEVICE);



//				Xil_DCacheFlushRange((INTPTR)fix_I_start_addr,fix_I_len*sizeof(DMA_DATA_128B_16));
				XAxiDma_SimpleTransfer(&do_axi_dma[1],(INTPTR)fix_I_start_addr,fix_I_len*sizeof(DMA_DATA_64B_32),XAXIDMA_DMA_TO_DEVICE);



//				Xil_DCacheFlushRange((INTPTR)fix_B_start_addr,fix_B_len*sizeof(DMA_DATA_128B_16));
				XAxiDma_SimpleTransfer(&do_axi_dma[0],(INTPTR)fix_B_start_addr,fix_B_len*sizeof(DMA_DATA_32B_32),XAXIDMA_DMA_TO_DEVICE);


//				Xil_DCacheFlushRange((INTPTR)fix_O_start_addr,fix_O_len*sizeof(DMA_DATA_128B_16));
				XAxiDma_SimpleTransfer(&do_axi_dma[2],(INTPTR)fix_O_start_addr,fix_O_len*sizeof(DMA_DATA_64B_32),XAXIDMA_DEVICE_TO_DMA);



				while(XAxiDma_Busy(&do_axi_dma[0],XAXIDMA_DMA_TO_DEVICE));
//				cout<<"BIAS in Done!"<<endl;
				while(XAxiDma_Busy(&do_axi_dma[1],XAXIDMA_DMA_TO_DEVICE));
//				cout<<"IFM in Done!"<<endl;
				while(XAxiDma_Busy(&do_axi_dma[3],XAXIDMA_DMA_TO_DEVICE));
//				cout<<"WEIGHT in Done!"<<endl;


				while(XAxiDma_Busy(&do_axi_dma[2],XAXIDMA_DEVICE_TO_DMA));
//				cout<<"OFM in Done!"<<endl;

				while(!XTop_IsDone(&do_top_simple));
//				cout<<"IP done"<<endl;

//				Xil_DCacheInvalidateRange((INTPTR)fix_O_start_addr,fix_O_len*sizeof(DMA_DATA_128B_16));
//
//				for(int i=0;i<fix_O_len;i++){
//					cout<<fix_O_start_addr[i].data.data1<<" ";
//					cout<<fix_O_start_addr[i].data.data2<<" ";
//					cout<<fix_O_start_addr[i].data.data3<<" ";
//					cout<<fix_O_start_addr[i].data.data4<<" ";
//					}

//				cout<<"end OFM"<<endl;

			}
		}
	}
	timer.stopTimer();
	timeInterval = timer.getElapsedTimerInSeconds();
	printf("Loading Weights and IFM Elapsed Time (seconds) %f\n",timeInterval);


//	for(int i=0;i<fix_W_len;i++){
//		int flag=0;
//		int num=0;
//		XTop_db_128b_fix_our_debug_Set_flag(&do_top_simple,flag);
//		XTop_db_128b_fix_our_debug_Start(&do_top_simple);
////		cout<<"waiting for shaking hand"<<endl;
//	}



	cleanup_platform();
	return 0;
}






