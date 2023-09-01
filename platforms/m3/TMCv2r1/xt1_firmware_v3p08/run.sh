#! /bin/bash
# exit when any command fails
set -e
## keep track of the last executed command
#trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG
## echo an error message before exiting
#trap 'echo "\"${last_command}\" command filed with exit code $?."' EXIT

# NOTE on arm-none-eabi-size (from Pat Pannuto)
#-- text: Fixed bits that do not change during execution. This includes compiled code as well as global constants (e.g. "DEADBEEF" would be in the text, as well as all of the body of `main`)
#-- data: Global variables that are not constant *and* are initialized with a value [your example does not have any of these]
#-- bss : Global variables that are not constant *and* are not initialized with a value (e.g. the `enumerated` variable _value_, the address of `enumerated` is in the text section)

# Main Body
make clean
sed -i "s/\/\/#define DEVEL/#define DEVEL/" include/TMCv2r1.h
sed -i "s/#define __NFC_GPO__ 1/#define __NFC_GPO__ 0/" include/TMCv2r1.h
sed -i "s/#define __NFC_SCL__ 3/#define __NFC_SCL__ 1/" include/TMCv2r1.h
sed -i "s/#define __NFC_SDA__ 0/#define __NFC_SDA__ 2/" include/TMCv2r1.h
make clean_libs
make others
make *_devel
sed -i "s/#define DEVEL/\/\/#define DEVEL/" include/TMCv2r1.h
make clean_libs
make *_nodevel
sed -i "s/#define __NFC_GPO__ 0/#define __NFC_GPO__ 1/" include/TMCv2r1.h
sed -i "s/#define __NFC_SCL__ 1/#define __NFC_SCL__ 3/" include/TMCv2r1.h
sed -i "s/#define __NFC_SDA__ 2/#define __NFC_SDA__ 0/" include/TMCv2r1.h
make clean_libs
make *_std
make *_std_short
echo ">>> *_devel.memmap"
tail -n 1 *_devel/*_devel.memmap
echo ">>> *_std.memmap"
tail -n 1 *_std/*_std.memmap
echo ">>> arm-none-eabi-size"
#arm-none-eabi-size *_devel/*_devel.elf *_std/*_std.elf # This is for RHEL6 machines
arm-linux-gnu-size *_devel/*_devel.elf *_std/*_std.elf  # This is for RHEL7 machines
make cp
