/*
 * Author: Xinyi Zhang and Weiwen Jiang
 * Affiliation: University of Pittsburgh
 *
 */

#include "source.h"

void LOAD_WEIGHT_DMA(hls::stream<DMA_DATA> &input_dma_W,
					 FPGA_DATA WEIGHT1[Tm][Tn][K][K],
					 int custom_k){

	DMA_DATA weight_input_dma;

	for(int i=0;i<divided_Tm_2;i++){
		for(int j=0;j<Tn;j++){
			for(int m=0;m<custom_k;m++){
#pragma HLS loop_tripcount min=3 max=3 avg=3
				for(int l=0;l<custom_k;l++){
#pragma HLS loop_tripcount min=3 max=3 avg=3
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=WEIGHT1 intra false

					weight_input_dma=input_dma_W.read();
					WEIGHT1[i][j][m][l]=weight_input_dma.data.data1;

					if(i+divided_Tm_2<Tm)
						WEIGHT1[i+divided_Tm_2][j][m][l]=weight_input_dma.data.data2;
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
#pragma HLS loop_tripcount min=13 max=13 avg=13
			for(int m=0;m<custom_Tc;m++){
#pragma HLS loop_tripcount min=13 max=13 avg=13
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




void FIRE(  FPGA_DATA WEIGHT1[Tm][Tn][K][K],
			FPGA_DATA (* IFM)[Tr][Tc],
			FPGA_DATA OFM[Tm][Tr][Tc],
			int row,
			int col,
			int custom_k,
			int custom_Tr,
			int custom_Tc){


	for(int i=0;i<custom_k;i++){
#pragma HLS loop_tripcount min=3 max=3 avg=3
		for(int j=0;j<custom_k;j++){
#pragma HLS loop_tripcount min=3 max=3 avg=3
			for(int trr=row;(trr<row+custom_Tr);trr++){
#pragma HLS loop_tripcount min=13 max=13 avg=13
				for(int tcc=col;(tcc<col+custom_Tc);tcc++){
#pragma HLS loop_tripcount min=13 max=13 avg=13
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=OFM inter false
					for(int too=0;too<Tm; too++){
						for(int tii=0;tii<Tn;tii++){

							FPGA_DATA add_res1;

							add_res1 = WEIGHT1[too][tii][i][j]*IFM[tii][trr-row][tcc-col];
							OFM[too][trr-row][tcc-col] = OFM[too][trr-row][tcc-col] + add_res1;
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
#pragma HLS loop_tripcount min=13 max=13 avg=13
			for(int k=0;k<custom_Tc;k++){
#pragma HLS loop_tripcount min=13 max=13 avg=13
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
#pragma HLS loop_tripcount min=13 max=13 avg=13
		for(int k=0;k<custom_Tc;k++){
#pragma HLS loop_tripcount min=13 max=13 avg=13
#pragma HLS PIPELINE II=1
			for(int i=0;i<Tm;i++){
				OFM[i][j][k] = 0;
			}
		}
	}
}





void Load_Fire( hls::stream<DMA_DATA> &input_dma_W,
				hls::stream<DMA_DATA> &input_dma_I,
				FPGA_DATA WEIGHT1[Tm][Tn][K][K],
				FPGA_DATA WEIGHT1_DB[Tm][Tn][K][K],
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
	int idx = 0;
	for(int i=0;i<N;i+=Tn){
#pragma HLS loop_tripcount min=6 max=6 avg=6
		if(idx%2==0){
			LOAD_WEIGHT_DMA(input_dma_W,WEIGHT1,custom_k);
			LOAD_IFM(input_dma_I,IFM,custom_Tr,custom_Tc);
			FIRE(WEIGHT1_DB,IFM_DB,OFM, row, col,custom_k,custom_Tr,custom_Tc);
		}else{
			LOAD_WEIGHT_DMA(input_dma_W,WEIGHT1_DB,custom_k);
			LOAD_IFM(input_dma_I,IFM_DB,custom_Tr,custom_Tc);
			FIRE(WEIGHT1,IFM,OFM, row, col,custom_k,custom_Tr,custom_Tc);
		}
		idx+=1;
	}
}


void top(hls::stream<DMA_DATA> &input_dma_W,
			hls::stream<DMA_DATA> &input_dma_I,
			hls::stream<DMA_DATA_B> &input_dma_B,
			hls::stream<DMA_DATA> &output_dma_O,
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


	static FPGA_DATA WEIGHT1[Tm][Tn][K][K];
#pragma HLS RESOURCE variable=WEIGHT1 core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=WEIGHT1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=WEIGHT1 complete dim=2

	static FPGA_DATA WEIGHT1_DB[Tm][Tn][K][K];
#pragma HLS RESOURCE variable=WEIGHT1_DB core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=WEIGHT1_DB complete dim=1
#pragma HLS ARRAY_PARTITION variable=WEIGHT1_DB complete dim=2


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


		if(num%2==0){
			Load_Fire(input_dma_W,input_dma_I,WEIGHT1,WEIGHT1_DB,IFM,IFM_DB,OFM,row,col,N,custom_k,custom_Tr,custom_Tc);
			OFM_STORE(output_dma_O,input_dma_B, OFM_DB, BIAS_DB,custom_Tr,custom_Tc);
		}else{
			Load_Fire(input_dma_W,input_dma_I,WEIGHT1,WEIGHT1_DB,IFM,IFM_DB,OFM_DB,row,col,N,custom_k,custom_Tr,custom_Tc);
			OFM_STORE(output_dma_O,input_dma_B, OFM, BIAS,custom_Tr,custom_Tc);
		}


}







