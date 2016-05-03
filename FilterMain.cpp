#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include "Filter.h"


using namespace std;

#include "rtdsc.h"

//
// Forward declare the functions
//
Filter * readFilter(string filename);
//double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);
double applyAvg (cs1300bmp *input, cs1300bmp *output);
double applyEmboss (cs1300bmp *input, cs1300bmp *output);
double applyHline (cs1300bmp *input, cs1300bmp *output);
double applyGauss (cs1300bmp *input, cs1300bmp *output);
int
main(int argc, char **argv)
{

  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  //Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;

  for (int inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
		double sample;
		if (filtername[0] == 'a')
		  //double sample = applyFilter(filter, input, output);
			sample = applyAvg(input, output);
		else if (filtername[0] == 'h')
			sample = applyHline(input, output);
		else if (filtername[0] == 'e')
			sample = applyEmboss(input, output);
		else if (filtername[0] == 'g')
			sample = applyGauss(input, output);
	  sum += sample;
	  samples++;
	  cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
  }
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

struct Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size;
    input >> size;
    int value;
    int div;
    input >> div;
    Filter *filter = new Filter(div);
	input >> value;
	filter -> set(0,value);
	input >> value;
	filter -> set(1,value);
	input >> value;
	filter -> set(2,value);
	input >> value;
	filter -> set(3,value);
	input >> value;
	filter -> set(4,value);
	input >> value;
	filter -> set(5,value);
	input >> value;
	filter -> set(6,value);
	input >> value;
	filter -> set(7,value);
	input >> value;
	filter -> set(8,value);
    return filter;
  }
}
double
applyHline(cs1300bmp *input, cs1300bmp *output)
{
	long long cycStart, cycStop;

	cycStart = rdtscll();
	int width = (input -> width) - 1;
	int height = (input -> height) - 1;
	output -> width = width + 1;
	output -> height = height + 1;
	int plane, row, col, sum, sum1, sum2;
	for(plane = 0; plane < 3; ++plane) 
	{
		for(row = 1; row < height; ++row) 
		{
			for(col = 1; col < width; ++col)
			{
				sum1 = input -> color[plane][row - 1][col - 1] * (-1);
				sum2 = input -> color[plane][row - 1][col] * (-2);
				sum1 += input -> color[plane][row - 1][col + 1] * (-1);
				//sum2 += input -> color[plane][row][col - 1];
				//sum1 += input -> color[plane][row][col];
				//sum2 += input -> color[plane][row][col + 1];
				sum1 += input -> color[plane][row + 1][col - 1];
				sum2 += input -> color[plane][row + 1][col] * 2;
				sum1 += input -> color[plane][row + 1][col + 1];
				sum = sum1 + sum2;

			if ( sum  < 0 ) {
				output -> color[plane][row][col] = 0;
			}

			else if ( sum > 255 ) { 
				output -> color[plane][row][col] = 255;
			}
			else
				output -> color[plane][row][col] = sum;
			}
		}
	}
	

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}

double
applyEmboss(cs1300bmp *input, cs1300bmp *output)
{
	long long cycStart, cycStop;

	cycStart = rdtscll();
	int width = (input -> width) - 1;
	int height = (input -> height) - 1;
	output -> width = width + 1;
	output -> height = height + 1;
	int plane, row, col, sum, sum1, sum2;
	for(plane = 0; plane < 3; ++plane) 
	{
		for(row = 1; row < height; ++row) 
		{
			for(col = 1; col < width; ++col)
			{
				sum1 = input -> color[plane][row - 1][col - 1];
				sum2 = input -> color[plane][row - 1][col];
				sum1 += input -> color[plane][row - 1][col + 1] * (-1);
				sum2 += input -> color[plane][row][col - 1];
				sum1 += input -> color[plane][row][col];
				sum2 += input -> color[plane][row][col + 1] * (-1);
				sum1 += input -> color[plane][row + 1][col - 1];
				sum2 += input -> color[plane][row + 1][col] * (-1);
				sum1 += input -> color[plane][row + 1][col + 1] * (-1);
				sum = sum1 + sum2;

			if ( sum  < 0 ) {
				output -> color[plane][row][col] = 0;
			}

			else if ( sum > 255 ) { 
				output -> color[plane][row][col] = 255;
			}
			else
				output -> color[plane][row][col] = sum;
			}
		}
	}
	

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}

