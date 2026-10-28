#ifndef PARSER_H
#define PARSER_H

int parse();

int parse_function(tree_t *global_symtable){}
int parse_main(){}

int parse_statement(){}
int parse_math(){}


void write_to_sym(){} // na zapisovani promennych do symtable
void send_to_code(){} // na posilani do generovani kodu

#endif 