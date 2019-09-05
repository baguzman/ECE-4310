// Brian Guzman
// ECE 4310
// Project 1: Smoothing

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main(int argc, char *argv[])
{
	//VARIABLES
	FILE *fin, *fout;
	char file_type[80];
	int r,c,ROWS,COLS,MAX,r2,c2;
	float sum;
	float *temp_image;
	unsigned char *image_in, *image_out;
	struct timespec start_time, stop_time;
	int i, num_runs = 10;
	long int total_time_2D = 0;
	long int t_time_seperable = 0;
	long int t_time_sliding = 0;

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
	
	//ALLOCATE MEMORY FOR INPUT IMAGE AND OUTPUT IMAGES
	image_in = (unsigned char *)calloc(ROWS * COLS,sizeof(unsigned char));
	image_out = (unsigned char *)calloc(ROWS * COLS,sizeof(unsigned char));
	temp_image = (float *)calloc(ROWS * COLS,sizeof(float));
	if(image_in == NULL || image_out == NULL || temp_image == NULL)
	{
		printf("Unable to allocate memory\n");
		exit(0);
	}
	
	//READ IMAGE DATA
	fread(image_in,1,ROWS*COLS,fin);
	fclose(fin);
	
	//2D CONVOLUTION ALGORITHM
	for(i = 0; i < num_runs; i++)
	{
		//RUN TIMER
		clock_gettime(CLOCK_REALTIME,&start_time);
		printf("2D CONVOLUTION:\n%ld %ld\n",(long int)start_time.tv_sec,start_time.tv_nsec);
		
		for(r = 3; r < ROWS-3; r++)
		{
			for(c = 3; c < COLS-3; c++)
			{
				sum = 0;
				for(r2 = -3; r2 <= 3; r2++)
				{
					for(c2 = -3; c2 <= 3; c2++)
					{
						sum += image_in[((r+r2)*COLS)+(c+c2)];
					}
				}
				image_out[(r*COLS)+c] = sum/49;
			}
		}
	
		//STOP TIMER
		clock_gettime(CLOCK_REALTIME,&stop_time);
		printf("%ld %ld\n\n",(long int)stop_time.tv_sec,stop_time.tv_nsec);
		total_time_2D += stop_time.tv_nsec - start_time.tv_nsec;
	}
	
	//WRITE OUTPUT ITMAGE
	fout = fopen("smoothed.ppm","w");
	fprintf(fout,"P5 %d %d 255\n", COLS, ROWS);
	fwrite(image_out,COLS*ROWS,1,fout);
	fclose(fout);

	//SEPERABLE FILTERS ALGORITHM
	for(i = 0; i < num_runs; i++)
	{
		//RUN TIMER
		clock_gettime(CLOCK_REALTIME,&start_time);
		printf("SEPERABLE FILTERS:\n%ld %ld\n",(long int)start_time.tv_sec,start_time.tv_nsec);
		
		for(r = 0; r < ROWS; r++)
		{
			for(c = 3; c < COLS-3; c++)
			{
				sum = 0;
				for(c2 = -3; c2 <= 3; c2++)
				{
					sum += image_in[(r*ROWS)+(c+c2)];
				}
				temp_image[(r*ROWS)+c] = sum;
			}
		}
		for(r = 3; r < ROWS-3; r++)
		{
			for(c = 0; c < COLS; c++)
			{
				sum = 0;
				for(r2 = -3; r2 <= 3; r2++)
				{
					sum += temp_image[((r+r2)*ROWS)+c];
				}
				image_out[(r*ROWS)+c] = sum/49;
			}
		}
		
		//STOP TIMER
		clock_gettime(CLOCK_REALTIME,&stop_time);
		printf("%ld %ld\n\n",(long int)stop_time.tv_sec,stop_time.tv_nsec);
		t_time_seperable += stop_time.tv_nsec - start_time.tv_nsec;
	}
	
	//WRITE IMAGE
	fout = fopen("sep_filters.ppm","w");
	fprintf(fout,"P5 %d %d 255\n", COLS, ROWS);
	fwrite(image_out,COLS*ROWS,1,fout);
	fclose(fout);
	
	//SLIDING WINDOW ALGORITHM
	for(i = 0; i < num_runs; i++)
	{
		//RUN TIMER
		clock_gettime(CLOCK_REALTIME,&start_time);
		printf("SEPERABLE FILTERS:\n%ld %ld\n",(long int)start_time.tv_sec,start_time.tv_nsec);
		
		
		
		//STOP TIMER
		clock_gettime(CLOCK_REALTIME,&stop_time);
		printf("%ld %ld\n\n",(long int)stop_time.tv_sec,stop_time.tv_nsec);
		t_time_sliding += stop_time.tv_nsec - start_time.tv_nsec;
	}
	//WRITE IMAGE
	fout = fopen("sliding_window.ppm","w");
	fprintf(fout,"P5 %d %d 255\n", COLS, ROWS);
	fwrite(image_out,COLS*ROWS,1,fout);
	fclose(fout);
	
	//PRINT OUT RESULTS
	printf("Average 2D Convolution Time: %ld\n",total_time_2D/num_runs);
	printf("Average Seperable Filters Time: %ld\n",t_time_seperable/num_runs);
	printf("Average Sliding Window Time: %ld\n",t_time_sliding/num_runs);
	
	
	return(0);
}
