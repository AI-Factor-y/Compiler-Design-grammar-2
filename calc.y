%{
	
	void yyerror (char *s);
	int yylex();
	#include <stdio.h>
	#include <stdlib.h>
	#include "codeGenerator.h"


%}

%union {int num;
		char *sym;
		struct Tnode *tnode;}
%start Program
%token FUN
%token <sym> VAR
%token <num> NUM
%token READ WRITE RETURN

/* condtional and loop tokens */
%token IF DO WHILE  
%token ELSE

/* relational tokens */
%token LessEqual GreaterEqual Equal NotEqual
%token Less Greater

/* types , all on the left side of the production */
%type <tnode> Program Stmts Stmt AsgStmt
%type <tnode> IfStmt IfElseStmt WhileStmt
%type <tnode> ReadStmt WriteStmt E RetStmt

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
			:ReadStmt VAR '=' FUN '(' E ')' '{' Stmts '}' WriteStmt	
													{ 	
													  print_AST($9);
													  printf("\n-------------------\n\n");

													  struct Tnode* fun_right = make_tree("FUN",FUN_KEYWORD,$6,NULL);

													  struct Tnode *AssgVar = create_node(-1,$2,SYMBOL);
													  
													  struct Tnode* fun_left = make_tree("=",ASSG_OPERATOR,AssgVar,fun_right);

													  main_setup($1,fun_left,$11,$9);
													}

// /* remove this rule -- test rule */
// pgm 
// 			: Stmts '|'								{ print_AST($1);
// 													  printf("\n-------------------\n\n");
// 													  code_genaration_main($1);
// 													}
// /* ================TEST========== */

Stmts		
			: Stmts Stmt   							{ $$ = make_tree("STMT",SYMBOL,$1,$2);
													 }					
			| Stmt 									{ $$ = $1;  }

Stmt 		
			: AsgStmt								{$$ = $1; }		
			| IfStmt 								{$$ = $1; }
			| IfElseStmt 							{$$ = $1; }
			| WhileStmt 							{$$ = $1; }
			| ReadStmt								{$$ = $1; }
			| WriteStmt 							{$$ = $1; }
			| RetStmt 	 							{$$ = $1; }

AsgStmt 	
			: VAR '=' E ';'							{ struct Tnode *AssgVar = create_node(-1,$1,SYMBOL);
													  $$ = make_tree("=",ASSG_OPERATOR,AssgVar,$3); }

IfStmt 		
			: IF '(' E ')' '{' Stmts '}'			{ $$ = make_tree("if",IF_KEYWORD,$3,$6); }

IfElseStmt 	
			: IF '(' E ')' '{' Stmts '}' ELSE '{' Stmts '}'  { $$ = make_if_else("ifElse",IF_ELSE_KEYWORD,$3,$6,$10); }

WhileStmt 	
			: DO '{' Stmts '}' WHILE '(' E ')' ';'  { $$ = make_tree("doWhile",DoWhile_KEYWORD,$7,$3); }

ReadStmt 	
			: READ '(' VAR ')' ';' 					{ 	struct Tnode *AssgVar = create_node(-1,$3,SYMBOL);
														$$ = make_tree("read",READ_KEYWORD,AssgVar,NULL);}
WriteStmt	
			: WRITE '(' E  ')' ';' 					{ $$ = make_tree("write",WRITE_KEYWORD,$3,NULL);}

RetStmt 	: RETURN E ';'							{ $$ =make_tree("return",RETURN_KEYWORD,$2,NULL);}

E 			
			: E '+' E 					{$$ = make_tree("+", ARITH_OPERATOR		 ,$1,$3); }
			| E '-' E 					{$$ = make_tree("-", ARITH_OPERATOR		 ,$1,$3); }
			| E '*' E 					{$$ = make_tree("*", ARITH_OPERATOR		 ,$1,$3); }
			| E Less E 					{$$ = make_tree("<", LESS_OPERATOR		 ,$1,$3); }
			| E Greater E				{$$ = make_tree(">", GREATER_OPERATOR	 ,$1,$3); }
			| E LessEqual E 			{$$ = make_tree("<=",LESS_EQ_OPERATOR	 ,$1,$3); }
			| E GreaterEqual E 			{$$ = make_tree(">=",GREATER_EQ_OPERATOR ,$1,$3); }
			| E Equal E 				{$$ = make_tree("==",EQUAL_OPERATOR 	 ,$1,$3); }
			| E NotEqual E 				{$$ = make_tree("!=",NOT_EQUAL_OPERATOR  ,$1,$3); }
			| '(' E ')'					{$$ = $2; }
			| NUM						{$$ = create_node($1,"NONE",NUMBER);  }
			| VAR						{$$ = create_node(-1,  $1  ,SYMBOL);  }
			| FUN '(' E ')'				{$$ = make_tree("FUN",FUN_KEYWORD,$3,NULL);}



%%

/* c code */



void yyerror(char *s){
	fprintf(stderr, "%s\n",s);
}


int main(void){

	return yyparse();
}