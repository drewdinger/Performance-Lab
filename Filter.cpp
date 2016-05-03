#include "Filter.h"
#include <iostream>

Filter::Filter(int div)
{
  divisor = div;
  dim = 3;
  data = new int[9];
}

int Filter::get(int r)
{
  return data[r];
}

void Filter::set(int r, int value)
{
  data[r] = value;
}

int Filter::getDivisor()
{
  return divisor;
}

int Filter::getSize()
{
  return 3;
}

void Filter::info()
{
  cout << "Filter is.." << endl;
  cout << get(0) << " ";
  cout << get(1) << " ";
  cout << get(2) << " ";
  cout << get(3) << " ";
  cout << get(4) << " ";
  cout << get(5) << " ";
  cout << get(6) << " ";
  cout << get(7) << " ";
  cout << get(8) << " ";
  /*
  for (int col = 0; col < 3; ++col) {
    for (int row = 0; row < 3; ++row) {
      int v = get(row, col);
      cout << v << " ";
    }*/
    cout << endl;
  //}
}
