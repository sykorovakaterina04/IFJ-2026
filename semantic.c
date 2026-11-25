#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>

void sem_check_node(ast_node_t *node, tree_t *local_symtable, int *error);

bool is_numeric(data_type_t type) {
	return (type == TYPE_INT || type == TYPE_DOUBLE);
}

bool is_relational_op(binop_type_t type) {
	return (type == EQ || type == NEQ || type == LT || type == GT ||
			type == LEQ || type == GEQ);
}

void convert_int_to_double(ast_node_t *operand) {
	if (operand == NULL) {
		return;
	}
	if (operand->type == AST_VAR && operand->data.variable.type == TYPE_INT) {
		operand->data.variable.conversion_type = TYPE_DOUBLE;
	} else if (operand->type == AST_LITERAL &&
			   operand->data.literal.type == TYPE_INT) {
		operand->data.literal.conversion_type = TYPE_DOUBLE;
	}
}

void check_binop(ast_node_t *node, tree_t *local_symtable, int *error) {
	if (node == NULL || error == NULL || *error != 0) {
		return;
	}

	sem_check_node(node->data.binop.left, local_symtable, error);
	sem_check_node(node->data.binop.right, local_symtable, error);
	if (*error != 0) {
		return;
	}

	data_type_t left_type = node->data.binop.left->return_type;
	data_type_t right_type = node->data.binop.right->return_type;

	if (node->data.binop.type == ADD) {
		if (left_type == TYPE_INT && right_type == TYPE_INT) {
			node->return_type = TYPE_INT;
		} else if (is_numeric(left_type) && is_numeric(right_type)) {
			node->return_type = TYPE_DOUBLE;
			convert_int_to_double(node->data.binop.left);
			convert_int_to_double(node->data.binop.right);
		} else if (left_type == TYPE_STRING && right_type == TYPE_STRING) {
			node->return_type = TYPE_STRING;
		} else {
			*error = 6;
		}
	} else if (node->data.binop.type == SUB || node->data.binop.type == MUL ||
			   node->data.binop.type == DIV) {
		if (left_type == TYPE_INT && right_type == TYPE_INT) {
			node->return_type = TYPE_INT;
			if (node->data.binop.type == DIV) {
				node->data.binop.type = IDIV;
			}
		} else if (is_numeric(left_type) && is_numeric(right_type)) {
			node->return_type = TYPE_DOUBLE;
			convert_int_to_double(node->data.binop.left);
			convert_int_to_double(node->data.binop.right);
		} else {
			*error = 6;
		}
	} else if (node->data.binop.type == IDIV) {
		if (left_type == TYPE_INT && right_type == TYPE_INT) {
			node->return_type = TYPE_INT;
		} else {
			*error = 6;
		}
	} else if (is_relational_op(node->data.binop.type)) {
		if (is_numeric(left_type) && is_numeric(right_type)) {
			convert_int_to_double(node->data.binop.left);
			convert_int_to_double(node->data.binop.right);
			node->return_type = TYPE_BOOL;
		} else if (left_type == TYPE_STRING && right_type == TYPE_STRING) {
			node->return_type = TYPE_BOOL;
		} else {
			*error = 6;
		}
	} else {
		*error = 6;
	}
}

void check_var(ast_node_t *node, tree_t *local_symtable, int *error) {
	if (node == NULL || error == NULL || *error != 0) {
		return;
	}

	char *var_name = node->data.variable.name;
	if (var_name == NULL) {
		*error = 99;
		return;
	}

	tdata_t *data = symtable_lookup(local_symtable, var_name);
	if (data == NULL || data->type == FUNCTION) {
		*error = 3;
		return;
	}
	node->data.variable.type = data->data_type;
	node->return_type = data->data_type;

	if (node->return_type == TYPE_ANY) {
		*error = 6;
	}
}

