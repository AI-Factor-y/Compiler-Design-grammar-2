#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "AbstractSyntaxTree.h"
#include "Generator.h"
#include "label_corrector.h"

// maximum number of variables allowed
#define  MAX_VARS            100 
// address from which we start storing variables
// stack space of 100 is allocated for variable name
// stack pointer shifted to 4196
#define  VAR_START_ADDR      4096
// the first var_start_Addr [4096] is used 
// to store the single argument passed to functions
#define  STACK_START         4196
#define  DEBUG               0

struct variableNames{
	char name[100];
	int addr;
};


int AVBLE_REG=3; // the currently available 
					  // least index register

int AVAILABLE_LABEL = 1; // the currently available 
			// label for if ,else statements

/*  
*	machanism of expression tree evaluation
*	push oparands into the stack and pop 2 operands
*	when we find an operation and push the result 
*	into the stack
*
*/


FILE *filePointer; // file pointer for writing into compiled file

char file_path[]="../xsm_expl/intermediate.xsm";


struct Variables{
	struct variableNames varname[MAX_VARS];
	int next_available_addr;
	int num_variables;
};


struct Variables Variable;


/* MAIN code translation starts from here */

/*--------- ALL MAIN DRIVER fUNCTIONS------------ */
/*================================================*/

void main_setup(struct Tnode *read_node, struct Tnode * fun_call,
					 struct Tnode *write_node, struct Tnode *fun_root){


	filePointer = fopen(file_path,"w");

	set_variable_start();

	start_initial_setup();

		evaluate_node(read_node); // does the read statement first

		evaluate_node(fun_call);  // makes the first main function call
										  // fun_call is a assignment type node 
										  // with left value as return variable, 
										  // right node as a function call

		// fprintf(filePointer,"--> write section start <--\n");
		evaluate_node(write_node); // does the write statement for the return value

	exit_sequence();


		fprintf(filePointer,"LABEL%d:MOV R0, R0\n",999); // create the header for a new function FUNCT

	/*-----FUNCTION DEFINITION-------*/

		code_generator(fun_root);

		fprintf(filePointer,"RET\n");

	/*===============================*/


	fclose(filePointer);

	replace_labels_with_address(file_path);
}


void code_genaration_main(struct Tnode *root){

	filePointer = fopen(file_path,"w");

	set_variable_start();

	start_initial_setup();

	code_generator(root);

	exit_sequence();

	fclose(filePointer);

	replace_labels_with_address(file_path);

}

/*-------------SET UP FUNCTIONS (exit,start etc )------------*/
/*===========================================================*/


void start_initial_setup(){

	fprintf(filePointer,"0\n2056\n");
	for(int i=0;i<6;i++){
		fprintf(filePointer,"0\n");
	}

	fprintf(filePointer,"MOV SP, %d\n",STACK_START);

}

// code to terminate the ExpOs program
void exit_sequence(){

	fprintf(filePointer,"MOV R0, 10\n");
	fprintf(filePointer,"PUSH R0\n");

	fprintf(filePointer,"PUSH R1\n");
	fprintf(filePointer,"PUSH R1\n");
	fprintf(filePointer,"PUSH R1\n");
	fprintf(filePointer,"PUSH R1\n");

	fprintf(filePointer,"INT 10\n"); // exit interrupt call

	fprintf(filePointer,"POP R1\n");
	fprintf(filePointer,"POP R1\n");
	fprintf(filePointer,"POP R1\n");
	fprintf(filePointer,"POP R1\n");
	fprintf(filePointer,"POP R0\n");


}

/*-------------VARIABLE MANAGEMENT fUNCTIONS (assign,retrieve, create )------------*/
/*=================================================================================*/


void assign_variable(char name[100],int value_Register){

	int addr=0;
	int found=find_variable(name,&addr);

	if(!found){
		// variable does not exist
		// create a new varaible
		addr=create_variable(name);
	}

	// the variable address is assigned a value
	// that is in the register `R{value_Register}`

	fprintf(filePointer,"MOV [%d], R%d\n",addr,value_Register);
}


/* find if a variable exists and get its addr
*  addr is given on to the reference parameter value
*  returns 0 if no such variable found 
*  returns 1 if a variable is found
**/
int find_variable(char name[100],int* addr){

	for(int i=0;i<Variable.num_variables;i++){
		if(strcmp(Variable.varname[i].name,name)==0){
			(*addr)=Variable.varname[i].addr;
			return 1;
		}
	}
	return 0;
}

