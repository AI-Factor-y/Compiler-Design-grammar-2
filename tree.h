struct Tnode{
	int val;
	char op;
	int type; // 0-> val , 1->op
	struct Tnode *left,*right;
};


struct Tnode* create_node(int type,int val,char op){
		
	struct Tnode* new_node=(struct Tnode*)malloc(sizeof(struct Tnode));

	new_node->type=type;
	new_node->val=val;
	new_node->op =op;
	new_node->left=NULL;
	new_node->right=NULL;
	return new_node;
}

struct Tnode* make_tree(char op,
						struct Tnode* left,struct Tnode* right){
	
	struct Tnode* new_node=create_node(1,0,op);

	new_node->left=left;
	new_node->right=right;

	return new_node;

}

int evaluate_tree(struct Tnode* node){

	if(node->type==0){
		return node->val;
	}

	switch(node->op){

		case '+':
			return evaluate_tree(node->left) + evaluate_tree(node->right);

		case '-':
			return evaluate_tree(node->left) - evaluate_tree(node->right);


		case '*':
			return evaluate_tree(node->left) * evaluate_tree(node->right);


		case '/':
			return evaluate_tree(node->left) / evaluate_tree(node->right);

	}
	return 0;
}