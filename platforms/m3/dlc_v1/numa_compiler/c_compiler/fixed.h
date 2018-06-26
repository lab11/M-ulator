#ifndef FIXED_H
#define FIXED_H

float float2fixed(float a, int int_bits_a, int float_bits_a);
float fixed_point_mul(float a, float b, int int_bits_a, int float_bits_a);
float fixed_point_mul_ab(float a, float b, int int_bits_a, int int_bits_b, int float_bits_a, int float_bits_b);

#endif 