int create_variable(char name[100]){

	int addr=Variable.next_available_addr;
	int i=Variable.num_variables;
	
	Variable.varname[i].addr=addr;
	strcpy(Variable.varname[i].name,name);

	Variable.num_variables++;
	Variable.next_available_addr++;

	return addr;

}


// set the variable initial address
void set_variable_start(){

	// the first variable address is used to store 
	// the argument to the function
	strcpy(Variable.varname[0].name,"argc");
	
	Variable.varname[0].addr=VAR_START_ADDR;

	Variable.next_available_addr= VAR_START_ADDR+1;
	Variable.num_variables=1;
}



/*-------------CODE GENRATION and EVALUATION-----------------*/
/*===========================================================*/



// go preorder and find the assembly for the 
// current node action
void code_generator(struct Tnode *root){

	if(root==NULL) return;

	int recur_status=evaluate_node(root);

	if(recur_status) return;

	code_generator(root->left);

	code_generator(root->right);

}



/*-------------EVALUATE NODE STMTs (read,write,assgn,if,ifelse,loops)------------*/
/*===============================================================================*/


/* returns 0 if we have to continue the recurive 
   preorder traversal
   returns 1 if we do not have to continue the recursive
   preorder traversal
  */
int evaluate_node(struct Tnode *root){

	int res;

	switch(root->type){

		case ASSG_OPERATOR:

			do_expression(root->right);

			do_assignment(root->left);

			return 1;

		case IF_KEYWORD:
			/* translate condition 
			* 	goto future label
			*	traslate statement code_gen(root->right)
			*	create label
			**/

			process_condition(root->left);	

			/* jump if the condition is false */
			int curr_label=AVAILABLE_LABEL;
			fprintf(filePointer,"JMP_AT_LABEL%d:JZ R0, \n",AVAILABLE_LABEL);
			AVAILABLE_LABEL++;

			code_generator(root->right);
			fprintf(filePointer,"LABEL%d:MOV R0,R0\n",curr_label);

			return 1;


		case IF_ELSE_KEYWORD:

			/* mechanism
			*  if condtion false: goto lb1 <goto else part>
			*  condition true :at the end of if goto lb 2 <skipping else part>
			* 	lb1 is the label for else part
			*	lb2 is the label after if-else section
			**/
			process_condition(root->left);

			/* jump if the condition is false, jump to else part */
			int else_label=AVAILABLE_LABEL;
			fprintf(filePointer,"JMP_AT_LABEL%d:JZ R0, \n",AVAILABLE_LABEL);
			AVAILABLE_LABEL++;

			code_generator(root->middle); // if part statements

			// if cond is true after <if> skip
			// the else part and continue excec
			int skip_else_label=AVAILABLE_LABEL;
			fprintf(filePointer,"JMP_AT_LABEL%d:JMP \n",AVAILABLE_LABEL);
			AVAILABLE_LABEL++;

			fprintf(filePointer,"LABEL%d:MOV R0,R0\n",else_label); // inserting dummy instruction <MOV R0, R0> to
																					// give a seprate line for the label

			// else part statements
			code_generator(root->right);

			// skip else label , statements after if else
			fprintf(filePointer,"LABEL%d:MOV R0,R0\n",skip_else_label);

			return 1;

		case DoWhile_KEYWORD:

			process_DoWhile(root);

			return 1;

		case READ_KEYWORD:
			read_to_memory(root);
			
			return 1;

		case WRITE_KEYWORD:	
			write_from_memory(root);
			
			return 1;

		case RETURN_KEYWORD:

			do_expression(root->left);
			// the result of the expression will be
			// pushed on to the stack

			// pop and store the result in register 3

			fprintf(filePointer,"POP R%d\n",AVBLE_REG);
			fprintf(filePointer,"RET\n");


			return 1;

	}


	return 0;
}

