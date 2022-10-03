make clean
sed -i "s/\/\/#define DEVEL/#define DEVEL/" include/TMCv1r1.h
sed -i "s/#define __NFC_GPO__ 1/#define __NFC_GPO__ 0/" include/TMCv1r1.h
sed -i "s/#define __NFC_SCL__ 3/#define __NFC_SCL__ 1/" include/TMCv1r1.h
sed -i "s/#define __NFC_SDA__ 0/#define __NFC_SDA__ 3/" include/TMCv1r1.h
make clean_libs
make others
make *_devel
make tconv_test
make math_test
make eink_test
make sar_vs_vdd
sed -i "s/#define DEVEL/\/\/#define DEVEL/" include/TMCv1r1.h
make clean_libs
make *_nodevel
sed -i "s/#define __NFC_GPO__ 0/#define __NFC_GPO__ 1/" include/TMCv1r1.h
sed -i "s/#define __NFC_SCL__ 1/#define __NFC_SCL__ 3/" include/TMCv1r1.h
sed -i "s/#define __NFC_SDA__ 3/#define __NFC_SDA__ 0/" include/TMCv1r1.h
make clean_libs
make *_std
make pmu_counter
make pmu_counter_5p
make pmu_counter_6p
make cp
