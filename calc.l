%{
	#include "y.tab.h"
	void yyerror(char *s);
	int yylex();
%}

/* while setting yylval  to string types
** remember to allocate seperate memory using 
** strdup
*/

%%


[0-9]+			{yylval.num = atof(yytext); return NUM; }
[ \t\n]			;
[-+=*;]			{return yytext[0];}
"("				{return yytext[0];}
")"				{return yytext[0];}
"{"				{return yytext[0];}
"}"				{return yytext[0];}

"|"				{return yytext[0];}


"FUN"			{return FUN;}
"read"			{return READ;}
"write"			{return WRITE;}
"return"		{return RETURN;}
"If"			{return IF;}
"do"			{return DO;}
"else"			{return ELSE;}
"while"			{return WHILE;}

"<="			{return LessEqual;}
">="			{return GreaterEqual;}
"=="			{return Equal;}
"!="			{return NotEqual;}
">"				{return Greater;}
"<"				{return Less;}

[a-zA-Z][a-z|A-Z|0-9]* {yylval.sym=strdup(yytext); return VAR;}

.				{ECHO; yyerror("unexpected character\n");}




%%

int yywrap(void){
	return 1;
}
