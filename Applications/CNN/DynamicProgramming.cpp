#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
using namespace std;

double weight1 =0.5;
double weight2=0.5;
int Power_wifi=1860;
int bandwidth1 = 4;
int bandwidth2= 12;

//ms
//first layer, i always 0; last layer, j always 7
int Sumlocaltime(int time_local[], int i, int j,int tranmission,int bandwidth)
{
	int localtime=0;
	for(int w1=i;w1<=j;w1++)
		localtime += time_local[w1];
	localtime = localtime/1000 + tranmission/(bandwidth*1000); //ms
	return localtime;
}

//first layer, i is always 0
int SumEnergy(int Power_local[], int time_local[], int i, int j,int tranmission, int bandwidth)
{
	int localenergy =0;
	int used_time=0;
	for(int w1=i;w1<=j;w1++)
	{
		used_time = time_local[w1]/1000; //ms
		localenergy  = localenergy + Power_local[w1]*used_time;
		//printf("localenergy: %d, %d, %d\n", localenergy, Power_local[w1], time_local[w1]);
	}
	int transmiss_time = tranmission/(bandwidth*1000); // ms
	localenergy  = localenergy + Power_wifi* transmiss_time;
	//printf("transmiss_time, %d, transmiisson: %d, localenergy: %d\n", transmiss_time, Power_wifi* transmiss_time, localenergy);
	return localenergy;
}


int Minimum_value(int a, int b)
{
	if(a<b)
		return a;
	return b;
}

