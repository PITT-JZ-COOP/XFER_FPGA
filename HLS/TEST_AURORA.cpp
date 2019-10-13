#include "source.h"


void LOAD_WEIGHT_DMA(hls::stream<DMA_DATA> &input_dma_W,
					 hls::stream<AURORA_DATA> &output_aurora_W,
					 FPGA_DATA WEIGHT1[divided_Tm_2][Tn][K][K],
					 int custom_k){

	DMA_DATA weight_input_dma;
	AURORA_DATA weight_output_aurora;

	for(int i=0;i<divided_Tm_4;i++){
		for(int j=0;j<Tn;j++){
			for(int m=0;m<custom_k;m++){
				for(int l=0;l<custom_k;l++){
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=WEIGHT1 intra false
					weight_input_dma=input_dma_W.read();
					WEIGHT1[i][j][m][l]=weight_input_dma.data.data1;
					if(i+divided_Tm_4<divided_Tm_2)
						WEIGHT1[i+divided_Tm_4][j][m][l]=weight_input_dma.data.data2;
					weight_output_aurora.data.data1=weight_input_dma.data.data1;
					weight_output_aurora.data.data2=weight_input_dma.data.data2;
					output_aurora_W.write(weight_output_aurora);
				}
			}
		}
	}
}

void LOAD_WEIGHT_AURORA(hls::stream<AURORA_DATA> &input_aurora_W,
						FPGA_DATA WEIGHT2[divided_Tm_2][Tn][K][K],
						int custom_k){

	AURORA_DATA weight_output_aurora;
	for(int i=0;i<divided_Tm_4;i++){
		for(int j=0;j<Tn;j++){
			for(int m=0;m<custom_k;m++){
				for(int l=0;l<custom_k;l++){
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=WEIGHT2 intra false
					weight_output_aurora=input_aurora_W.read();
					WEIGHT2[i][j][m][l]=weight_output_aurora.data.data1;
					if(i+divided_Tm_4<divided_Tm_2)
						WEIGHT2[i+divided_Tm_4][j][m][l]=weight_output_aurora.data.data2;
				}
			}
		}
	}
}


void LOAD_IFM(hls::stream<DMA_DATA> &input_dma_I,
		FPGA_DATA (* IFM)[Tr][Tc],
		int custom_Tr,
		int custom_Tc){


	DMA_DATA ifm_input_dma;
	for(int i=0;i<divided_Tn_2;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=IFM intra false
				ifm_input_dma=input_dma_I.read();
				IFM[i][j][m]=ifm_input_dma.data.data1;
				if(i+divided_Tn_2<Tn)
					IFM[i+divided_Tn_2][j][m]=ifm_input_dma.data.data2;
			}
		}
	}
}




void FIRE(  FPGA_DATA WEIGHT1[divided_Tm_2][Tn][K][K],
			FPGA_DATA WEIGHT2[divided_Tm_2][Tn][K][K],
			FPGA_DATA (* IFM)[Tr][Tc],
			FPGA_DATA OFM[Tm][Tr][Tc],
			int row,
			int col,
			int custom_k,
			int custom_Tr,
			int custom_Tc){


	for(int i=0;i<custom_k;i++){
		for(int j=0;j<custom_k;j++){
			for(int trr=row;(trr<row+custom_Tr);trr++){
#pragma HLS loop_tripcount min=1 max=Tr avg=Tr
				for(int tcc=col;(tcc<col+custom_Tc);tcc++){
#pragma HLS loop_tripcount min=1 max=Tc avg=Tc
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=OFM inter false
					for(int too=0;too<Tm; too++){
						for(int tii=0;tii<Tn;tii++){

							FPGA_DATA add_res1;
							FPGA_DATA add_res2;
							if (too<divided_Tm_2){
								add_res1 = WEIGHT1[too][tii][i][j]*IFM[tii][trr-row][tcc-col];
								OFM[too][trr-row][tcc-col] = OFM[too][trr-row][tcc-col] + add_res1;
							}else
							{
								add_res2 = WEIGHT2[too-divided_Tm_2][tii][i][j]*IFM[tii][trr-row][tcc-col];
								OFM[too][trr-row][tcc-col] = OFM[too][trr-row][tcc-col] + add_res2;
							}
						}
					}
				}
			}
		}
	}
}

