// Brian Guzman
// ECE 4310
// Project 1-1: 2D Convolution

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main(int argc, char *argv[])
{
	//VARIABLES
	FILE *fin, *fout;
	char file_type[80];
	int r,c,ROWS,COLS,MAX,r2,c2,sum;
	unsigned char *image_in, *image_out;
	struct timespec start_time, stop_time;

	//VERIFY COMMAND LINE ARGUMENTS ARE PRESENT
	if(argc != 2)
	{
		printf("Usage: ./convolution [filename]\n");
		exit(0);
	}
	
	//OPEN INPUT IMAGE
	fin = fopen(argv[1], "rb");
	if(fin == NULL)
	{
		printf("Unable to open file %s\n", argv[1]);
		exit(0);
	}
	
	//PARSE HEADER
	fscanf(fin, "%s %d %d %d", file_type, &COLS, &ROWS, &MAX);
	fread(&file_type[79],1,1,fin);
	
	//ALLOCATE MEMORY FOR INPUT IMAGE AND OUTPUT IMAGE
	image_in = (unsigned char *)calloc(ROWS * COLS,sizeof(unsigned char));
	image_out = (unsigned char *)calloc(ROWS * COLS,sizeof(unsigned char));
	if(image_in == NULL || image_out == NULL)
	{
		printf("Unable to allocate memory\n")
		exit(0);
	}
	
	//READ IMAGE DATA
	fread(image_in,1,ROWS*COLS,fin);
	fclose(fin);
	
	//RUN TIMER
	clock_gettime(CLOCK_REALTIME,&start_time);
	printf("%ld %ld\n",(long int)start_time.tv_sec,start_time.tv_nsec);
	
	//CONVOLUTION ALGORITHM
	for(r = 3; r < ROWS-3; r++)
	{
		for(c = 3; c < COLS-3; c++)
		{
			sum = 0;
			for(r2 = -3; r2 <= 3; r2++)
			{
				for(c2 = -3; c2 <= 3; c2++)
				{
					sum += image[(r+r2)*COLS*(c+c2)];
				}
			}
			smoothed[r*COLS+c] = sum/49;
		}
	}
	
	//STOP TIMER
	clock_gettime(CLOCK_REALTIME,&stop_time);
	printf("%ld %ld\n",(long int)stop_time.tv_sec,stop_time.tv_nsec);
	
	//PRINT OUT RESULTS
	printf("Convolution Time: %ld\n",stop_time.tv_nsec - start_time.tv_nsec);
	
	//WRTIE OUTPUT IMAGE
	fout = fopen("smoothed.ppm","w");
	fprintf(fout,"P5 %d %d 255\n", COLS, ROWS);
	fwrite(smoothed,COLS*ROWS,1,fout);
	fclose(fout);
	
	return(0);
}