double
applyGauss(cs1300bmp *input, cs1300bmp *output)
{
	long long cycStart, cycStop;

	cycStart = rdtscll();
	int width = (input -> width) - 1;
	int height = (input -> height) - 1;
	output -> width = width + 1;
	output -> height = height + 1;
	int plane, row, col, sum, sum1, sum2;
	for(plane = 0; plane < 3; ++plane) 
	{
		for(row = 1; row < height; ++row) 
		{
			for(col = 1; col < width; ++col)
			{
				//sum1 = input -> color[plane][row - 1][col - 1];
				sum2 = input -> color[plane][row - 1][col] * (4);
				//sum1 += input -> color[plane][row - 1][col + 1];
				sum1 = input -> color[plane][row][col - 1] * 4;
				sum2 += input -> color[plane][row][col] * 8;
				sum1 += input -> color[plane][row][col + 1] * 4;
				//sum2 += input -> color[plane][row + 1][col - 1];
				sum1 += input -> color[plane][row + 1][col] * 4;
				//sum2 += input -> color[plane][row + 1][col + 1];
				sum = sum1 + sum2;
				sum = sum / 24;

			if ( sum  < 0 ) {
				output -> color[plane][row][col] = 0;
			}

			else if ( sum > 255 ) { 
				output -> color[plane][row][col] = 255;
			}
			else
				output -> color[plane][row][col] = sum;
			}
		}
	}
	

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}

double
applyAvg(cs1300bmp *input, cs1300bmp *output)
{
	long long cycStart, cycStop;

	cycStart = rdtscll();
	int width = (input -> width) - 1;
	int height = (input -> height) - 1;
	output -> width = width + 1;
	output -> height = height + 1;
	int plane, row, col, sum, sum1, sum2;
	for(plane = 0; plane < 3; ++plane) 
	{
		for(row = 1; row < height; ++row) 
		{
			for(col = 1; col < width; ++col)
			{
				sum1 = input -> color[plane][row - 1][col - 1];
				sum2 = input -> color[plane][row - 1][col];
				sum1 += input -> color[plane][row - 1][col + 1];
				sum2 += input -> color[plane][row][col - 1];
				sum1 += input -> color[plane][row][col];
				sum2 += input -> color[plane][row][col + 1];
				sum1 += input -> color[plane][row + 1][col - 1];
				sum2 += input -> color[plane][row + 1][col];
				sum1 += input -> color[plane][row + 1][col + 1];
				sum = sum1 + sum2;
				sum = sum / 9;

			if ( sum  < 0 ) {
				output -> color[plane][row][col] = 0;
			}

			else if ( sum > 255 ) { 
				output -> color[plane][row][col] = 255;
			}
			else
				output -> color[plane][row][col] = sum;
			}
		}
	}
	

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}
/*
double
applyFilter(struct Filter *filter, cs1300bmp *input, cs1300bmp *output)
{

  long long cycStart, cycStop;

	cycStart = rdtscll();
	int width = (input -> width) - 1;
	int height = (input -> height) - 1;
	output -> width = width + 1;
	output -> height = height + 1;
	int plane, row, col, sum, sum1, sum2;
	int division = filter -> getDivisor();
	int filt[9];
	filt[0] = filter -> get(0);
	filt[1] = filter -> get(1);
	filt[2] = filter -> get(2);
	filt[3] = filter -> get(3);
	filt[4] = filter -> get(4);
	filt[5] = filter -> get(5);
	filt[6] = filter -> get(6);
	filt[7] = filter -> get(7);
	filt[8] = filter -> get(8);
	for(plane = 0; plane < 3; ++plane) 
	{
		for(row = 1; row < height; ++row) 
		{
			for(col = 1; col < width; ++col)
			{

				//output -> color[plane][row][col] = 0;
			/*	for (i = 0; i < 3; ++i) 
				{
					for (j = 0; j < 3; ++j) 
					{	
						output -> color[plane][row][col]
						= output -> color[plane][row][col]
						+ (input -> color[plane][row + i -1][col + j -1] 
						* filter -> get(i, j) );
					}
				}
				*///output -> color[plane][row][col] = 9 * output -> color[plane][row][col];
/*				sum1 = input -> color[plane][row - 1][col - 1] * filt[0];
				sum2 = input -> color[plane][row - 1][col] * filt[1];
				sum1 += input -> color[plane][row - 1][col + 1] * filt[2];
				sum2 += input -> color[plane][row][col - 1] * filt[3];
				sum1 += input -> color[plane][row][col] * filt[4];
				sum2 += input -> color[plane][row][col + 1] * filt[5];
				sum1 += input -> color[plane][row + 1][col - 1] * filt[6];
				sum2 += input -> color[plane][row + 1][col] * filt[7];
				sum1 += input -> color[plane][row + 1][col + 1] * filt[8];
				sum = sum1 + sum2;
				//output -> color[plane][row][col] = sum;
				sum = sum / division;

			if ( sum  < 0 ) {
				output -> color[plane][row][col] = 0;
			}

			else if ( sum > 255 ) { 
				output -> color[plane][row][col] = 255;
			}
			* 
			else
				output -> color[plane][row][col] = sum;
			}
		}
	}
	

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}
*/
