
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xaxidma.h"
#include "xtop.h"
#include <iostream>
#include "AxiTimer.h"

using namespace std;

XAxiDma do_axi_dma[4];
XAxiDma_Config *do_axi_dma_cfg[4];

XTop do_top;
XTop_Config * do_top_cfg;

AxiTimer timer;
double timeInterval;


void init(){
	for(int i=0;i<4;i++){
			switch(i){
			case 0:
				do_axi_dma_cfg[i] = XAxiDma_LookupConfig(XPAR_AXIDMA_3_DEVICE_ID);
				break;
			case 1:
				do_axi_dma_cfg[i] = XAxiDma_LookupConfig(XPAR_AXIDMA_1_DEVICE_ID);
				break;
			case 2:
				do_axi_dma_cfg[i] = XAxiDma_LookupConfig(XPAR_AXIDMA_0_DEVICE_ID);
				break;
			case 3:
				do_axi_dma_cfg[i] = XAxiDma_LookupConfig(XPAR_AXIDMA_2_DEVICE_ID);
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


	do_top_cfg=XTop_LookupConfig(XPAR_TOP_0_DEVICE_ID);
	if(do_top_cfg){
		int status = XTop_CfgInitialize(&do_top,do_top_cfg);
				if (status != XST_SUCCESS){
					cout<<"Error initializing IP"<<endl;
					}
				}

	cout<<"All init done!"<<endl;

	}

//define data structure

	typedef float FPGA_DATA;
	struct DOUBLE{
		FPGA_DATA data1;
		FPGA_DATA data2;
		};


	struct DMA_DATA{
		DOUBLE data;
//		bool last;
		};

	struct DMA_DATA_B{
		FPGA_DATA data;
//		bool last;
		};

	struct AURORA_DATA{
	 DOUBLE data;
		};


	const int Tm=64;
	const int divided_Tm_2=32;
	const int divided_Tm_4=16;
	const int Tn=7;
	const int divided_Tn_2=4;
//	const int Tr=13;
//	const int Tc=13;
//	const int K=3;


//	const int N=192;



	const int M=192;
	const int N=192;
	const int Loop_N=28;	//192/7
	const int R=13;
	const int C=13;
	const int custom_k=3;
	const int custom_Tr=13;
	const int custom_Tc=13;
int main()
{
	cout<<"================================================================="<<endl;
	cout<<"===========================start Test=========================="<<endl;
	cout<<"================================================================="<<endl;

    init_platform();
    init();

    cout<<"haha"<<endl;


    const int W_len= Loop_N*divided_Tm_4*Tn*custom_k*custom_k;
	const int I_len= Loop_N*divided_Tn_2*custom_Tr*custom_Tc;	// floor should be ceil, according to HLS
	const int O_len= divided_Tm_2*custom_Tr*custom_Tc;
	const int B_len= Tm;

	static DMA_DATA Weight[W_len];
	static DMA_DATA IFM[I_len];
	static DMA_DATA OFM[O_len];
	static DMA_DATA_B BIAS[B_len];

//weight
	for(int i=0;i<W_len;i++){
		Weight[i].data.data1=2;
		Weight[i].data.data2=2;
		}
//ifm
	for(int i=0;i<I_len;i++){
		IFM[i].data.data1=1;
		IFM[i].data.data2=1;
	}


//bias
	for(int i=0;i<B_len;i++){
		BIAS[i].data=13;
		}

	DMA_DATA * W_start_addr = Weight;
	DMA_DATA * I_start_addr = IFM;
	DMA_DATA * O_start_addr = OFM;
	DMA_DATA_B * B_start_addr = BIAS;

	int num=0;
	cout<<"Ready to send."<<endl;
//

	int flag=1;
	XTop_Set_flag(&do_top,flag);
	XTop_Start(&do_top);
	while(!XTop_IsDone(&do_top));
	cout<<"Handshaking Done!"<<endl;
	flag=3;

	XTop_Set_N(&do_top,N);
	XTop_Set_custom_k(&do_top,custom_k);
	XTop_Set_custom_Tr(&do_top,custom_Tr);
	XTop_Set_custom_Tc(&do_top,custom_Tc);

	int iter = 0;
	timer.startTimer();
	for(int row=0;row<R*10000;row+=custom_Tr){
		for(int col=0;col<C;col+=custom_Tc){
			for(int to=0;to<M;to+=Tm){
//				for(int ti=0;ti<N;ti+=Tn){
//					cout<<"Iter: "<<iter++<<endl;

					XTop_Set_col(&do_top,col);
					XTop_Set_row(&do_top,row);

					XTop_Set_num(&do_top,num);
					num++;
					XTop_Set_flag(&do_top,flag);


					XTop_Start(&do_top);

//LOAD WEIGHT
//					Xil_DCacheFlushRange((INTPTR)W_start_addr,W_len*sizeof(DMA_DATA));
					XAxiDma_SimpleTransfer(&do_axi_dma[0],(INTPTR)W_start_addr,W_len*sizeof(DMA_DATA),XAXIDMA_DMA_TO_DEVICE);
//LOAD IFM
//					Xil_DCacheFlushRange((INTPTR)I_start_addr,I_len*sizeof(DMA_DATA));
					XAxiDma_SimpleTransfer(&do_axi_dma[1],(INTPTR)I_start_addr,I_len*sizeof(DMA_DATA),XAXIDMA_DMA_TO_DEVICE);
//LOAD BIAS
//					Xil_DCacheFlushRange((INTPTR)B_start_addr,B_len*sizeof(DMA_DATA_B));
					XAxiDma_SimpleTransfer(&do_axi_dma[2],(INTPTR)B_start_addr,B_len*sizeof(DMA_DATA_B),XAXIDMA_DMA_TO_DEVICE);

//					Xil_DCacheFlushRange((INTPTR)O_start_addr,O_len*sizeof(DMA_DATA));
					XAxiDma_SimpleTransfer(&do_axi_dma[3],(INTPTR)O_start_addr,O_len*sizeof(DMA_DATA),XAXIDMA_DEVICE_TO_DMA);


					while(XAxiDma_Busy(&do_axi_dma[2],XAXIDMA_DMA_TO_DEVICE));
//					cout<<"BIAS Done!"<<endl;

					while(XAxiDma_Busy(&do_axi_dma[0],XAXIDMA_DMA_TO_DEVICE));
//					cout<<"WEIGHT Done!"<<endl;
					while(XAxiDma_Busy(&do_axi_dma[1],XAXIDMA_DMA_TO_DEVICE));
//					cout<<"IFM Done!"<<endl;


					while(XAxiDma_Busy(&do_axi_dma[3],XAXIDMA_DEVICE_TO_DMA));
//					cout<<"OFM Done!"<<endl;


					while(!XTop_IsDone(&do_top));
//					cout<<"IP Done!"<<endl;





//					Xil_DCacheInvalidateRange((INTPTR)O_start_addr,O_len*sizeof(DMA_DATA));
//
//
//					//ofm
//					for(int i=0;i<O_len;i++){
//						cout<<O_start_addr[i].data.data1<<" ";
//						cout<<O_start_addr[i].data.data2<<" ";
//						}
//					cout<<"end OFM"<<endl;
//
				}
//				cout<<endl;
		}
	}
	timer.stopTimer();
	timeInterval = timer.getElapsedTimerInSeconds();
	printf("Loading Weights and IFM Elapsed Time (seconds) %f\n",timeInterval);





////	for(int j=0;j<divided_Tm_4;j++){
////		for(int i=0;i<Tn*custom_k*custom_k;i++){
////			flag=0;
////			XTop_Set_flag(&do_top,flag);
////			XTop_Start(&do_top);
////			while(!XTop_IsDone(&do_top));
//////			cout<<"Handshaking Done!"<<endl;
////		}
////		for(int k=0;k<10000;k++){
////			for(int m=0;m<10000;m++){
////				int x = k*19+17-m;
////			}
////		}
////		cout<<"Iter: "<<j<<endl;
////	}
//
//	cout<<"ENdl"<<endl;
//	XTop_Set_col(&do_top,0);
//	XTop_Set_row(&do_top,0);
//
//	XTop_Set_num(&do_top,num);
//	num++;
//	XTop_Set_flag(&do_top,flag);
//
//
//	XTop_Start(&do_top);
//
////LOAD WEIGHT
////					Xil_DCacheFlushRange((INTPTR)W_start_addr,W_len*sizeof(DMA_DATA));
//	XAxiDma_SimpleTransfer(&do_axi_dma[0],(INTPTR)W_start_addr,W_len*sizeof(DMA_DATA),XAXIDMA_DMA_TO_DEVICE);
////LOAD IFM
////					Xil_DCacheFlushRange((INTPTR)I_start_addr,I_len*sizeof(DMA_DATA));
//	XAxiDma_SimpleTransfer(&do_axi_dma[1],(INTPTR)I_start_addr,I_len*sizeof(DMA_DATA),XAXIDMA_DMA_TO_DEVICE);
////LOAD BIAS
////					Xil_DCacheFlushRange((INTPTR)B_start_addr,B_len*sizeof(DMA_DATA_B));
//	XAxiDma_SimpleTransfer(&do_axi_dma[2],(INTPTR)B_start_addr,B_len*sizeof(DMA_DATA_B),XAXIDMA_DMA_TO_DEVICE);
//
////					Xil_DCacheFlushRange((INTPTR)O_start_addr,O_len*sizeof(DMA_DATA));
//	XAxiDma_SimpleTransfer(&do_axi_dma[3],(INTPTR)O_start_addr,O_len*sizeof(DMA_DATA),XAXIDMA_DEVICE_TO_DMA);
//
//
//	while(XAxiDma_Busy(&do_axi_dma[2],XAXIDMA_DMA_TO_DEVICE));
//	cout<<"BIAS Done!"<<endl;
//
//	while(XAxiDma_Busy(&do_axi_dma[3],XAXIDMA_DEVICE_TO_DMA));
//	cout<<"OFM Done!"<<endl;
//
//	while(XAxiDma_Busy(&do_axi_dma[1],XAXIDMA_DMA_TO_DEVICE));
//	cout<<"IFM Done!"<<endl;
//
//	while(XAxiDma_Busy(&do_axi_dma[0],XAXIDMA_DMA_TO_DEVICE));
//	cout<<"WEIGHT Done!"<<endl;
//
//
//	while(!XTop_IsDone(&do_top));
//	cout<<"IP Done!"<<endl;



	cleanup_platform();
	return 0;
}





