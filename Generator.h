#pragma once

#include "AbstractSyntaxTree.h"

void code_generator(struct Tnode *);
int evaluate_node(struct Tnode *root);

void do_assignment(struct Tnode *root);
void do_expression(struct Tnode* root);

void start_initial_setup();
void set_variable_start();

void assign_variable(char name[100],int val);
int find_variable(char name[100],int* addr);
int create_variable(char name[100]);
void Arith_operation(char operation[10]);

void read_to_memory(struct Tnode *root);
void write_from_memory(struct Tnode *root);

void exit_sequence();
void process_condition(struct Tnode *root);

void process_DoWhile(struct Tnode *root);
void Relation_operation(char operation[10]);

void Function_handler(struct Tnode *root);
void main_setup(struct Tnode *read_node, struct Tnode * fun_call,
					 struct Tnode *write_node, struct Tnode *fun_root);