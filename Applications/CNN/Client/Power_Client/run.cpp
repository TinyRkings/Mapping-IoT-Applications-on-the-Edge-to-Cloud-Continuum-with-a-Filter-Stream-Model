#include "cnn.h"
#include "Client.h"
#include <time.h>


using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	omp_init_lock(&lock);
	// load the cnn model
	cnn trafficSign;
	trafficSign.loadModel("./data/gtsrb.txt");

	// forward the cnn
	stringstream ss;
	string filename;
	Mat img;
	
	//connect to server
	int sock_fd = connect_server();
	cout<<"********successfully connected************"<<endl;
	
	////////////////////power measurement//////////////////////////
	// Create I2C bus
	int file;
	char *bus = "/dev/i2c-1";
	if ((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, ADS1115 I2C address is 0x48(72)
	ioctl(file, I2C_SLAVE, 0x48);
	int t1_power, t2_power, t3_power, t4_power, t5_power, t6_power, t7_power, t8_power,t9_power, t10_power, t11_power,wifi_power;
	t1_power = 0;
	t2_power= 0;
	t3_power = 0;
	t4_power = 0;
	t5_power = 0;
	t6_power = 0;
	t7_power = 0;
	t8_power = 0;
	t9_power = 0;
	t10_power = 0;
	t11_power = 0;
	wifi_power = 0;
	int CurrentPower=0;
	int t1_iteration =0;
	int t2_iteration = 0;
	int t3_iteration =0;
	int t4_iteration = 0;
	int t5_iteration = 0;
	int t6_iteration =0;
	int t7_iteration = 0;
	int t8_iteration =0;
	int t9_iteration = 0;
	int t10_iteration = 0;
	int t11_iteration = 0;
	int wifi_iteration = 0;
	////////////////////power measurement//////////////////////////
	
	
	//initial notification
	int gate = 11;
	int int_size = sizeof(int);
    char sendline1[int_size];
    memset(sendline1,0,int_size); 
    memcpy(sendline1, &gate, int_size);
    client_send(sock_fd, (void*)sendline1, int_size);
    
	/*
	VideoCapture captRefrnc(0);
    if (!captRefrnc.isOpened())
    {
        cout  << "Could not open reference "  << endl;
        return -1;
    }
    Size refS = Size((int) captRefrnc.get(CAP_PROP_FRAME_WIDTH),
                     (int) captRefrnc.get(CAP_PROP_FRAME_HEIGHT));
    int Fps = (int) captRefrnc.get(CAP_PROP_FPS);
    cout << "Reference frame resolution: Width=" << refS.width << "  Height=" << refS.height
         << " of Fps :# " << captRefrnc.get(CAP_PROP_FPS) << endl;
    */ 
	int sampling =3;
	
	Mat frameReference;
	Mat grayImage;
	for(; ;)
	{
		for(int idx=1; idx <= 100; ++idx)
    	{
    		/*
    		captRefrnc >> frameReference;
    		if (frameReference.empty())
        	{
            	break;
        	}
        	*/
        	
        	//generate images
        	ss.clear();
			//ss<<"./data/png/"<<setw(5)<<setfill('0')<<idx<<".png";
			ss<<"./data/jpg/00001.jpg";
			ss>>filename;
			frameReference = imread(filename);
        	
        	CurrentPower = Current_Power(file);
        	t1_power = (t1_power * t1_iteration + CurrentPower)/(t1_iteration + 1);
        	t1_iteration ++;
        	//generated image:640*480*3 channels
        	if(gate ==1)
        	{
        		//Send Frames
        		client_send(sock_fd, (void *)frameReference.data, 640*480*3);
        		CurrentPower = Current_Power(file);
        		wifi_power = (wifi_power * wifi_iteration + CurrentPower)/(wifi_iteration + 1);
        		wifi_iteration ++;
        	}
        	//bgr2gray image:640*480
        	else if(gate ==2)
        	{
        		//bgr2gray
        		cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        		//Send Frames
        		client_send(sock_fd, (void *)grayImage.data, 640*480);
        		
        	}
        //Resized image:32*32
		if(gate == 3)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double img[32][32];
			int send_size= sizeof(double)*32*32;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.img, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline, send_size);
		}
		else if(gate ==4)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double conv1tanh2[16][28][28];
			trafficSign.forward12();
			int send_size= sizeof(double)*16*28*28;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.conv1tanh2, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline, send_size);
		}
		else if(gate ==5)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double pooling3[16][14][14];
			trafficSign.forward12();
			trafficSign.forward3();
			int send_size= sizeof(double)*16*14*14;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.pooling3, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline, send_size);
		}
		else if(gate ==6)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double conv4tanh5[32][10][10];
			trafficSign.forward12();
			trafficSign.forward3();
			trafficSign.forward45();
			int send_size= sizeof(double)*32*10*10;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.conv4tanh5, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  send_size);
		}
		else if(gate ==7)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double pooling6[32][5][5];
			trafficSign.forward12();
			trafficSign.forward3();
			trafficSign.forward45();
			trafficSign.forward6();
			int send_size= sizeof(double)*32*5*5;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.pooling6, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  send_size);
		}
		else if(gate ==8)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double reshape7[800];
			trafficSign.forward12();
			trafficSign.forward3();
			trafficSign.forward45();
			trafficSign.forward6();
			trafficSign.forward7();
			int send_size= sizeof(double)*800;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.reshape7, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  send_size);
		}
		else if(gate ==9)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double linear8tanh9[256];
			trafficSign.forward12();
			trafficSign.forward3();
			trafficSign.forward45();
			trafficSign.forward6();
			trafficSign.forward7();
			trafficSign.forward89();
			int send_size= sizeof(double)*256;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.linear8tanh9, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  send_size);
		}
		else if(gate ==10)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double linear10[43];
			trafficSign.forward12();
			trafficSign.forward3();
			trafficSign.forward45();
			trafficSign.forward6();
			trafficSign.forward7();
			trafficSign.forward89();
			trafficSign.forward10();
			int send_size= sizeof(double)*43;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.linear10, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  send_size);
		}
		else if(gate == 11)
		{
			sleep(1);
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	CurrentPower = Current_Power(file);
        	t2_power = (t2_power * t2_iteration + CurrentPower)/(t2_iteration + 1);
        	t2_iteration ++;
        	
        	sleep(1);
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	CurrentPower = Current_Power(file);
        	t3_power = (t3_power * t3_iteration + CurrentPower)/(t3_iteration + 1);
        	t3_iteration ++;
        	
        	sleep(1);
			trafficSign.forward12();
			CurrentPower = Current_Power(file);
        	t4_power = (t4_power * t4_iteration + CurrentPower)/(t4_iteration + 1);
        	t4_iteration ++;
        	
        	sleep(1);
			trafficSign.forward3();
			CurrentPower = Current_Power(file);
        	t5_power = (t5_power * t5_iteration + CurrentPower)/(t5_iteration + 1);
        	t5_iteration ++;
        	
        	sleep(1);
			trafficSign.forward45();
			CurrentPower = Current_Power(file);
        	t6_power = (t6_power * t6_iteration + CurrentPower)/(t6_iteration + 1);
        	t6_iteration ++;
        	
        	sleep(1);
			trafficSign.forward6();
			CurrentPower = Current_Power(file);
        	t7_power = (t7_power * t7_iteration + CurrentPower)/(t7_iteration + 1);
        	t7_iteration ++;
        	
        	sleep(1);
			trafficSign.forward7();
			CurrentPower = Current_Power(file);
        	t8_power = (t8_power * t8_iteration + CurrentPower)/(t8_iteration + 1);
        	t8_iteration ++;
        	
        	sleep(1);
			trafficSign.forward89();
			CurrentPower = Current_Power(file);
        	t9_power = (t9_power * t9_iteration + CurrentPower)/(t9_iteration + 1);
        	t9_iteration ++;
        	
        	sleep(1);
			trafficSign.forward10();
			CurrentPower = Current_Power(file);
        	t10_power = (t10_power * t10_iteration + CurrentPower)/(t10_iteration + 1);
        	t10_iteration ++;
        	
        	sleep(1);
			int label = trafficSign.labelresult();
			CurrentPower = Current_Power(file);
        	t11_power = (t11_power * t11_iteration + CurrentPower)/(t11_iteration + 1);
        	t11_iteration ++;
			char sendline[int_size];
			char* moving = sendline;
    		memcpy(moving, &label, int_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  int_size);
		}
		}
	printf("gate %d, 100 images t2 used power:%f us\n",gate, t1_power);
	printf("gate %d, 100 images t2 used power:%f us\n",gate, t2_power);
	printf("gate %d, 100 images t3 used power:%f us\n",gate, t3_power); 
	printf("gate %d, 100 images t4 used power:%f us\n",gate, t4_power);
	printf("gate %d, 100 images t5 used power:%f us\n",gate, t5_power);
	printf("gate %d, 100 images t6 used power:%f us\n",gate, t6_power); 
	printf("gate %d, 100 images t7 used power:%f us\n",gate, t7_power);
	printf("gate %d, 100 images t8 used power:%f us\n",gate, t8_power); 
	printf("gate %d, 100 images t9 used power:%f us\n",gate, t9_power);
	printf("gate %d, 100 images t10 used power:%f us\n",gate, t10_power); 
	printf("gate %d, 100 images t11 used power:%f us\n",gate, t11_power);
	printf("gate %d, 100 images wifi used power:%f us\n",gate, wifi_power);
	printf("\n");
	}
	
	return 0;
}