/* process the do while condition
*/
void process_DoWhile(struct Tnode *root){

	if(root==NULL) return;

	// create a label for the loop to come back 
	int curr_label=AVAILABLE_LABEL;

	fprintf(filePointer,"LABEL%d:MOV R0,R0\n",AVAILABLE_LABEL);
	AVAILABLE_LABEL++;

	// do while excecutes the 
	// code atleast once 
	// the condition check happens at the end
	code_generator(root->right);

	process_condition(root->left);

	fprintf(filePointer,"JMP_AT_LABEL%d:JNZ R0, \n",curr_label);

	
}



/* process the condition for if else statements
*  conditional statements
*  the condition result will be stored in 
*  register R0
*  1-> if true
**/
void process_condition(struct Tnode *root){


	if(root==NULL) return;

	do_expression(root);

	// store the result in the register R0
	fprintf(filePointer,"POP R0\n");


	/* if its a symbol or a number 
	* 	then take the number from the stack (now in R0)
	* 	check if the number is non zero or not
	* 	if == 0 then cond is false else true
	**/
	if(root->type==SYMBOL ||
		root->type==NUMBER){

		fprintf(filePointer,"MOV R1, 0\n");
		fprintf(filePointer,"NE R0, R1\n");

	}

	//**[ the result of the condition is stored in the register R0 ]**//
	
}


// read from standard input
void read_to_memory(struct Tnode *root){

	if(root==NULL) return;

	char var_name[100];
	strcpy(var_name,root->left->varname);

	int addr=0;
	int found=find_variable(var_name,&addr);

	if(!found){
		addr=create_variable(var_name);
	}

	fprintf(filePointer,"MOV R1, %d\n",7);
	fprintf(filePointer,"PUSH R1\n");

	fprintf(filePointer,"MOV R2, %d\n",-1);
	fprintf(filePointer,"PUSH R2\n");

	fprintf(filePointer,"MOV R0, %d\n",addr);
	fprintf(filePointer,"PUSH R0\n");
	
	fprintf(filePointer,"PUSH R3\n");
	fprintf(filePointer,"PUSH R4\n");

	fprintf(filePointer,"INT %d\n",6);

	fprintf(filePointer,"POP R4\n");
	fprintf(filePointer,"POP R3\n");
	fprintf(filePointer,"POP R0\n");
	fprintf(filePointer,"POP R2\n");
	fprintf(filePointer,"POP R1\n");


}


void write_from_memory(struct Tnode *root){

	if(root==NULL) return;

	// do the expression inside the write function call
	do_expression(root->left);

	/* expression result is stored in the
	*  stack ..pop and move it to R{AVAIABLE_REG} 
	**/

	fprintf(filePointer,"POP R%d\n",AVBLE_REG);

	/*  do the set up for print system call
	*  using interrupt number 7
	**/
	fprintf(filePointer,"MOV R1, %d\n",5);
	fprintf(filePointer,"PUSH R1\n");

	fprintf(filePointer,"MOV R2, %d\n",-2);
	fprintf(filePointer,"PUSH R2\n");

	// push the value to write to console
	// value is stored in R{AVBLE_REG}
	fprintf(filePointer,"PUSH R%d\n",AVBLE_REG);
	
	fprintf(filePointer,"PUSH R3\n");
	fprintf(filePointer,"PUSH R4\n");

	fprintf(filePointer,"INT %d\n",7);

	fprintf(filePointer,"POP R4\n");
	fprintf(filePointer,"POP R3\n");
	fprintf(filePointer,"POP R%d\n",AVBLE_REG);
	fprintf(filePointer,"POP R2\n");
	fprintf(filePointer,"POP R1\n");

}



// do the variable assignment
void do_assignment(struct Tnode *root){

	if(root==NULL) return;
	fprintf(filePointer,"POP R%d\n",AVBLE_REG);
	assign_variable(root->varname,AVBLE_REG);

}


/*-------------Expression Evaluation functions (+,-,FUN,*,NUM,VAR, etc )------------*/
/*==================================================================================*/



