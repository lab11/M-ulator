#include "common.h"

#include "dnn_sim.h"
#include "config.h"
#include "option_parser.h"

#include <iostream>
#include <fstream>

static int sg_argc = 3;
static const char *sg_argv[] = {"", "-config", "dnn.config"};

dnn_config *m_dnn_config = NULL;
dnn_sim *m_dnn_sim = NULL;

int main(int argc, char** argv) {

  // Creat new configuartion options
  m_dnn_config = new dnn_config();

  // Parse configuration file 
  cout << "read config files..." << endl;
  option_parser_t opp = option_parser_create();
  m_dnn_config->reg_options(opp);
  option_parser_cmdline(opp, sg_argc, sg_argv);
//  option_parser_print(opp, stdout);
  m_dnn_config->word_size = 96;         // HARD CODED
  
  m_dnn_sim = new dnn_sim(m_dnn_config);
  m_dnn_sim->load_inputs(string(m_dnn_config->input_file));
  
  m_dnn_sim->print_stat();

  cout << "generate instruction file..." << endl;
  string instructions = m_dnn_sim->print_instructions();
  ofstream inst_file(string(m_dnn_config->inst_file).c_str());
  if (!inst_file.is_open()) {  
    cout << "error creating instruction file" << endl;
    exit(1);
  }
  inst_file << instructions;
  inst_file.close();

  cout << "generate binary memory file..." << endl;
  string memory = m_dnn_sim->print_memory();
  ofstream memory_file(string(m_dnn_config->memory_file).c_str());
  if (!memory_file.is_open()) {  
    cout << "error creating memory file" << endl;
    exit(1);
  }
  memory_file << memory;
  memory_file.close();

  cout << "generate test case binary memory file..." << endl;
  string test_case = m_dnn_sim->print_test_case();
  ofstream test_case_file(string(m_dnn_config->test_case_file).c_str());
  if (!test_case_file.is_open()) {  
    cout << "error creating test case file" << endl;
    exit(1);
  }
  test_case_file << test_case;
  test_case_file.close();

  option_parser_destroy(opp);
  delete m_dnn_sim;
  delete m_dnn_config;

  cout << "Done!" << endl;
}

