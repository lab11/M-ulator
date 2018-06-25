#!/usr/bin/env bash

app=audio_fft
app=audio_m0_fft
app=audio_m0_fft_dft
app=audio_dft_fft
app=audio_m0_fft_fft
app=audio_m0_fft_combined_fft

gcc -o $app -g -std=gnu99 ${app}.c -lm || { exit 1; }
./$app &> ${app}.log
cat ${app}.log