void OFM_STORE( hls::stream<DMA_DATA> &output_dma_O,
		hls::stream<DMA_DATA_B> &input_dma_B,
		FPGA_DATA OFM[Tm][Tr][Tc],
		FPGA_DATA BIAS[Tm],
		int custom_Tr,
		int custom_Tc){

	DMA_DATA output_dma_O_data;
	DMA_DATA_B bias_input_dma;

	for(int i=0;i<Tm;i++){
#pragma HLS PIPELINE II=1
		bias_input_dma=input_dma_B.read();
		BIAS[i]=bias_input_dma.data;
	}

	for(int i=0;i<divided_Tm_2;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int k=0;k<custom_Tc;k++){
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=OFM intra false
				output_dma_O_data.last=0;
				if (i==divided_Tm_2-1 && j==custom_Tr-1 && k==custom_Tc-1){
					output_dma_O_data.last=1;
					}
				output_dma_O_data.data.data1=OFM[i][j][k]+BIAS[i];
				output_dma_O_data.data.data2=OFM[i+divided_Tm_2][j][k]+BIAS[i];
				output_dma_O.write(output_dma_O_data);
			}
		}
	}
	for(int j=0;j<custom_Tr;j++){
		for(int k=0;k<custom_Tc;k++){
#pragma HLS PIPELINE II=1
			for(int i=0;i<Tm;i++){
				OFM[i][j][k] = 0;
			}
		}
	}
}





void Load_Fire(hls::stream<DMA_DATA> &input_dma_W,
		hls::stream<DMA_DATA> &input_dma_I,
		hls::stream<AURORA_DATA> &output_aurora_W,
		hls::stream<AURORA_DATA> &input_aurora_W,
		FPGA_DATA WEIGHT1[divided_Tm_2][Tn][K][K],
		FPGA_DATA WEIGHT1_DB[divided_Tm_2][Tn][K][K],
		FPGA_DATA WEIGHT2[divided_Tm_2][Tn][K][K],
		FPGA_DATA WEIGHT2_DB[divided_Tm_2][Tn][K][K],
		FPGA_DATA IFM[Tn][Tr][Tc],
		FPGA_DATA IFM_DB[Tn][Tr][Tc],
		FPGA_DATA OFM[Tm][Tr][Tc],
		int row,
		int col,
		int N,
		int custom_k,
		int custom_Tr,
		int custom_Tc
		){

	for(int i=0;i<N;i+=Tn){
		if(i%2==0){
			LOAD_WEIGHT_DMA(input_dma_W,output_aurora_W,WEIGHT1,custom_k);
			LOAD_WEIGHT_AURORA(input_aurora_W,WEIGHT2,custom_k);
			LOAD_IFM(input_dma_I,IFM,custom_Tr,custom_Tc);
			FIRE(WEIGHT1_DB,WEIGHT2_DB,IFM_DB,OFM, row, col,custom_k,custom_Tr,custom_Tc);
		}else{
			LOAD_WEIGHT_DMA(input_dma_W,output_aurora_W,WEIGHT1_DB,custom_k);
			LOAD_WEIGHT_AURORA(input_aurora_W,WEIGHT2_DB,custom_k);
			LOAD_IFM(input_dma_I,IFM_DB,custom_Tr,custom_Tc);
			FIRE(WEIGHT1,WEIGHT2,IFM,OFM, row, col,custom_k,custom_Tr,custom_Tc);
		}
	}
}


