import yaml
import sys
import os

def check_elements(template, config, err_list, name):
    for k in template.keys():
        if k in config.keys():
            if(type(template[k]) != type(config[k])):
                err_list.append(['{}:{}'.format(name, k), 'type mismatch: {} != {}'.format(type(template[k]), type(config[k]))])
            if isinstance(template[k], dict):
                check_elements(template[k], config[k], err_list, '{}:{}'.format(name, k))
        else:
            err_list.append(['{}:{}'.format(name, k), 'template element not found'])



def check_template(filename):
    trigger_dir = filename
    ppath = os.path.abspath(sys.path[0]) + '/'
    out_dir = ppath + trigger_dir + '/'
    
    config_file = out_dir + 'trigger_configs.yaml'
    template_file = ppath + 'trigger_configs.yaml.template'
    
    err_list = []
    
    with open(config_file, 'r') as config_f:
        with open(template_file, 'r') as template_f:
            config = yaml.load(config_f, Loader=yaml.FullLoader)
            template = yaml.load(template_f, Loader=yaml.FullLoader)
    
    
            check_elements(template, config, err_list, '')
    
    if(len(err_list) == 0):
        print('Template check passed')
        return 0
    else:
        print('Found discrepancy between template and config files')
        for e in err_list:
            print('{}\t\t\t{}'.format(e[0], e[1]))
        return 1
