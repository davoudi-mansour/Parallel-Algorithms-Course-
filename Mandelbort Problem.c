#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h> 
#include <math.h>
#include <graphics.h>
#include <X11/Xlib.h>
# define data_tag 100
#define terminator 200
typedef struct {
	float real, imag;
} complex;

int cal_pixel(complex c) {
	int count = 0, max = 256;
	float temp, lengthsq;
	complex z;
	z.real = 0;
	z.imag = 0;

	do {
		temp = z.real * z.real - z.imag * z.imag + c.real;
		z.imag = 2 * z.real * z.imag + c.imag;
		z.real = temp;
		lengthsq = z.real * z.real + z.imag * z.imag;
		count++;
	} while ((lengthsq < 4) && (count < max));
	return count;
}

int main(int argc, char **argv) {
/*
exercise #1-> an efficient parallel program using p processors for Mandelbrot problem---Mansur Davudi 9731568  1397/08/09
type=1---method1() ---> static (consecutive rows)
type=2---method2()---> static (using cyclic rows)
type=3---method3()---> dynamic task assignment
TO RUN--->mpirun -n pp a.out height width type

*/
	printf("mansur davudi ex_1 9731568 1397/08/09 Mandelbrot problem \n\n");
	
	int height=atoi(argv[1]);
	int width=atoi(argv[2]);
	int type=atoi(argv[3]);
	switch(type)
	{
	case(1):
	method1(argc,height,width,argv);
	break;
	case(2):
	method2(argc,height,width,argv);
	break;
	case(3):
	method3(argc,height,width,argv);
	break;
	default:
	printf(" Please Enter Valid Type id; 1,2 or 3");
	exit(1);

	}

	
	return 0;
}
void method1(int argc, int height,int width,char **argv)
{
int myid, procs,  first, last, k, err;
	
	int gdriver = 0, gmode = 0; 
	MPI_Status status;

	err = MPI_Init(&argc, &argv);//init MPI
	if (err != MPI_SUCCESS) {
		printf("Error in init MPI\n");
		MPI_Abort(MPI_COMM_WORLD, err);
	}



	MPI_Comm_rank(MPI_COMM_WORLD, &myid);//get rank of each proc
	MPI_Comm_size(MPI_COMM_WORLD, &procs);//get num of proc

	if (myid == 0) {
		if (argc < 4) {
			printf("\n\tOOOPS...., INVALID No of Arguements,\n"
				"\tA program Parallel Mandelbrot in Ubuntu\n"
				"\tTO RUN	mpirun -n pp a.out height width type \n\n\n");
		} 
	} 

	if (argc < 4) { MPI_Finalize(); }   
	float rmin = -2, imin = -2, rmax = 2, imax = 2; 
	float scale_real = (rmax - rmin) / width;
	float scale_imag = (imax - imin) / height;
	k = height / procs;
	first = myid * k;
	last = first + k;
	int *temp = (int*)malloc(k * width * sizeof(int));// each proc has a local array temp and calculate the result by that and sent it to master
	int *total = (int*)malloc(height * width * sizeof(int));
//master proc collect the result in total array
	if (myid != 0)
	{
		complex c;
		int count=0;//each proc calculate it's contribution from first to last
		for (int x = first; x <last; x++){
			for (int y = 0; y < width; y++) {
				c.real = rmin + ((float)x * scale_real);
				c.imag = imin + ((float)y * scale_imag);
				int color = cal_pixel(c);
				temp[count * width + y] = color;
			}
                          count++;
               }
// each proc send temp array to master

		MPI_Send(temp, k * width, MPI_INT, 0, myid, MPI_COMM_WORLD);
	}
	if (myid == 0) {
		XInitThreads();
		detectgraph(&gdriver, &gmode);
		initgraph(&gdriver, &gmode, 0);
		complex c;	
		int count=0;
//master calculate it's contribution and save the result in total
		for (int x = first; x <last; x++){     
			for (int y = 0; y < width; y++) {
				c.real = rmin + ((float)x * scale_real);
				c.imag = imin + ((float)y * scale_imag);
				int color = cal_pixel(c);
				total[count * width + y] = color;

			}
                        count++;
                }
// master collect the result of other procs and save them in total array
		for (int i = 0; i < procs - 1; i++)
		{
			MPI_Recv(temp, k*width, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			
			int s = status.MPI_TAG;
			int z;
			for (z = 0; z < k * width; z++)
			{
				total[s * k * width + z] =temp[z];
			}

		}	
// master trace the whole the total array and show the result in screen
			int x,y;
                        for (int i = 0; i < height*width; i++)
		{
			x = i / width;
			y = i % width;
			putpixel(x, y, total[x * width + y]);
		}
                        
	}
	
	MPI_Finalize();
	closegraph();

}
void method2(int argc, int height,int width,char **argv)
{
int myid, procs, err,  k;
	
	int gdriver = 0, gmode = 0;
	MPI_Status status;

	
	err = MPI_Init(&argc, &argv);
	if (err != MPI_SUCCESS) {
		printf("Error in init MPI\n");
		MPI_Abort(MPI_COMM_WORLD, err);
	}

	 
	MPI_Comm_size(MPI_COMM_WORLD, &procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	if (myid == 0) {
		if (argc < 4) {
			printf("\n\tOOOPS...., INVALID No of Arguements,\n"
				"\tA program Parallel Mandelbrot in Ubuntu\n"
				"\tTO RUN	mpirun -n pp a.out height width type\n\n\n");
		} 
	} 

	if (argc < 4) { MPI_Finalize(); } 
	float rmin = -2, imin = -2, rmax = 2, imax = 2; 
	float scale_real = (rmax - rmin) / width; 
	float scale_imag = (imax - imin) / height; 
	k = height / procs;
	int *temp = (int*)malloc( width * sizeof(int));
	int *total = (int*)malloc(height * width * sizeof(int));

	if (myid != 0)
	{
		complex c;
// each procs caculate it's contribution and save the result in temp array and send it to master, each proc calculates rows started from it's id, 
//id+1*num of procs, id+2*num of procs and so on
	for (int x = myid; x < height; x += procs)
			for (int y = 0; y < width; y++) {
				c.real = rmin + ((float)x * scale_real);
				c.imag = imin + ((float)y * scale_imag);
				int color = cal_pixel(c);
				temp[y] = color;
			MPI_Send(temp, width, MPI_INT, 0, x, MPI_COMM_WORLD);
			}

	}
	if (myid == 0) {
		XInitThreads();
		detectgraph(&gdriver, &gmode);
		initgraph(&gdriver, &gmode, 0);
		complex c;
// master calculate it's contribution and save the result in total
		for (int x = 0; x < height; x += procs)
			for (int y = 0; y < width; y++) {
				c.real = rmin + ((float)x * scale_real);
				c.imag = imin + ((float)y * scale_imag);
				int color = cal_pixel(c);
				total[x * width + y] = color;
			}
// master place each received result in it's proper location in total array
		for (int i = 0; i < height; i++)
		{
			if((i %k ) != 0)
			{
				MPI_Recv(temp,width, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				int s=status.MPI_TAG;
				for (int z = 0; z < width; z++)
			{
		                total[s * width + z] = temp[z];
			}
			}
			
		}
// master trace whole the total array and showes the result in screen
		int q;
	 	int w;
		for (int i = 0; i < height*width; i++)
		{
			q = i / width;
			w = i % width;
			putpixel(q, w, total[q * width + w]);
		}
	}
	MPI_Finalize();
	closegraph();
}

void method3(int argc, int height,int width,char **argv)
{
int myid, procs, err;
	
	int gdriver = 0, gmode = 0;
	MPI_Status status;

	
	err = MPI_Init(&argc, &argv);
	if (err != MPI_SUCCESS) {
		printf("Error in init MPI\n");
		MPI_Abort(MPI_COMM_WORLD, err);
	}

	 
	MPI_Comm_size(MPI_COMM_WORLD, &procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	if (myid == 0) {
		if (argc < 4) {
			printf("\n\tOOOPS...., INVALID No of Arguements,\n"
				"\tA program Parallel Mandelbrot in Ubuntu\n"
				"\tTO RUN	mpirun -n pp a.out height width type\n\n\n");
		} 
	} 

	if (argc < 4) { MPI_Finalize();} 
	float rmin = -2, imin = -2, rmax = 2, imax = 2;
	float scale_real = (rmax - rmin) / width;
	float scale_imag = (imax - imin) / height; 
	int *temp = (int*)malloc(width * sizeof(int));
	int *total = (int*)malloc(height * width * sizeof(int));
	int count = 0, row = 0;
	if (myid != 0)
	{
// at the first each proc caclculate one row and send the result to master
		
		MPI_Recv(&row, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		complex c;
		int s;
		s=status.MPI_TAG;
// while the tag of received message from master equals to data_tag each proc caclculate the row num in message and save it to temp and send it to master
		while(s == data_tag){
			
			for (int y = 0; y < width; y++) {
			c.real = rmin + ((float)row * scale_real);
			c.imag = imin + ((float)y * scale_imag);
			int color = cal_pixel(c);
			temp[y] = color;
		}
		MPI_Send(temp, width, MPI_INT, 0, row, MPI_COMM_WORLD);
		MPI_Recv(&row, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		s=status.MPI_TAG;
}
// if the tag number in received message equals to terminate the calculate process is finished  by each proc
		MPI_Finalize();	

	}
	
	if (myid == 0) {
		XInitThreads();
		detectgraph(&gdriver, &gmode);
		initgraph(&gdriver, &gmode, 0);
// at the first master send some rows to others 
		for (int i = 0; i < procs - 1 ; i++)
		{
			MPI_Send(&row, 1, MPI_INT, i+1 ,data_tag, MPI_COMM_WORLD);
			count++;
			row++;
		}
// while count < num of row master send rows to others by data_tag tag and waits to receive the result then place the result in proper location in total array
		do
		{
			MPI_Recv(temp, width, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			
			int p=status.MPI_TAG;
			for (int z = 0; z < width; z++)
			{
				total[p * width + z] = temp[z];
				
			}

			count--;
			int des = status.MPI_SOURCE;
			if (row < height)
			{
				MPI_Send(&row, 1, MPI_INT, des, data_tag, MPI_COMM_WORLD);
				row++;
				count++;
			}
			else
			{
				MPI_Send(&row, 1, MPI_INT, des, terminator, MPI_COMM_WORLD);
			}

		} while (count > 0);
// master trace whole the total array and show the resukt in screen
		int x,y;
		for (int i = 0; i < height * width; i++)
			{
				x=i / width;
				y=i % width;
				putpixel(x,y,total[x * width +y]);
						}
			
	}
	
	closegraph();
}



