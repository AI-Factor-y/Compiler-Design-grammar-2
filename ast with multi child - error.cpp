#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* marcos for Tnode type */
#define NUMBER 0
#define SYMBOL 1
#define KEYWORD 2
#define OPERATOR 3
//------------------------+

#define MAX_CHILD 3

struct Tnode{
	int val;
	char varname[100]; // variable name , if the node is an identifier
					   // keyword , if the node is keyword
	int type;    // 0 -> number 1->symbol 2-> keyword 3-> operator
	int nodetype; // 0-> leaf 	1->non leaf
	struct Tnode *children[MAX_CHILD];
	int childCount;
};



struct Tnode* create_node(int val,char varname[]
						,int type){

	struct Tnode *newNode=(struct Tnode*)malloc(sizeof(struct Tnode));

	newNode->val=val;
	strcpy(newNode->varname,varname);
	newNode->type = type; // type 0 means an integer
	newNode->nodetype=0;  // new node is always a leaf
	newNode->childCount=0;

	for(int i=0;i<(MAX_CHILD);i++){
		newNode->children[i]=NULL;
	}

	return newNode;
}


struct Tnode* make_tree(char symbol[],int type,struct Tnode* left,
						struct Tnode *right){

	struct Tnode *newNode=(struct Tnode*)malloc(sizeof(struct Tnode));

	strcpy(newNode->varname,symbol);
	newNode->children[0]= left;
	newNode->children[1]= right;
	newNode->nodetype= 1;
	newNode->type= type; // type =1 means that this is always a symbol
						 // tyoe =2 means that this is a keyword
	newNode->childCount=2;
	return newNode;
}



void print_AST(struct Tnode *root){

	if(root==NULL) return;

	for(int i=0;i<(root->childCount)-1;i++){
		print_AST(root->children[i]);	
	}
	
	printf("[%d,%s]\n",root->val,root->varname);
}