ast_node_t *find_node(ast_node_t *node, ast_type_t type, char *func_name,
					  char *var_name) {

	if (!node) {
		return NULL;
	}

	if (type == AST_NO_TYPE || node->type == type) {
		if (func_name != NULL) {
			if ((node->type == AST_FUNC_DEF || node->type == AST_CALL) &&
				node->data.func.name != NULL &&
				strcmp(node->data.func.name, func_name) == 0) {
				return node;
			}
		} else if (var_name != NULL) {
			if ((node->type == AST_VAR_DEF || node->type == AST_VAR) &&
				node->data.variable.name != NULL &&
				strcmp(node->data.variable.name, var_name) == 0) {
				return node;
			}
		} else if (type != AST_NO_TYPE) {
			if (node->type == type) {
				return node;
			}
		}
	}

	ast_node_t *target_node = NULL;

	switch (node->type) {
	case AST_ASSIGN:
		target_node =
			find_node(node->data.assign.dest, type, func_name, var_name);
		if (target_node != NULL)
			return target_node;
		target_node =
			find_node(node->data.assign.source, type, func_name, var_name);
		break;

	case AST_BINOP:
		target_node =
			find_node(node->data.binop.left, type, func_name, var_name);
		if (target_node != NULL)
			return target_node;
		target_node =
			find_node(node->data.binop.right, type, func_name, var_name);
		break;

	case AST_BLOCK:
		target_node =
			find_node(node->data.block.first_line, type, func_name, var_name);
		break;

	case AST_IF:
		target_node =
			find_node(node->data.condicionals.cond, type, func_name, var_name);
		if (target_node != NULL)
			return target_node;
		target_node =
			find_node(node->data.condicionals.body, type, func_name, var_name);
		if (target_node != NULL)
			return target_node;
		target_node = find_node(node->data.condicionals.else_body, type,
								func_name, var_name);
		break;

	case AST_WHILE:
		target_node =
			find_node(node->data.condicionals.cond, type, func_name, var_name);
		if (target_node != NULL)
			return target_node;
		target_node =
			find_node(node->data.condicionals.body, type, func_name, var_name);
		break;

	case AST_FUNC_DEF:
		target_node =
			find_node(node->data.func.args, type, func_name, var_name);
		if (target_node != NULL)
			return target_node;
		target_node =
			find_node(node->data.func.body, type, func_name, var_name);
		break;

	case AST_CALL:
		target_node =
			find_node(node->data.func.args, type, func_name, var_name);
		break;

	case AST_VAR_DEF:
		break;

	case AST_VAR:
		break;

	case AST_LITERAL:
		break;

	case AST_RETURN:
		target_node =
			find_node(node->data.node_return.var, type, func_name, var_name);
		break;

	case AST_NO_TYPE:
		break;
	}
	if (target_node != NULL) {
		return target_node;
	}

	return find_node(node->next, type, func_name, var_name);
}

bool branch_unit(ast_node_t *node) {
	if (node != NULL && node->type == AST_BLOCK) {
		node = node->data.block.first_line;
	}

	if (node == NULL) {
		return true;
	}

	while (node->next != NULL) {
		node = node->next;
	}

	switch (node->type) {
	case AST_IF:
		return branch_unit(node->data.condicionals.body) &&
			   branch_unit(node->data.condicionals.else_body);

	case AST_ASSIGN:
	case AST_VAR_DEF:
	case AST_WHILE:
		return true;

	case AST_CALL:
	case AST_LITERAL:
		return (node->return_type == TYPE_UNIT);

	default:
		return false;
	}
}

bool branch_return(ast_node_t *node, data_type_t expected) {
	if (node != NULL && node->type == AST_BLOCK) {
		node = node->data.block.first_line;
	}

	if (node == NULL) {
		return false;
	}

	while (node->next != NULL) {
		node = node->next;
	}

	if (node->type == AST_RETURN) {
		if (node->data.node_return.var == NULL) {
			return false;
		}
		data_type_t type = node->data.node_return.var->return_type;
		return (type == expected) ||
			   (expected == TYPE_DOUBLE && type == TYPE_INT);
	}
	if (node->type == AST_IF) {
		return branch_return(node->data.condicionals.body, expected) &&
			   branch_return(node->data.condicionals.else_body, expected);
	}
	return false;
}

