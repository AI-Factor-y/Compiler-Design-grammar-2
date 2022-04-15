#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* marcos for Tnode type */
#define NUMBER                                0
#define SYMBOL                                1
#define KEYWORD                               2
#define IF_KEYWORD                            3
#define IF_ELSE_KEYWORD                       4
#define ARITH_OPERATOR                        5
#define ASSG_OPERATOR                         6
#define GREATER_EQ_OPERATOR                   7
#define LESS_EQ_OPERATOR                      8
#define EQUAL_OPERATOR                        9
#define NOT_EQUAL_OPERATOR                    10
#define READ_KEYWORD                          11
#define WRITE_KEYWORD                         12
#define DoWhile_KEYWORD                       13
#define GREATER_OPERATOR                      14
#define LESS_OPERATOR                         15
#define FUN_KEYWORD                           16
#define RETURN_KEYWORD                        17

//------------------------+

struct Tnode{
	int val;
	char varname[100]; // variable name , if the node is an identifier
					   // keyword , if the node is keyword
	int type;     // Tnode type is based on the Tnode type macros

	int nodetype; // 0-> leaf 	1->non leaf
	struct Tnode *left,*right,*middle; // * middle is a pointer child specifically
										// for else part of if else conditions
};



struct Tnode* create_node(int val,char varname[]
						,int type){

	struct Tnode *newNode=(struct Tnode*)malloc(sizeof(struct Tnode));

	newNode->val=val;
	strcpy(newNode->varname,varname);
	newNode->type = type; // type is one of the Tnode type macros
	newNode->nodetype=0;  // new node is always a leaf
	newNode->left= NULL;
	newNode->right=NULL;
	newNode->middle=NULL;
	return newNode;
}


struct Tnode* make_tree(char symbol[],int type,struct Tnode* left,
						struct Tnode *right){

	struct Tnode *newNode=(struct Tnode*)malloc(sizeof(struct Tnode));

	strcpy(newNode->varname,symbol);
	newNode->left= left;
	newNode->right= right;
	newNode->nodetype= 1;
	newNode->type= type; // type =1 means that this is always a symbol
						 // tyoe =2 means that this is a keyword
	return newNode;
}

struct Tnode* make_if_else(char symbol[],int type,struct Tnode* left,
						struct Tnode *middle, struct Tnode *right){

	struct Tnode *newNode=(struct Tnode*)malloc(sizeof(struct Tnode));

	strcpy(newNode->varname,symbol);
	newNode->left= left;
	newNode->right= right;
	newNode->middle= middle;
	newNode->nodetype= 1;
	newNode->type= type; // type is a Tnode type macros
	return newNode;
}




void print_AST(struct Tnode *root){

	if(root==NULL) return;

	print_AST(root->left);	

	if(root->type== IF_ELSE_KEYWORD){
		print_AST(root->middle);
	}

	printf("[%d,%s]\n",root->val,root->varname);

	print_AST(root->right);

}
