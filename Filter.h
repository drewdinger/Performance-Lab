//-*-c++-*-
#ifndef _Filter_h_
#define _Filter_h_

using namespace std;

class Filter {
  int divisor;
  int dim;
  int *data;

public:
  Filter(int div);
  int get(int r);
  void set(int r, int value);

  int getDivisor();

  int getSize();
  void info();
};

#endif