void top(hls::stream<DMA_DATA> &input_dma_W,
			hls::stream<DMA_DATA> &input_dma_I,
			hls::stream<DMA_DATA_B> &input_dma_B,
			hls::stream<DMA_DATA> &output_dma_O,
			hls::stream<AURORA_DATA> &output_aurora_W,
			hls::stream<AURORA_DATA> &input_aurora_W,
			int row,
			int col,
			int flag,
			int num,
			int N,
			int custom_k,
			int custom_Tr,
			int custom_Tc
			){

#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=row bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=col bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=flag bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=num bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=N bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_k bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_Tr bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_Tc bundle=CRTL_BUS

#pragma HLS INTERFACE axis port=input_dma_W
#pragma HLS INTERFACE axis port=input_dma_I
#pragma HLS INTERFACE axis port=input_dma_B
#pragma HLS INTERFACE axis port=output_dma_O

#pragma HLS INTERFACE axis port=output_aurora_W
#pragma HLS INTERFACE axis port=input_aurora_W

	static FPGA_DATA WEIGHT1[divided_Tm_2][Tn][K][K];
#pragma HLS RESOURCE variable=WEIGHT1 core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=WEIGHT1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=WEIGHT1 complete dim=2

	static FPGA_DATA WEIGHT1_DB[divided_Tm_2][Tn][K][K];
#pragma HLS RESOURCE variable=WEIGHT1_DB core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=WEIGHT1_DB complete dim=1
#pragma HLS ARRAY_PARTITION variable=WEIGHT1_DB complete dim=2

	static FPGA_DATA WEIGHT2[divided_Tm_2][Tn][K][K];
#pragma HLS RESOURCE variable=WEIGHT2 core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=WEIGHT2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=WEIGHT2 complete dim=2

	static FPGA_DATA WEIGHT2_DB[divided_Tm_2][Tn][K][K];
#pragma HLS RESOURCE variable=WEIGHT2_DB core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=WEIGHT2_DB complete dim=1
#pragma HLS ARRAY_PARTITION variable=WEIGHT2_DB complete dim=2

	static FPGA_DATA IFM[Tn][Tr][Tc];
//#pragma HLS RESOURCE variable=IFM core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=IFM complete dim=1

	static FPGA_DATA IFM_DB[Tn][Tr][Tc];
//#pragma HLS RESOURCE variable=IFM core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=IFM_DB complete dim=1

	static FPGA_DATA OFM[Tm][Tr][Tc];
#pragma HLS RESOURCE variable=OFM core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=OFM complete dim=1

	static FPGA_DATA OFM_DB[Tm][Tr][Tc];
#pragma HLS RESOURCE variable=OFM_DB core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=OFM_DB complete dim=1

	static FPGA_DATA BIAS[Tm];
	static FPGA_DATA BIAS_DB[Tm];

	AURORA_DATA output_aurora_W_initial;

	if (flag==0){//Board start second
		output_aurora_W_initial.data.data1=233333333;
		output_aurora_W_initial.data.data2=233333333;
		output_aurora_W.write(output_aurora_W_initial);
	}else if (flag==1){ //Board start first
		output_aurora_W_initial=input_aurora_W.read();
	}else if (flag==3){
		if(num%2==0){
			Load_Fire(input_dma_W,input_dma_I,output_aurora_W,input_aurora_W,WEIGHT1,WEIGHT1_DB,WEIGHT2,WEIGHT2_DB,IFM,IFM_DB,OFM,row,col,N,custom_k,custom_Tr,custom_Tc);
			OFM_STORE(output_dma_O,input_dma_B, OFM_DB, BIAS_DB,custom_Tr,custom_Tc);
		}else{
			Load_Fire(input_dma_W,input_dma_I,output_aurora_W,input_aurora_W,WEIGHT1,WEIGHT1_DB,WEIGHT2,WEIGHT2_DB,IFM,IFM_DB,OFM_DB,row,col,N,custom_k,custom_Tr,custom_Tc);
			OFM_STORE(output_dma_O,input_dma_B, OFM, BIAS,custom_Tr,custom_Tc);
		}
	}
}