int get_max(int a, int b)
{
	if(a>b)
		return a;
	return b;
}
int main(){
	int stage =9;
	int P_adjust=1294;
	int Power_local1[]={2846, 1570,3238,1596,3920,1495,2068,1495};//mW
	int first_power= 2099;


	
	//int time_local[2][9];
	int first_time= 363147;
	int first_time2=first_time/1000;//ms
	int time_local1[] = {662857,45781,731369,64555,2746728,16794,254771,14981};//us
	int time_local2[] = {662857,45781,731369,64555,2746728,16794,254771,14981};//us
	
	int first_timepc= 476869;
	int time_pc1[] ={73404,6385,242302,43050,612809,11544,45161,6762};
	
	int tranmiss_size[]={640*480*100, 100*32*32*sizeof(double), 100*sizeof(double)*16*28*28, 100*sizeof(double)*16*14*14, 100*sizeof(double)*32*10*10, 100*sizeof(double)*32*5*5, 100*sizeof(double)*256, 100*sizeof(int)};
	//int tranmiss_size1[]={640*480*100, 100*32*32*sizeof(double), 100*sizeof(double)*16*28*28, 100*sizeof(double)*16*14*14, 100*sizeof(double)*32*10*10, 100*sizeof(double)*32*5*5, 100*sizeof(double)*256, 100*sizeof(int)};
	int first_transmiss=640*480*3*100;
	
	
	int Tcost[8][8];//m*m (middle layer)
	int FirstTcost[8];//Tcost for first layer
	int LastTcost[8];// Tcost for last layer
	int Ecost[8];//m
	int ConstantTCost; //constant Time cost
	int ConstantECost;// constant energy cost
	
	//-----------------------------
	ConstantTCost = P_adjust * first_time2  * weight2;
	ConstantECost = first_power * first_time2 * weight1;
	int num_services = 9-1;
	
	for(int i=0;i<8;i++)
		for(int j=i;j<8;j++)
		{
			//second layer
			Tcost[i][j] =  Sumlocaltime(time_local2, i,j,tranmiss_size[j],bandwidth2) * P_adjust * weight2;
		}
	//first layer from 0 to ww : Tcost (extreme device)
	for(int ww=0;ww <8; ww++)
	{
		FirstTcost[ww]= Sumlocaltime(time_local1, 0,ww,tranmiss_size[ww],bandwidth1) * P_adjust * weight2;
		Ecost[ww] = SumEnergy(Power_local1,time_local1,0,ww,tranmiss_size[ww],bandwidth1) * weight1;//computation power + transmission power
		//printf("Ecost:%d\n", Ecost[ww]);
	}
	//last layer from ww to 7: Tcost (PC)
	for(int ww=0;ww <8; ww++)
	{
		LastTcost[ww]= Sumlocaltime(time_pc1, ww, 7,0,bandwidth2) * P_adjust * weight2;
	}
	int cost[8][3];//m==8, n==3
	int cut[8][3];
	for(int i=0;i<8;i++)
		for(int k=0;k<3;k++)
		{
			cost[i][k]=-1;
			cut[i][k]=-1;
		}
	
	for(int i=7;i>-1;i--)
		for(int k=0;k<3;k++)
		{
			int min_value=1000000000;
			//last one
			if(k==0)
			{
				cost[i][k] = LastTcost[i];
				cut[i][k] = 7;
				//cost[i][k]=  Tcost[2][i][7];
			}
			//last two
			else if(k == 1)
			{
				//int min_value=1000000000;
				int temp_value;
				int j=i;
				if(i==0)
				{
					int another = first_transmiss/(bandwidth2*1000);//ms
					int another_cost = P_adjust* another * weight2;
					min_value= get_max(cost[j][k-1],another_cost) ;
				}
				else
				{
					int tranmiss_time = tranmiss_size[i-1]/(bandwidth2*1000);
					int tran_cost = P_adjust * tranmiss_time * weight2;
					min_value= get_max(cost[j][k-1], tran_cost) ;
					//printf("hhhhh%d, %d,%d,%d,%d\n", i,k,tranmiss_time,min_value,tran_cost);
					
				}
				cut[i][k] = j-1;
				for(j=i+1;j<=num_services;j++)
				{
					if(j==num_services)
						temp_value = Tcost[i][j-1] ;
					else
						temp_value= get_max(cost[j][k-1],Tcost[i][j-1]);
					if(temp_value< min_value)
					{	
						min_value = temp_value;
						cut[i][k] = j-1;
					}
				}
				cost[i][k] = min_value; 
			}
			//last three layers
			else if(k == 2&& i ==0)
			{
				//int min_value;
				int temp_value;
				int j=i;
				if(i==0)
				{
					int another2 = first_transmiss/(bandwidth1*1000); //ms
					int another2_cost = P_adjust * another2 * weight2;
					int transEcost = Power_wifi * another2* weight1;
					min_value= get_max(cost[j][k-1],another2_cost + ConstantTCost ) + ConstantECost + transEcost;
					//printf("wwww%d, %d, %d,%d, %d, another2cost: %d\n", i, min_value, another2_cost + ConstantTCost, cost[j][k-1], ConstantECost + transEcost,another2_cost);
				}
				else
					min_value= get_max(cost[j][k-1],tranmiss_size[i-1]/(bandwidth1*1000) * Power_wifi * weight2) + Ecost[j-1];
				cut[i][k] = j-1;
				for(j=i+1;j<=num_services;j++)
				{
					if(j==num_services)
						temp_value = FirstTcost[j-1] + Ecost[j-1] + ConstantTCost + ConstantECost;
					else
						temp_value= get_max(cost[j][k-1],FirstTcost[j-1]+ ConstantTCost) + Ecost[j-1] + ConstantECost;
					if(temp_value<= min_value)
					{
						min_value = temp_value;
						cut[i][k] = j-1;
					}
				}
				cost[i][k] = min_value;
			}
		}
	for(int ww=7;ww>-1;ww--)
		for(int i=0;i<3;i++)
			printf("%d, %d, %d\n",ww, i, cost[ww][i]);
	
	int i=0;
	int j=2;
	for(int j=2;j>0;j--)
	{
		int m = cut[i][j];
		printf("%d\n", m);
		i= m+1;
	}
	for(int ww=7;ww>-1;ww--)
		for(int i=0;i<3;i++)
			printf("%d, %d, %d\n",ww, i, cut[ww][i]);
	
	//printf("hhhhhh%d",LastTcost[6]);
	int cut_place=1;
	printf("First 2 filters:%d\n",FirstTcost[cut_place]);
	printf("ConstantTCost:%d\n",ConstantTCost);
	int transmiss_time22 = tranmiss_size[cut_place]/(bandwidth1*1000); // ms
	int transmiss_energy  =  Power_wifi* transmiss_time22 * weight1;
	printf("Ecost: %d, transmiss_energy: %d, ConstantECost: %d\nEcost + ConstantECost - transmiss_energy: %d\n", Ecost[cut_place], transmiss_energy, ConstantECost, Ecost[cut_place] + ConstantECost - transmiss_energy);
	
	//latency
	int latency = (FirstTcost[cut_place] + ConstantTCost)/(P_adjust*weight2);
	printf("First 2 latency: %d, %d\n",FirstTcost[cut_place] + ConstantTCost, latency);

}


