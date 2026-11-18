#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>

int sem_check(ast_tree_t *tree) {
	(void)tree;
	return 0;
}

ast_node_t sem_check_node(ast_node_t *node){
	if(node == NULL){
        return;
    }
    
    switch(node->type){
        case AST_ASSIGN:
            sem_check_node(node->data.assign.dest);
            sem_check_node(node->data.assign.source);
            break;
        
        case AST_BINOP:
            sem_check_node(node->data.binop.left);
            sem_check_node(node->data.binop.right);
            break;

        case AST_BLOCK:
            sem_check_node(node->data.block.first_line);
            break;

        case AST_IF:
            sem_check_node(node->data.condicionals.cond);
            sem_check_node(node->data.condicionals.body);
            sem_check_node(node->data.condicionals.else_body);
            break;

        case AST_WHILE:
            sem_check_node(node->data.condicionals.cond);
            sem_check_node(node->data.condicionals.body);
            break;

        case AST_FUNC_DEF:
            sem_check_node(node->data.func.args);
            sem_check_node(node->data.func.body);
            break;

        case AST_CALL:
            sem_check_node(node->data.func.args);
            break;

        case AST_VAR_DEF:
            break;

        case AST_VAR:
            break;

        case AST_LITERAL:

            break;

        case AST_RETURN:
            sem_check_node(node->data.node_return.var);
            break;
    }
    sem_check_node(node->next);
}