void check_func_2(ast_node_t *node, tree_t *local_symtable, int *error) {
	if (node == NULL)
		return;

	ast_node_t *first_line = node->data.func.body->data.block.first_line;

	if (first_line == NULL) {
		if(node->return_type != TYPE_UNIT && node->return_type != TYPE_ANY){
			*error = 6;
		} else {
			node->return_type = TYPE_UNIT;
			tdata_t *data = symtable_lookup(local_symtable, node->data.func.name);
			if(data != NULL){
				data->data_type = TYPE_UNIT;
			}
		}
		return;
	}

	ast_node_t *last_line = first_line;
	data_type_t type;

	while (last_line->next != NULL) {
		last_line = last_line->next;
	}

	if (node->return_type == TYPE_ANY) {
		if (last_line->type == AST_ASSIGN || last_line->type == AST_FUNC_DEF ||
			last_line->type == AST_VAR_DEF || last_line->type == AST_WHILE) {
			type = TYPE_UNIT;
		}

		if (last_line->type == AST_IF) {
			if (branch_unit(last_line)) {
				type = TYPE_UNIT;
			} else {
				*error = 6;
			}
		}

		if (last_line->type == AST_VAR || last_line->type == AST_LITERAL ||
			last_line->type == AST_BINOP || last_line->type == AST_CALL) {
			type = last_line->return_type;
		}

		if (type == TYPE_ANY || type == TYPE_BOOL) {
			*error = 6;
		}

		node->return_type = type;

		tdata_t *data = symtable_lookup(local_symtable, node->data.func.name);
		if (data != NULL) {
			data->data_type = type;
		}
	} else if (node->return_type != TYPE_UNIT){
		if(last_line->type == AST_RETURN){
			if(last_line->data.node_return.var == NULL){
				*error = 6;
				return;
			}
			type = last_line->data.node_return.var->return_type;
			if(node->return_type == TYPE_DOUBLE && type == TYPE_INT){
				convert_int_to_double(last_line->data.node_return.var);
			} else if (type != node->return_type){
				*error = 6;
				return;
			}
				
			
		}

		else if (last_line->type == AST_VAR || last_line->type == AST_LITERAL || last_line->type == AST_BINOP || last_line->type == AST_CALL){
			if(node->return_type == TYPE_DOUBLE && last_line->return_type == TYPE_INT){
				convert_int_to_double(last_line);
			} else if (last_line->return_type != node->return_type){
				*error = 6;
				return;
			}
		}

		else if (last_line->type == AST_IF){
			if(!branch_return(last_line, node->return_type)){
				*error = 6;
				return;
			}
		}

		else {
			*error = 6;
			return;
		}
	}
}

void check_func_1(ast_node_t *node, int *error) {
	if (node == NULL || *error != 0) {
		return;
	}

	bool is_recursive = (find_node(node->data.func.body, AST_CALL,
								   node->data.func.name, NULL) != NULL);
	bool has_return =
		(find_node(node->data.func.body, AST_RETURN, NULL, NULL) != NULL);

	if (node->return_type == TYPE_ANY && (is_recursive || has_return)) {
		*error = 6;
		return;
	}
}

void check_if(ast_node_t *node, tree_t *local_symtable, int *error) {
	if (node == NULL || error == NULL || *error != 0) {
		return;
	}

	if (node->data.condicionals.cond == NULL) {
		*error = 99;
		return;
	}
	sem_check_node(node->data.condicionals.cond, local_symtable, error);
	if (*error != 0) {
		return;
	}
	if (node->data.condicionals.cond->return_type != TYPE_BOOL) {
		*error = 6;
		return;
	}

	if (node->data.condicionals.body == NULL) {
		*error = 99;
		return;
	} else {
		sem_check_node(node->data.condicionals.body, local_symtable, error);
	}
	if (*error != 0) {
		return;
	}

	if (*error == 0) {
		if (node->data.condicionals.else_body != NULL) {
			sem_check_node(node->data.condicionals.else_body, local_symtable,
						   error);
		}
	}
}

