%{
	
	void yyerror (char *s);
	int yylex();
	#include <stdio.h>
	#include <stdlib.h>
	#include "tree.h"


%}

%union {int num;
		struct Tnode *tnode;}
%start Program
%token FUN VAR
%token <num> NUM
%token READ WRITE

/* condtional and loop tokens */
%token IF DO WHILE  
%token ELSE

/* relational tokens */
%token LessEqual GreaterEqual Equal NotEqual


/* types , all on the left side of the production */
%type <tnode> Program Stmts Stmt AsgStmt
%type <tnode> IfStmt IfElseStmt WhileStmt
%type <tnode> ReadStmt WriteStmt E

/* associativity specification */
%right '='
%left '+' '-'
%left '*'

/* everything that comes on the left side of
** the grammar is a type
** rest of it except literal tokens '+' or '*' are
** set as tokens
*/



%%


/* actual grammar productions start from here */

Program 	
			: FUN '(' ')' '{' Stmts '}'				

Stmts		
			: Stmts Stmt   												
			| Stmt 									

Stmt 		
			: AsgStmt								
			| IfStmt
			| IfElseStmt
			| WhileStmt
			| ReadStmt
			| WriteStmt

AsgStmt 	
			: VAR '=' E ';'							{ printf("result : %d\n",evaluate_tree($3)); }

IfStmt 		
			: IF '(' E ')' '{' Stmts '}'

IfElseStmt 	
			: IF '(' E ')' '{' Stmts '}' ELSE '{' Stmts '}'

WhileStmt 	
			: DO '{' Stmts '}' WHILE '(' E ')' ';'

ReadStmt 	
			: READ '(' VAR ')' ';' 
WriteStmt	
			: WRITE '(' E  ')' ';'

E 			
			: E '+' E 					{$$ = make_tree('+',$1,$3); printf("node connected\n");}
			| E '*' E 					{$$ = make_tree('*',$1,$3); printf("node connected\n");}
			| E LessEqual E 
			| E GreaterEqual E
			| E Equal E
			| E NotEqual E
			| '(' E ')'
			| NUM						{$$ = create_node(0,$1,0); printf("node created\n");}
			| VAR





%%

/* c code */



void yyerror(char *s){
	fprintf(stderr, "%s\n",s);
}


int main(void){

	return yyparse();
}