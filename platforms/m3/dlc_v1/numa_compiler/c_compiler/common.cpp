#include "common.h"
#include <string>

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
    if (result.size() > total_bits - 1) {
      cout << "error!!" << endl;
      exit(1);
    }
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
          cout << "input:" << '\t' << dec << '\t' << "total_bits:" << '\t' << total_bits << endl;
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
          cout << "input:" << '\t' << dec << '\t' << "total_bits:" << '\t' << total_bits << endl;
          exit(1);
        }
      }
      result = '1' + result;
    }
  } else { 
    if (result.size() > total_bits) {
      cout << "error!! binary size larger than total bits" << endl;
      exit(1);
    }
    while (result.size() != total_bits) result = '0' + result;
  }
//  cout << result << '\t' << dec << '\t' << endl;
  return result;
}

// www-inst.eecs.berkeley.edu/~cs61c/sp06/handout/fixedpt.html
string dec2bin(float dec, bool is_signed, int int_bits, int total_bits){
  string result;
  int frac_bit = total_bits - int_bits - is_signed;
  int integer = int(dec);
  float fractional = dec - integer;

  float number = round(dec * pow(2, frac_bit));
  if ( (abs(number) >= pow(2, total_bits - is_signed)) ) {
    cout << "### overflow during dec2bin: floating point number is larger than expected; consider change integer bits ##" << endl;
    exit(0);
  }
  result = dec2bin(number, is_signed, total_bits);

//  cout << result << '\t' << dec << '\t' << endl;
  return result;
} 

string bin2hex(string bin) {
  string binary = bin;
  string result;
  while (binary.size() % 4 != 0)
    binary = '0' + binary;
  while (binary.size() >= 4) {
    switch(stoi(binary.substr(binary.size() - 4, 4))) {
      case 0 : result = '0' + result; break; 
      case 1 : result = '1' + result; break;
      case 10 : result = '2' + result; break;
      case 11 : result = '3' + result; break;
      case 100 : result = '4' + result; break;
      case 101 : result = '5' + result; break;
      case 110 : result = '6' + result; break;
      case 111 : result = '7' + result; break;
      case 1000 : result = '8' + result; break; 
      case 1001 : result = '9' + result; break;
      case 1010 : result = 'A' + result; break;
      case 1011 : result = 'B' + result; break;
      case 1100 : result = 'C' + result; break;
      case 1101 : result = 'D' + result; break;
      case 1110 : result = 'E' + result; break;
      case 1111 : result = 'F' + result; break;
      default: cout << "bin2hex error" << endl; exit(1); break;
    }
    binary = binary.substr(0, binary.size() - 4);
  }
  return result;
} 

string binary_insert(string instruction, int data, int size) {
  string binary = dec2bin(data);
  if (binary.size() > size) {
    cout << "binary_insert size error: " << data << '\t' << size << endl;
    exit(0);
  } 
  while (binary.size() != size) binary = '0' + binary;
  return binary + instruction;
}

int lcm(int a, int b){
	int x = a, y = b;
	while (x != y) {
		if (y == 0)
			break;

		if (x > y)
			x = x - y;
		else
			y = y - x;
	}
	return a * b / x;
}