void check_while(ast_node_t *node, tree_t *local_symtable, int *error) {
	if (node == NULL || error == NULL || *error != 0) {
		return;
	}

	if (node->data.condicionals.cond == NULL) {
		*error = 99;
		return;
	}
	sem_check_node(node->data.condicionals.cond, local_symtable, error);
	if (*error != 0) {
		return;
	}
	if (node->data.condicionals.cond->return_type != TYPE_BOOL) {
		*error = 6;
		return;
	}

	if (node->data.condicionals.body == NULL) {
		*error = 99;
		return;
	} else {
		sem_check_node(node->data.condicionals.body, local_symtable, error);
	}
	if (*error != 0) {
		return;
	}
}

void check_assign(ast_node_t *node, tree_t *local_symtable, int *error) {
	if (node == NULL || error == NULL || *error != 0) {
		return;
	}

	ast_node_t *source = node->data.assign.source;
	ast_node_t *dest = node->data.assign.dest;
	if (source == NULL || dest == NULL) {
		*error = 99;
		return;
	}

	sem_check_node(source, local_symtable, error);
	if (*error != 0) {
		return;
	}
	sem_check_node(dest, local_symtable, error);
	if (*error != 0) {
		return;
	}

	if (dest->type != AST_VAR) {
		*error = 99;
		return;
	} else {
		tdata_t *var =
			symtable_lookup(local_symtable, dest->data.variable.name);
		if (var == NULL) {
			*error = 3;
			return;
		} 
	}

	data_type_t dest_type = dest->return_type;
	data_type_t source_type = source->return_type;

	if (dest_type == source_type || source_type == TYPE_ANY) {
		return;
	} else if (source_type == TYPE_INT && dest_type == TYPE_DOUBLE) {
		convert_int_to_double(source);
	} else {
		*error = 6;
	}
}

void check_return(ast_node_t *node, tree_t *local_symtable, int *error){

	if(node == NULL) return;

	ast_node_t *current_func = node->data.node_return.func_def;

	if(current_func == NULL){
		*error = 10;
		return;
	}

	ast_node_t *return_var = node->data.node_return.var;
	data_type_t func_return = current_func->return_type;

	if(func_return == TYPE_ANY){
		*error = 6;
		return;
	}

	if(func_return == TYPE_UNIT){
		if(return_var != NULL){
			*error = 6;
		}
		return;
	}

	if(return_var == NULL){
		*error = 6;
		return;
	}

	sem_check_node(return_var, local_symtable, error);
	if(*error != 0){
		return;
	}

	if (func_return == TYPE_DOUBLE && return_var->return_type == TYPE_INT){
		convert_int_to_double (return_var);
	} else if(return_var->return_type != func_return){
		*error = 6;
	}
	return;
}

void sem_check_node(ast_node_t *node, tree_t *local_symtable, int *error) {
	if (node == NULL || *error != 0) {
		return;
	}

	switch (node->type) {
	case AST_ASSIGN:
		check_assign(node, local_symtable, error);
		break;

	case AST_BINOP:
		check_binop(node, local_symtable, error);
		break;

	case AST_BLOCK: {
		tree_t *block_symtable = node->data.block.symtable;
		sem_check_node(node->data.block.first_line, block_symtable, error);
		break;
	}

	case AST_IF:
		check_if(node, local_symtable, error);
		break;

	case AST_WHILE:
		check_while(node, local_symtable, error);
		break;

	case AST_FUNC_DEF:
		check_func_1(node, error);
		sem_check_node(node->data.func.args, local_symtable, error);
		sem_check_node(node->data.func.body, local_symtable, error);
		check_func_2(node, local_symtable, error);
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
		check_return(node, local_symtable, error);
		break;

	case AST_NO_TYPE:
		break;
	}
	sem_check_node(node->next, local_symtable, error);
}

int sem_check(ast_tree_t *tree) {
	int error = 0;

	sem_check_node(tree->root, tree->global_symtable, &error);

	return error;
}
