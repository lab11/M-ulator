#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <vector>

#include "fixed.h"
#include "common.h"
#define MAX_INPUTS 15000

#define SEGMENTS 10
 
using namespace std;
///
int function_switch = 2;
///

///////////////////////
vector<array<float, SEGMENTS+1>> get_bounary_list(int &bounary_num){
  vector<array<float, SEGMENTS+1>> bounary_list;
  array<float, SEGMENTS+1> bounary_tmp;

  switch (function_switch) {
    case 0: // sigmoid 
      bounary_tmp = {-7.5, -2, -1, -.5, -.125, 0, .125, .5, 1, 2, 7.5};
      bounary_list.push_back(bounary_tmp);
    break;
    case 1: // tanh
      bounary_tmp = {-7.5, -2, -1, -.5, -.125, 0, .125, .5, 1, 2, 7.5};
      bounary_list.push_back(bounary_tmp);
    break;
    case 2: // log
      for (float i = 1; i < 2; i += 0.1) {
        bounary_tmp = {-63, 0, 0, pow(2, -9), pow(2, -6), pow(2, -4) * i, .125, .25, 1, 63}; 
        bounary_list.push_back(bounary_tmp);
      }
    break;
  }

  bounary_num = bounary_list.size();
  return bounary_list;
}

float function(float a) {
  switch (function_switch) {
    case 0: // sigmoid 
      return 1. / (1. + exp(a));
    break;
    case 1: // tanh 
      return tanh(a);
    break;
    case 2: // log 
      return (a > 0) ? log(a) : 0;
    break;
  }
}

/////////////////////
float* get_inputs(int &inputs_num, array<float,SEGMENTS+1> bounary){
  // sigmoid
  inputs_num = 10 * 10;
  float* inputs = new float[inputs_num];
  for (int i = 0; i < 10; i++){
    float lower_bounary = bounary[i];
    float step = (bounary[i + 1] - bounary[i]) / 10;
    for (int j = 0; j < 10; j++){
      inputs[i * 10 + j] = lower_bounary + step * j;
    }
  } 
  return inputs;
};

float* get_inputs(int &inputs_num, string filename){
  // check file
  ifstream file(filename.c_str());
  if (!file.is_open()) {  
    cout << "error reading input file " << file << endl;
    exit(1);
  }

  float inputs[MAX_INPUTS];
  int i = 0;
  while (!file.eof() && (i < MAX_INPUTS))
    file >> inputs[i++];
  inputs_num = i;
  float* result = new float[inputs_num];
  for (int j = 0; j < inputs_num; j++)
    result[j] = inputs[j];
  return result;
}


float get_coeff(float a[SEGMENTS], float b[SEGMENTS], array<float,SEGMENTS+1> bounary) {
  for (int i = 0; i < SEGMENTS; i++){
    float x0 = bounary[i];
    float x1 = bounary[i+1];
    float y0 = function(x0); 
    float y1 = function(x1); 
    a[i] = (y1 - y0) / (x1 - x0);
    b[i] = y0;
  }
  if (function_switch == 2) { // log
    a[0] = 0; b[0] = 0;
    a[1] = 0; b[1] = 0;
    a[2] = 0; b[2] = -2.701;
  }
/*
  cout << "final coeff\n";
  for (int i = 0; i < SEGMENTS; i++)
    cout << setprecision(3) << a[i] << '\t' << b[i] << endl;
*/
};

float get_error(float inputs[], int inputs_num, float a[], float b[], array<float, SEGMENTS+1> bounary) {
  float total_error = 0; 
  for (int i = 0; i < inputs_num; i++) {
    float exact = function(inputs[i]);
    float interpolated;  
    for (int j = 0; j < SEGMENTS; j++) {
      if (inputs[i] < bounary[j + 1]) {
        interpolated = a[j] * (inputs[i] - bounary[j]) + b[j];
        break;
      }
    }
    float error = abs((interpolated - exact) / exact);
    if (!isnan(error) && !isinf(error))
      total_error += error;
//    cout << "input:\t" << inputs[i] << "\texact:\t" << exact << "\tinterpolated:\t" << interpolated << "\terror:\t" << error << '\n';
  }
  return total_error / inputs_num;
}; 

float print_to_memory(string filename, float a[], float b[], array<float, SEGMENTS+1>bounary){
  ofstream out_file(filename.c_str());
  if (!out_file.is_open()) {  
    cout << "error creating file" << endl;
    exit(1);
  }
  // bounary
  cout << "bounary:\n";
  out_file << "bounary:\n";
  for (int i = 0; i < SEGMENTS + 1; i++) {
    cout << bounary[i] << '\t'; 
    cout << dec2bin(bounary[i], 1, 6, 16) << '\n';  // signed, 6 int, 16 total
    out_file << dec2bin(bounary[i], 1, 6, 16) << '\t';  // signed, 6 int, 16 total
  }
  // a
  cout << "\na:\n";
  out_file << "\na:\n";
  for (int i = 0; i < SEGMENTS; i++) {
    cout << a[i] << '\t'; 
    cout << dec2bin(a[i], 1, 8, 16) << '\n';  // signed, 4 int, 16 total
    out_file << dec2bin(a[i], 1, 8, 16) << '\t';  // signed, 4 int, 16 total
  }
  // b
  cout << "\nb:\n";
  out_file << "\nb:\n";
  for (int i = 0; i < SEGMENTS; i++) {
    cout << b[i] << '\t'; 
    cout << dec2bin(b[i], 1, 8, 32) << '\n';  // signed, 4 int, 32 total
    out_file << dec2bin(b[i], 1, 8, 32) << '\t';  // signed, 4 int, 32 total
  }
  cout << endl; 
}

//////////
int main(int argc, char** argv){
  float a[SEGMENTS], b[SEGMENTS];

  int bounary_num;
  vector<array<float, SEGMENTS+1>> bounary_list = get_bounary_list(bounary_num);
  float* errors = new float[bounary_num];
  float* inputs;
  int best_bounary = 0; 
  float least_error = 1;

  int inputs_num;
  if (function_switch == 2) // log
    inputs = get_inputs(inputs_num, "log.input");

  for (int i = 0; i < bounary_num; i++) {
    get_coeff(a, b, bounary_list[i]);
    if (function_switch != 2) // not log
      inputs = get_inputs(inputs_num, bounary_list[i]);
    errors[i] = get_error(inputs, inputs_num, a, b, bounary_list[i]);
    cout << "error:" << errors[i] << '\n';
    if (errors[i] < least_error) {
      best_bounary = i;
      least_error = errors[i];
    }
  }

  get_coeff(a, b, bounary_list[best_bounary]);
  print_to_memory("non_linear_parameters", a, b, bounary_list[best_bounary]);

//  delete[] inputs;
  delete[] errors;
}
