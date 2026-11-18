#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>

void check_var(ast_node_t *node, tree_t *local_symtable, int *error) {
	char *var_name = node->data.variable.name;
	tdata_t *data = symtable_lookup(local_symtable, var_name);
	if (data == NULL) {
		*error = 3;
	} else {
		node->data.variable.type = data->data_type;
	}
}

ast_node_t *find_node_by_type(ast_node_t *node, ast_type_t type){

	if (!node) {
		return NULL;
	}

	if(node->type == type){
		return node;
	}

	ast_node_t *target_node = NULL;
		

	switch (node->type) {
		case AST_ASSIGN:
			target_node = find_node_by_type(node->data.assign.dest, type);
			if (target_node != NULL) return target_node;
			target_node = find_node_by_type(node->data.assign.source, type);
			break;

		case AST_BINOP:
			target_node = find_node_by_type(node->data.binop.left, type);
			if (target_node != NULL) return target_node;
			target_node = find_node_by_type(node->data.binop.right, type);
			break;

		case AST_BLOCK:
			target_node = find_node_by_type(node->data.block.first_line, type);
			break;

		case AST_IF:
			target_node = find_node_by_type(node->data.condicionals.cond, type);
			if (target_node != NULL) return target_node;
			target_node = find_node_by_type(node->data.condicionals.body, type);
			if (target_node != NULL) return target_node;
			target_node = find_node_by_type(node->data.condicionals.else_body, type);
			break;

		case AST_WHILE:
			target_node = find_node_by_type(node->data.condicionals.cond, type);
			if (target_node != NULL) return target_node;
			target_node = find_node_by_type(node->data.condicionals.body, type);
			break;

		case AST_FUNC_DEF:
			target_node = find_node_by_type(node->data.func.args, type);
			if (target_node != NULL) return target_node;
			target_node = find_node_by_type(node->data.func.body, type);
			break;

		case AST_CALL:
			target_node = find_node_by_type(node->data.func.args, type);
			break;

		case AST_VAR_DEF:
			break;

		case AST_VAR:
			break;

		case AST_LITERAL:
			break;

		case AST_RETURN:
			target_node = find_node_by_type(node->data.node_return.var, type);
			break;
	}
	if (target_node != NULL) {
        return target_node;
    }

	return find_node_by_type(node->next, type);
}

void sem_check_node(ast_node_t *node, tree_t *local_symtable, int *error) {
	if (node == NULL || *error != 0) {
		return;
	}

	switch (node->type) {
	case AST_ASSIGN:
		sem_check_node(node->data.assign.dest, local_symtable, error);
		sem_check_node(node->data.assign.source, local_symtable, error);
		break;

	case AST_BINOP:
		sem_check_node(node->data.binop.left, local_symtable, error);
		sem_check_node(node->data.binop.right, local_symtable, error);
		break;

	case AST_BLOCK:
		tree_t *block_symtable = node->data.block.symtable;
		sem_check_node(node->data.block.first_line, block_symtable, error);
		break;

	case AST_IF:
		sem_check_node(node->data.condicionals.cond, local_symtable, error);
		sem_check_node(node->data.condicionals.body, local_symtable, error);
		sem_check_node(node->data.condicionals.else_body, local_symtable,
					   error);
		break;

	case AST_WHILE:
		sem_check_node(node->data.condicionals.cond, local_symtable, error);
		sem_check_node(node->data.condicionals.body, local_symtable, error);
		break;

	case AST_FUNC_DEF:
		sem_check_node(node->data.func.args, local_symtable, error);
		sem_check_node(node->data.func.body, local_symtable, error);
		break;

	case AST_CALL:
		sem_check_node(node->data.func.args, local_symtable, error);
		break;

	case AST_VAR_DEF:
		break;

	case AST_VAR:
		check_var(node, local_symtable, error);
		break;

	case AST_LITERAL:

		break;

	case AST_RETURN:
		sem_check_node(node->data.node_return.var, local_symtable, error);
		break;
	}
	sem_check_node(node->next, local_symtable, error);
}

int sem_check(ast_tree_t *tree, tree_t *global_symtable) {
	int error = 0;

	sem_check_node(tree->root, global_symtable, &error);

	return error;
}