// do the expression to the right of assignment operator
// do expression will push 1 value into the stack 
// which is the final result of the operation
void do_expression(struct Tnode* root){
	if(root==NULL) return;

	if(root->type == NUMBER){
		if(DEBUG){
			fprintf(filePointer,"NUMBER : %d\n",root->val);	
		}
		
		fprintf(filePointer,"MOV  R%d, %d\n",AVBLE_REG,root->val);
		fprintf(filePointer,"PUSH R%d\n",AVBLE_REG);

	}else if(root->type == SYMBOL){
		if(DEBUG){
			fprintf(filePointer,"SYMBOL : %s\n",root->varname);
		}

		int addr=0;
		int found=find_variable(root->varname,&addr);

		if(!found){
			printf("[ERROR] variable %s was not declared\n",root->varname);
			exit(0);
		}

		fprintf(filePointer,"MOV R%d, [%d]\n",AVBLE_REG, addr);
		fprintf(filePointer,"PUSH R%d\n",AVBLE_REG);

	}else if(root->type == ARITH_OPERATOR){

		do_expression(root->left);
		do_expression(root->right);

		if(root->varname[0]=='+'){	
			Arith_operation("ADD");
		}else if(root->varname[0]=='*'){
			Arith_operation("MUL");
		}else if(root->varname[0]=='-'){
			Arith_operation("SUB");
		}

	}else if(root->type == LESS_EQ_OPERATOR){

		do_expression(root->left);
		do_expression(root->right);

		Relation_operation("LE");

	}else if(root->type == GREATER_EQ_OPERATOR){

		do_expression(root->left);
		do_expression(root->right);

		Relation_operation("GE");

	}else if(root->type == EQUAL_OPERATOR){

		do_expression(root->left);
		do_expression(root->right);

		Relation_operation("EQ");

	}else if(root->type == NOT_EQUAL_OPERATOR){

		do_expression(root->left);
		do_expression(root->right);

		Relation_operation("NE");
	}else if(root->type == GREATER_OPERATOR){

		do_expression(root->left);
		do_expression(root->right);

		Relation_operation("GT");
	}else if(root->type == LESS_OPERATOR){

		do_expression(root->left);
		do_expression(root->right);

		Relation_operation("LT");
	
	}else if(root->type == FUN_KEYWORD){

		Function_handler(root);
	}



}


void Arith_operation(char operation[10]){

	fprintf(filePointer,"POP R%d\n",AVBLE_REG+1);
	fprintf(filePointer,"POP R%d\n",AVBLE_REG);
	
	fprintf(filePointer,"%s R%d, R%d\n",operation,AVBLE_REG,AVBLE_REG+1);

	fprintf(filePointer,"PUSH R%d\n",AVBLE_REG);
}


void Relation_operation(char operation[10]){
	fprintf(filePointer,"POP R%d\n",AVBLE_REG+1);
	fprintf(filePointer,"POP R%d\n",AVBLE_REG);
	
	fprintf(filePointer,"%s R%d, R%d\n",operation,AVBLE_REG,AVBLE_REG+1);

	fprintf(filePointer,"PUSH R%d\n",AVBLE_REG);
}

//----------CALLING A FUNCTION------------------------//
//====================================================//



void Function_handler(struct Tnode *root){


	// evaluate the arg expression first so that 
	// the variables are available.
	// after this all current local variables will be destroyed
	do_expression(root->left);
	// the result of the expression is already pushed on to the stack
	fprintf(filePointer,"POP R0\n"); // pop and store the evaluation result in R0


	// push all the variables of the 
	// current function into the stack before 
	// executing the function call

	for(int i=Variable.num_variables-1;i>=0;i--){
		fprintf(filePointer,"MOV R1, [%d]\n",Variable.varname[i].addr);
		fprintf(filePointer,"PUSH R1\n");
	}

	// for storing the argument
	// to the function

	// the 0th address variable is an special variable 
	// used to store the single argument
	fprintf(filePointer,"MOV [%d], R0\n",Variable.varname[0].addr);

	// 999 is a special LABEL used for denoting function
	fprintf(filePointer,"JMP_AT_LABEL%d:CALL \n",999); // call function


	// restore the previous variables from the stack after the call

	int i;
	for(i=0;i<Variable.num_variables;i++){
		fprintf(filePointer,"POP R1\n");
		fprintf(filePointer,"MOV [%d], R1\n",VAR_START_ADDR+i);
	}

	// the return value was stored in the registed R{AVBLE} which is 
	// currently R3 . 
	// this is now fed to the assignment statement 
	// ret_val = FUN (x) ;
	// here the assignment evaluate the return value under the assumption 
	// that the result of RHS is in the stack ( as assignment treans function call as an expression)
	// so push the return res back into the stack

	fprintf(filePointer,"PUSH R%d\n",AVBLE_REG);

}
