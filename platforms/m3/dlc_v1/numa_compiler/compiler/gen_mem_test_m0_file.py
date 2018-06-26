#!/usr/bin/env python
from os import system
import sys

try:
    inst_no = int(sys.argv[1])
except:
    inst_no = 2
try:
    has_nli = int(sys.argv[2])
except:
    has_nli = 0
try:
    nli_no = int(sys.argv[3]) 
except:
    nli_no = 0   

#########
inst_file = open("new_dnn.inst.c", 'w')
header_file = open("dnn_parameters.h", 'w')
inst_file_old = open("dnn.inst.c", 'r')
if (has_nli):
  nli_file = open("new_dnn.nli.c", 'w')
  nli_file_old = open("dnn.nli.c", 'r')

#### dnn.inst.c
inst_file.write("#include \"dnn_parameters.h\"\n");
inst_file.write("uint32_t PE_INSTS[%d][4][6];\n" % inst_no);
inst_file.write("\n");
inst_file.write("void set_dnn_insts() {\n");
inst = -1
for line in inst_file_old: 
    if line.startswith("PE_INSTS[i][0][0]"):
      inst = 0
    elif line.startswith("PE_INSTS[i][1][0]"):
      inst = 1
    new_line = line.replace('i', str(inst)) 
#    print line, new_line
    inst_file.write(new_line)
inst_file.write("}\n");
inst_file.close()
inst_file_old.close()
system("mv new_dnn.inst.c dnn.inst.c") 

#### dnn.nli.c
if (has_nli):
  nli_file.write("#include \"dnn_parameters.h\"\n");
  nli_file.write("uint32_t NLI_X[%d][4][11];\n" % nli_no);	
  nli_file.write("uint32_t NLI_A[%d][4][10];\n" % nli_no);	
  nli_file.write("uint32_t NLI_B[%d][4][10];\n" % nli_no);	
  nli_file.write("\n");
  nli_file.write("void set_nli_parameters() {\n"); 
  nli = -1
  for line in nli_file_old:    
      if line.startswith("NLI_X[i][0][0]"):   ## new nli
          nli += 1
      new_line = line.replace('i', str(nli))
      nli_file.write(new_line)
  if (nli != nli_no - 1):
      print("error: nli no. does not match!!")
      sys.exit()
  nli_file.write("}\n");
  nli_file.close()
  nli_file_old.close()
  system("mv new_dnn.nli.c dnn.nli.c") 

#### dnn_parameters.h
header_file.write("#ifndef _DNN_PARAMETERS_H\n");
header_file.write("#define _DNN_PARAMETERS_H\n");
header_file.write("#include <stdint.h>\n");
header_file.write("extern uint32_t PE_INSTS[%d][4][6];\n" % inst_no);	## inst_no, PE_no, ADDR_no
header_file.write("void set_dnn_insts();\n");	
if (has_nli):
  header_file.write("#define HAS_NLI\n");
  header_file.write("extern uint32_t NLI_X[%d][4][11];\n" % nli_no);	## inst_no, PE_no, ADDR_no
  header_file.write("extern uint32_t NLI_A[%d][4][10];\n" % nli_no);	## inst_no, PE_no, ADDR_no
  header_file.write("extern uint32_t NLI_B[%d][4][10];\n" % nli_no);	## inst_no, PE_no, ADDR_no
  header_file.write("\n");	
header_file.write("void set_nli_parameters();\n");	
header_file.write("#endif\n");
