#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
using namespace std;

int main(){
	//sampling added
	int stage =5;
	double weight1 =0.175;
	double weight2=0.815;
	int P_adjust=1294;
	int bandwdith = 4;
	int Power_local1[]={2066,2872, 2564,2468,2303};//mW


	int Power_wifi=1860;
	int time_local1[] = {18163,15288,21584,65463350,112};//us


	int time_pc1[] ={18376, 1441,9751,17423864,19};
	
	//int tranmiss_size[]={1280*720*3*5*3, 1280*720*3*5, 1280*720*5, 1280*720*5, 1280*720*5+ 2, 1280*720*5+ 3};
	//int tranmiss_size[]={1280*720*3*5, 1280*720*5, 1280*720*5, 1280*720*5+ 32, 1280*720*5+ 32+56};
	int tranmiss_size[]={640*480*3*5, 640*480*5, 640*480*5, 640*480*5+32, 640*480*5+32+56};
	
	int sum_localtime =0;
	int sum_pctime=0;
	for(int i=0;i<stage; i++)
	{
		sum_pctime = sum_pctime +time_pc1[i];
		//sum_pctime = sum_pctime +time_pc2[i];
		//sum_pctime = sum_pctime +time_pc3[i];
	}
	sum_pctime = sum_pctime/1000;//ms
	double current_value = 0.0;
	for(int i=0;i<stage;i++)
	{
		sum_localtime = sum_localtime + time_local1[i]/1000;//ms
		//sum_localtime = sum_localtime + time_local2[i]/1000;//ms
		//sum_localtime = sum_localtime + time_local3[i]/1000;//ms
		
		sum_pctime = sum_pctime - time_pc1[i]/1000;//ms
		//sum_pctime = sum_pctime - time_pc2[i]/1000;//ms
		//sum_pctime = sum_pctime - time_pc3[i]/1000;//ms
		int trans= tranmiss_size[i]/(bandwdith*1000);//ms
		printf("transmission time for i= %d: %d\n", i, trans);
		if((sum_localtime + trans)> sum_pctime)
		{
			current_value =  current_value + (double)Power_local1[i]*time_local1[i]*weight1/1000;
			//current_value =  current_value + (double)Power_local2[i]*time_local2[i]*weight1/1000;
			//current_value =  current_value + (double)Power_local3[i]*time_local3[i]*weight1/1000;
			double min = current_value + Power_wifi*trans*weight1 + (sum_localtime+ trans)*P_adjust*weight2;//ms * mW
			printf("i: %d, More local time: %.0f, cost function: %.0f \n", i, current_value, min);
		}
		else
		{
			current_value =  current_value + (double)Power_local1[i]*time_local1[i]*weight1/1000;
			//current_value =  current_value + (double)Power_local2[i]*time_local2[i]*weight1/1000;
			//current_value =  current_value + (double)Power_local3[i]*time_local3[i]*weight1/1000;
			double min = current_value + Power_wifi*trans*weight1 + sum_pctime*P_adjust*weight2;
			printf("i: %d, More PC time: %.0f, cost function: %.0f \n", i,current_value,  min);
		}
	}
	
	
	
}