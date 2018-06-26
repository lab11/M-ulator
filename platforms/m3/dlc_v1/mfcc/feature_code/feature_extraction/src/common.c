#include "common.h"

float float2fixed(float a, int int_bits_a, int float_bits_a) {
  if (a > pow(2, int_bits_a))
    return pow(2, int_bits_a);
  else if (a < -pow(2, int_bits_a))
    return -pow(2, int_bits_a);
  else {
    float minimum_precision = pow(2, -float_bits_a);
    return round(a / minimum_precision) * minimum_precision;
  }
}

float fixed_point_mul(float a, float b, int int_bits_a, int float_bits_a) {
   float fixed_a = float2fixed(a, int_bits_a, float_bits_a); 
//   printf("fixed_a%f, float_a:%f", fixed_a, a);
//   printf("fixed_b%f, float_b:%f", fixed_b, b);
   return (fixed_a * b);
}

float fixed_point_mul_ab(float a, float b, int int_bits_a, int int_bits_b, int float_bits_a, int float_bits_b) {
   float fixed_a = float2fixed(a, int_bits_a, float_bits_a); 
   float fixed_b = float2fixed(b, int_bits_b, float_bits_b); 
//   printf("fixed_a%f, float_a:%f", fixed_a, a);
//   printf("fixed_b%f, float_b:%f", fixed_b, b);
   return (fixed_a * fixed_b);
}

/*
string dec2bin(int dec){
  string result;
  if (dec <=1) return to_string(dec);
  result = to_string(dec % 2);
  result = dec2bin(dec / 2) + result;
//  cout << result << '\t' << dec << '\t' << endl;
  return result;
}

string dec2bin(int dec, bool is_signed, int total_bits){
  string result;
  if (dec < 0) 
    result = dec2bin(-dec);
  else 
    result = dec2bin(dec);
  if (is_signed) {
    while (result.size() != total_bits - 1) result = '0' + result;
    if (dec >= 0) 
      result = '0' + result;
    else {  // do 2's complement
      int i;
      for (i = 0; i < result.size(); i++) {
        if (result[i] == '0') 
          result[i] = '1';
        else if (result[i] == '1')
          result[i] = '0';
        else {
          cout << "error doing 2's complement" << endl;
          exit(1);
        }
      }
      for (i = result.size() - 1; i >= 0; i--) {
        if (result[i] == '0') {
          result[i] = '1';
          break;
        } else if (result[i] == '1') {
          result[i] = '0';
        } else {
          cout << "error doing 2's complement" << endl;
          exit(1);
        }
      }
      result = '1' + result;
    }
  } else { 
    while (result.size() != total_bits) result = '0' + result;
  }
//  cout << result << '\t' << dec << '\t' << endl;
  return result;
}

// www-inst.eecs.berkeley.edu/~cs61c/sp06/handout/fixedpt.html
string dec2bin(float dec, bool is_signed, int int_bits, int total_bits){
//  cout << "### dec2bin ###\t" << is_signed << "\t" << int_bits << "\t" << total_bits << endl;
//  cout << "dec:\t" << dec << endl;
  string result;
  int frac_bit = total_bits - int_bits - is_signed;
  int integer = int(dec);
  float fractional = dec - integer;
  // todo: is_signed and int_bit

  if ( (dec >= pow(2, int_bits)) || (dec <= -pow(2, int_bits)) ) {
    cout << "overflow during dec2bin" << endl;
    exit(1);
  }
  float number = dec * pow(2, frac_bit);
  result = dec2bin(round(number), is_signed, total_bits);

//  cout << result << '\t' << dec << '\t' << endl;
  return result;
}
*/ 
