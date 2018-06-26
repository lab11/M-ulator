#include "fixed.h"
#include <math.h>

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

