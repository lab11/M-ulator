#!/usr/bin/env bash

echo "0"
diff FFT_INIT_MEM_0.data FFT_0_MEM_0.testdata
echo "1"
diff FFT_1_MEM_0.testdata FFT_0_MEM_0.goldendata
echo "2"
diff FFT_2_MEM_0.testdata FFT_1_MEM_0.goldendata
echo "3"
diff FFT_3_MEM_0.testdata FFT_2_MEM_0.goldendata
echo "4"
diff FFT_4_MEM_0.testdata FFT_3_MEM_0.goldendata
echo "5"
diff FFT_5_MEM_0.testdata FFT_4_MEM_0.goldendata
echo "6"
diff FFT_6_MEM_0.testdata FFT_5_MEM_0.goldendata
echo "7"
diff FFT_7_MEM_0.testdata FFT_6_MEM_0.goldendata
