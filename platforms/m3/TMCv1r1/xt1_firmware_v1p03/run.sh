make clean
sed -i "s/\/\/#define DEVEL/#define DEVEL/" include/TMCv1r1.h
sed -i "s/#define __NFC_GPO__ 1/#define __NFC_GPO__ 0/" include/TMCv1r1.h
sed -i "s/#define __NFC_SCL__ 3/#define __NFC_SCL__ 1/" include/TMCv1r1.h
sed -i "s/#define __NFC_SDA__ 0/#define __NFC_SDA__ 3/" include/TMCv1r1.h
touch libs/TMCv1r1.c
make others
make *_devel
make tconv_test
make pmu_counter
sed -i "s/#define DEVEL/\/\/#define DEVEL/" include/TMCv1r1.h
touch libs/TMCv1r1.c
make *_nodevel
sed -i "s/#define __NFC_GPO__ 0/#define __NFC_GPO__ 1/" include/TMCv1r1.h
sed -i "s/#define __NFC_SCL__ 1/#define __NFC_SCL__ 3/" include/TMCv1r1.h
sed -i "s/#define __NFC_SDA__ 3/#define __NFC_SDA__ 0/" include/TMCv1r1.h
touch libs/TMCv1r1.c
make *_std
make cp
