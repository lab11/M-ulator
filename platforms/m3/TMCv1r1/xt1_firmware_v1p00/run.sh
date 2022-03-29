make clean
sed -i "s/\/\/#define DEVEL/#define DEVEL/" include/TMCv1r1.h
make *_devel
make tconv_test
sed -i "s/#define DEVEL/\/\/#define DEVEL/" include/TMCv1r1.h
make std
make *_std
make cp
