// Brian Guzman
// ECE 4310
// Project 2: MSF/ROC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main(int argc, char *argv[])
{
	//VARIABLES
	FILE *inputFile, *templateFile, *outputFile;
	char header[80], templateHeader[80];
	int inputRows, inputCols, inputMax;
	int templateRows, templateCols, templateMax;
	unsigned char *imageIn, *templateImage, *outputImage;

	//VERIFY COMMAND LINE ARGUMENTS ARE PRESENT
	if (argc != 4)
	{
		printf("Usage: ./convolution [inputImage] [templateImage] [groundTruth]\n");
		exit(0);
	}

	//OPEN INPUT IMAGE
	inputFile = fopen(argv[1], "rb");
	templateFile = fopen(argv[2], "rb");
	if (inputFile == NULL)
	{
		printf("Unable to open file %s\n", argv[1]);
		exit(0);
	}
	if (templateFile == NULL)
	{
		printf("Unable to open file %s\n", argv[2]);
	}

	//PARSE HEADERS
	fscanf(inputFile, "%s %d %d %d", header, &inputCols, &inputRows, &inputMax);
	fread(&header[79], 1, 1, inputFile);

	fscanf(templateFile, "%s %d %d %d", templateHeader, &templateCols, &templateRows, &templateMax);
	fread(&templateHeader[79], 1, 1, templateFile);

	//ALLOCATE MEMORY FOR INPUT IMAGES AND OUTPUT IMAGES
	imageIn = (unsigned char *)calloc(inputRows * inputCols, sizeof(unsigned char));
	templateImage = (unsigned char *)calloc(templateRows * templateCols, sizeof(unsigned char));
	if (imageIn == NULL || templateImage == NULL)
	{
		printf("Unable to allocate sufficient memory\n");
		exit(0);
	}

	//READ IMAGE DATA
	fread(imageIn, 1, inputRows*inputCols, inputFile);
	fclose(inputFile);
	fread(templateImage, 1, templateRows * templateCols, templateFile);
	fclose(templateFile);

	//FIND ZERO MEAN TEMPLATE
	int sum = 0, count, mean;
	int *zeroMeanTemplate;

	zeroMeanTemplate = (int *)calloc(templateRows * templateCols, sizeof(int));

	for (count = 0; count < (templateRows * templateCols); count++)
	{
		sum += templateImage[count];
	}

	mean = sum / (templateRows * templateCols);

	for (count = 0; count < (templateCols * templateRows); count++)
	{
		zeroMeanTemplate[count] = templateImage[count] - mean;
	}

	//MSF ALGORITHM
	int r, c, r2, c2;
	int *msfImage;

	msfImage = (int *)calloc(inputCols * inputRows, sizeof(int));

	for (r = 7; r < inputRows - 7; r++)
	{
		for (c = 4; c < inputCols - 4; c++)
		{
			sum = 0;
			for (r2 = -7; r2 < templateRows - 7; r2++)
			{
				for (c2 = -4; c2 < templateCols - 4; c2++)
				{
					sum += zeroMeanTemplate[(templateCols * (r2 + 7)) + (c2 + 4)] * imageIn[(inputCols * (r + r2)) + (c + c2)];
				}
			}
			msfImage[(inputCols * r) + c] = sum;
		}
	}

	//CALCULATE MAX AND MIN OF MSF OUTPUT
	int max;
	int min;

	max = min = msfImage[0];

	for (count = 1; count < inputRows * inputCols; count++)
	{
		if (msfImage[count] > max)
		{
			max = msfImage[count];
		}
		if (msfImage[count] < min)
		{
			min = msfImage[count];
		}
	}

	//NORMALIZE MSF IMAGE 
	outputImage = (unsigned char *)calloc(inputRows * inputCols, sizeof(unsigned char));

	for (count = 0; count < inputRows * inputCols; count++)
	{
		outputImage[count] = (msfImage[count] - min)*(255) / (max - min);
	}

	outputFile = fopen("normalized.ppm", "w");
	fprintf(outputFile, "P5 %d %d 255\n", inputCols, inputRows);
	fwrite(outputImage, inputCols * inputRows, 1, outputFile);
	fclose(outputFile);

	//CALCULATE ROC AND DETERMINE BEST THRESHOLD
	FILE *groundFile, *rocFile;
	int threshold, match, found;
	int TP, TN, FP, FN;
	char current[2];
	char templateChar[2];
	int groundRow, groundCol;
	unsigned char *thresholdOuput;

	thresholdOuput = (unsigned char *)calloc(inputCols * inputRows, sizeof(unsigned char));
	strcpy(templateChar, "e");
	TP = TN = FP = FN = 0;

	rocFile = fopen("ROC.csv", "w");
	fprintf(rocFile, "Threshold,TP,FP,TN,FN,TPR,FPR,PPV\n");
	groundFile = fopen(argv[3], "r");
	if (groundFile == NULL)
	{
		printf("Error could not read file\n");
		exit(0);
	}


	for (count = 0; count < 256; count += 5)
	{
		TP = TN = FP = FN = 0;
		threshold = count;
		for (c2 = 0; c2 < inputCols * inputRows; c2++)
		{
			if (outputImage[c2] >= threshold)
			{
				thresholdOuput[c2] = 255;
			}
			else
			{
				thresholdOuput[c2] = 0;
			}
		}

		while (fscanf(groundFile, "%s %d %d\n", current, &groundCol, &groundRow) != EOF)
		{
			for (r = groundRow - 7; r <= groundRow + 7; r++)
			{
				for (c = groundCol - 4; c <= groundCol + 4; c++)
				{
					if (thresholdOuput[(r*inputCols) + c] == 255)
					{
						found = 1;
					}
				}
			}

			match = strcmp(current, templateChar);
			if (found == 1 && match == 0)
				TP++;
			if (found == 1 && match != 0)
				FP++;
			if (found == 0 && match == 0)
				FN++;
			if (found == 0 && match != 0)
				TN++;
			found = 0;
		}
		//WRITE OUTPUT DATA FILE
		fprintf(rocFile, "%d,%d,%d,%d,%d,%.2F,%.2F,.%.2F\n", threshold, TP, FP, TN, FN,TP/(double)(TP+FN),FP/(double)(FP+TN),FP/(double)(TP+FP));
		rewind(groundFile);
	}
	fclose(groundFile);
	fclose(rocFile);

	int exit = 0;
	while (exit != 1)
	{
		printf("Enter Threshold Value 0 - 255 to Display or Value > 255 to exit: ");
		scanf("%d", &threshold);
		if (threshold > 255)
		{
			exit = 1;
		}
		else
		{

			for (c2 = 0; c2 < inputCols * inputRows; c2++)
			{
				if (outputImage[c2] < threshold)
				{
					thresholdOuput[c2] = 0;
				}
				else
				{
					thresholdOuput[c2] = 255;
				}
			}
			outputFile = fopen("thresholdOutput.ppm", "w");
			fprintf(outputFile, "P5 %d %d 255\n", inputCols, inputRows);
			fwrite(thresholdOuput, inputCols * inputRows, 1, outputFile);
			fclose(outputFile);
		}
	}
	return(0);
}
