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
double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

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

  Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;

  for (int inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
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
    //filter -> setDivisor(div); // took out for loop # does nothing
    input >> value;
    filter -> set(0,0,value);
    input >> value;
    filter -> set(0,1,value);
    input >> value;
    filter -> set(0,2,value);
    input >> value;
    filter -> set(1,0,value);
    input >> value;
    filter -> set(1,1,value);
    input >> value;
    filter -> set(1,2,value);
    input >> value;
    filter -> set(2,0,value);
    input >> value;
    filter -> set(2,1,value);
    input >> value;
    filter -> set(2,2,value);
    return filter;
  }
}


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
	filt[0] = filter -> get(0,0);
	filt[1] = filter -> get(0,1);
	filt[2] = filter -> get(0,2);
	filt[3] = filter -> get(1,0);
	filt[4] = filter -> get(1,1);
	filt[5] = filter -> get(1,2);
	filt[6] = filter -> get(2,0);
	filt[7] = filter -> get(2,1);
	filt[8] = filter -> get(2,2);
	for(int plane =0; plane < 3; plane++) 
	{ // changed < 3, 3 is plane size max
		for(int row = 1; row < (input -> height) - 1 ; row = row + 1) 
		{
			for(int col = 1; col < (input -> width) - 1; col = col + 1) 
			{
// remove t=0 not used
	//output -> color[plane][row][col] = 0; dont need because we are using an inner sum and will write over this anyway
/*
	for (int i = 0; i < 3; i++) 
	{  // changed to 3
	  for (int j = 0; j < 3; j++) 
	  {	// changed to 3
	    output -> color[plane][row][col
	      = output -> color[plane][row][col]
	      + (input -> color[plane][row + i - 1][col + j - 1] 
		 * filter -> get(i, j) );
	  }
	}
	*/
	sum1 = input -> color[plane][row - 1][col - 1] * filt[0];
	sum2 = input -> color[plane][row - 1][col] * filt[1];
	sum1 += input -> color[plane][row - 1][col + 1] * filt[2];
	sum2 += input -> color[plane][row][col - 1] * filt[3];
	sum1 += input -> color[plane][row][col] * filt[4];
	sum2 += input -> color[plane][row][col + 1] * filt[5];
	sum1 += input -> color[plane][row + 1][col - 1] * filt[6];
	sum2 += input -> color[plane][row + 1][col] * filt[7];
	sum1 += input -> color[plane][row + 1][col + 1] * filt[8];
	sum = sum1 +sum2;
	//output -> color[plane][row][col] = 	
	  sum = sum / division;

	if ( sum < 0 ) {
	  output -> color[plane][row][col] = 0;
	}
// else if so it doesn't loop 
	else if ( sum > 255 ) { 
	  output -> color[plane][row][col] = 255;
	}  
	 else {
		output -> color[plane][row][col] = sum;
	}
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
