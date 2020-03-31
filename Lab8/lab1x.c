//Brian Guzman
//ECE 4680
//Lab1 - X Image Viewer
//Spring 2019


//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int					ROWS,COLS,MAX;
	unsigned char		*displaydata;


	FILE *fin;
	char type[80];
	unsigned char *image;
	int i,j;
	unsigned char r,g,b;

	//CHECK FOR PROPER COMMAND LINE ARGUMENTS
	if(argc != 2)
	{
	  printf("Usage: lab1 [filename]\n");
	  exit(0);
	}
	//OPEN THE GIVEN FILE
	fin = fopen(argv[1], "rb");
	if(fin == NULL)
	{
	  printf("Unable to open file %s\n",argv[1]);
	  exit(0);
	}
	
	//SCAN THE HEADER INFO AND THE LAST WHITESPACE
	fscanf(fin, "%s %d %d %d", type, &COLS, &ROWS, &MAX);
	fread(&type[79],1,1,fin);
	
	//ALLOCATE SPACE FOR THE DISPLAY DATA (16 BIT)
	displaydata = (unsigned char*)calloc(ROWS * COLS * 2, sizeof(unsigned char));
	
	//CHECK THE TYPE OF FILE BEING READ (P5 = GREYSCALE, P6 = RGB)
	if(strcmp(type,"P5") == 0)
	{
		//ALLOCATE SPACE TO READ INPUT FILE AND READ THE FILES
		image = (unsigned char *)calloc(ROWS * COLS, sizeof(unsigned char));
		fread(image, ROWS*COLS,1,fin);
		
		//LOOP THROUGH PIXELS
		for(i = 0; i < ROWS*COLS; i++)
		{
			//SHIFT READ BITS TO GET RED GREEN AND BLUE
			r = b = image[i] >> 3;
			g = image[i] >> 2;
			
			//COPY INFO TO DISPLAY DATA
			displaydata[i*2+0] = ((g<<5) | b);
			displaydata[i*2+1] = ((r<<3) | (g>>3));
		}
	}
	else if(strcmp(type,"P6") == 0)
	{
		//ALLOCATE SPACE TO READ INPUT FILE AND READ THE FILE
		image = (unsigned char *)calloc(ROWS * COLS * 3, sizeof(unsigned char));
		fread(image, ROWS*COLS*3,1,fin);
		j = 0;
		//LOOP THOUGH PIXELS
		for(i = 0; i < ROWS*COLS*3; i+=3)
		{
			//SHIFT READ BITS TO GET RED GREEN AND BLUE
			r = image[i] >> 3;
			g = image[i+1] >> 2;
			b = image[i+2] >> 3;
			
			//COPY INTO TO DISPLAY DATA
			displaydata[j*2+0] = ((g<<5) | b);
			displaydata[j*2+1] = ((r<<3) | (g>>3));
			j++;
		}
	}
	else
	{
		printf("Invalid File Type\n");
		exit(0);
	}

	// WRITE OUTPUT FILE
	FILE *outputFile;
	outputFile = fopen("converted.ppm", "w");
	fprintf(outputFile, "P6 %d %d 255\n", COLS, ROWS);
	fwrite(displaydata, ROWS * COLS, 1, outputFile);
	fclose(outputFile);
	//CLOSE THE FILE
	fclose(fin);
	

}
