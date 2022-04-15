#include <stdio.h>
#include <stdlib.h>
#include "calc.h"

extern int yylex();
extern int yylineno;
extern char* yytext;


char *names[]={NULL,"NUM","SYM"};

int main(void){

	int ntoken,vtoken;
	ntoken =yylex();

	float a,b,res;
	char op;
	
	while(ntoken){

		if(ntoken==INVALID){
			printf("Invalid character , expected a number, got '%s'\n",yytext);
			return 1;
		}

		a=atof(yytext);

		if(yylex()!=SYM){
			printf("Syntax error in line %d : expected a symbol got '%s'\n",yylineno,yytext);
			return 1;
		}

		op=yytext[0]; // get the operator

		vtoken=yylex();

		if(vtoken!=NUM){
			printf("Syntax error , Expected a number , got '%s'\n",yytext); return 1;
		}

		b=atof(yytext);

		switch(op){

			case '+':
				res=a+b;
				break;

			case '-':
				res=a-b;
				break;

			case '*':
				res=a*b;
				break;

			case '/':
				res=a/b;
				break;
		}

		printf("Result = %f\n",res);

		ntoken=yylex();
	}

	return 0;
}