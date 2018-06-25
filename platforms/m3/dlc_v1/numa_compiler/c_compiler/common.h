////////////////////////////////////////////////////////
// common.h
// Common shared structures / types
////////////////////////////////////////////////////////

#ifndef __COMMON_H__
#define __COMMON_H__

#include <iostream>
#include <map>
#include <string>
#include <bitset>
#include <cmath>

using namespace std;

//////////////////////////////
template<typename T>
inline typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, std::string>::type
to_string(T const val) {
	return std::to_string(static_cast<long long>(val));
} 

//////////////////////////////
#define OPCODE_BIT 2
enum opcode {
    MOV = 0,
    MAC = 1,
    FFT = 2,
    NON = 3
}; 

/////// 2^4 = 16, 2^6 = 64, 2^8 = 256, 2^10 = 1kb, 2^13 = 1kB, 2^16 = 64kB, 2^18 = 256kB
/////// for 96 bits word, 2^4 = 1.5kb, 2^7 = 1.5kB, 2^13 = 96kB(1PE) 
#define PE_SRAM_ADDR_BIT 13   // 96kB
#define PE_SRAM_SIZE_BIT 13   // 128k (4b precision)
#define DSP_SRAM_ADDR_BIT 15
#define DSP_SRAM_SIZE_BIT 15

#define PRECISION_BIT 3
static map<int, int> precision_type = {{6, 0}, {8, 1}, {12, 2}, {16, 3}, {24, 4}, {32, 5}};
#define TEMPOUT_PRECISION_BIT 3
static map<int, int> tempout_precision_type = {{6, 0}, {8, 1}, {12, 2}, {16, 3}, {24, 4}, {32, 5}};

// FFT
/////// assuming max FFT: 4096 pts
#define FFT_PTS_BIT 4
static map<int, int> fft_pts_type = {{4, 0}, {8, 1}, {16, 2}, {32, 3}, {64, 4}, {128, 5}, {256, 6}, {512, 7},
                                     {1024, 8}, {2048, 9}, {4096, 10}, {8192, 11}};

// MAC
#define MAC_NEURON_SIZE_BIT 14
#define TILING_BIT 3
#define SHIFT_BIT 5

// Non-linear
//////////////////////////////
#define SHOW(a,b) cout << #b << " " << #a << ": " << (a) <<endl;
#ifdef DEBUG
#define SHOW_D(a,b) cout << #b << " " << #a << ": " << (a) <<endl;
#else
#define SHOW_D(a,b)  
#endif

#define WORD_SIZE m_config->word_size
#define TO_WORD_SIZE(a) (((a) + WORD_SIZE - 1) / WORD_SIZE)

string dec2bin(int dec);
string bin2hex(string bin);
string dec2bin(float dec, bool is_signed, int int_bit, int total_bit);

string binary_insert(string instruction, int data, int size);

int lcm(int a, int b);

#endif
