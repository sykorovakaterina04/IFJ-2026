#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>

int sem_check(ast_tree_t *tree) {
	(void)tree;
	return 0;
}

ast_node_t sem_check_node(ast_node_t *node, tree_t *local_symtable){
	if(node == NULL){
        return;
    }
    
    switch(node->type){
        case AST_ASSIGN:
            sem_check_node(node->data.assign.dest, local_symtable);
            sem_check_node(node->data.assign.source, local_symtable);
            break;
        
        case AST_BINOP:
            sem_check_node(node->data.binop.left, local_symtable);
            sem_check_node(node->data.binop.right, local_symtable);
            break;

        case AST_BLOCK:
            sem_check_node(node->data.block.first_line, local_symtable);
            break;

        case AST_IF:
            sem_check_node(node->data.condicionals.cond, local_symtable);
            sem_check_node(node->data.condicionals.body, local_symtable);
            sem_check_node(node->data.condicionals.else_body, local_symtable);
            break;

        case AST_WHILE:
            sem_check_node(node->data.condicionals.cond, local_symtable);
            sem_check_node(node->data.condicionals.body, local_symtable);
            break;

        case AST_FUNC_DEF:
            sem_check_node(node->data.func.args, local_symtable);
            sem_check_node(node->data.func.body, local_symtable);
            break;

        case AST_CALL:
            sem_check_node(node->data.func.args, local_symtable);
            break;

        case AST_VAR_DEF:
            break;

        case AST_VAR:
            break;

        case AST_LITERAL:

            break;

        case AST_RETURN:
            sem_check_node(node->data.node_return.var, local_symtable);
            break;
    }
    sem_check_node(node->next, local_symtable);
}