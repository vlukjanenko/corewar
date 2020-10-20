
# include "asm.h"

t_op	g_op_tab[17] =
{
	{0,	0, {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0},
	/*								*/
	{"live", 1, {{0, 0, T_DIR, 0}}, 1, 0},
	{"ld", 2, {{0,0, T_DIR, T_IND}, {0, T_REG, 0, 0}}, 2, 1},
	{"st", 2, {{0, T_REG, 0, 0}, {0, T_REG, 0, T_IND}}, 3, 1},
	{"add", 3, {{0, T_REG}, {0, T_REG}, {0, T_REG}}, 4, 1},
	{"sub", 3, {{T_REG, T_REG, T_REG}}, 5, 1},
	{"and", 3, {{0, T_REG, T_DIR, T_IND}, {0, T_REG, T_DIR, T_IND}, {0, T_REG}}, 6, 1},
	{"or", 3,  {{0, T_REG, T_DIR, T_IND}, {0, T_REG, T_DIR, T_IND}}, 7, 1},
	{"xor", 3, {{0, T_REG, T_DIR, T_IND}, {0, T_REG, T_DIR, T_IND}}, 8, 1},
	{"zjmp", 1, {{0, 0, T_DIR}}, 9, 0},
	{"ldi", 3, {{0, T_REG, T_DIR / 2, T_IND}, { 0, T_REG, T_DIR / 2}, {0, T_REG}}, 10, 1},
	{"sti", 3, {{0, T_REG}, {0, T_REG, T_DIR / 2, T_IND}, {0, T_REG, T_DIR / 2}}, 11, 1},
	{"fork", 1, {{0, 0, T_DIR}}, 12, 0},
	{"lld", 2, {{0, 0, T_DIR, T_IND}, {0, T_REG}}, 13, 1},
	{"lldi", 3, {{0, T_REG, T_DIR / 2, T_IND}, {0, T_REG, T_DIR / 2}, {0, T_REG}}, 14, 1},
	{"lfork", 1, {{0, 0, T_DIR}}, 15, 0},
	{"aff", 1, {{0, T_REG}}, 16, 1}
};

unsigned char g_params[2][4] = {{0, T_REG, T_DIR, T_IND}, {0, T_REG, 2, T_IND}};

void print_usage(void) {}

t_asm	*init_asm(int fd)
{
	t_asm *assembler;

	assembler = (t_asm *)ft_memalloc(sizeof(t_asm)); //todo:free
	if (!assembler)
		exit(0); //todo:add term
	assembler->fd = fd;
	//assembler->row = 0;
	//assembler->column = 0;
	//assembler->tokens = NULL;
	return (assembler);
}

t_token		*new_token(t_asm *assembler, int type)
{
	t_token	*token;

	token = (t_token *)malloc(sizeof(t_token)); //todo:free
	if (!token)
		exit(0); //todo:add term
	token->content = NULL;
	token->type = type;
	token->row = assembler->row;
	if (type == SEPARATOR || type == NEW_LINE)
		token->column = assembler->column - 1;
	else
		token->column = assembler->column;
	token->next = NULL;
	return (token);
}

void 	add_token(t_token **head, t_token *token)
{
	t_token *temp;

	if (!head)
		return ;
	if (*head)
	{
		temp = *head;
		while (temp->next)
			temp = temp->next;
		if (token->type == NEW_LINE && temp->type == NEW_LINE)
			free(token);
		else
			temp->next = token;
	}
	else
	{
		if (token->type == NEW_LINE)
			free(token);
		else
			*head = token;
	}
}

void 	parse_symbols(t_asm *assembler, char *row, unsigned int start, t_token *token)
{
	unsigned int column;

	token->column = start;
	column = assembler->column;
	while (row[assembler->column] && ft_strchr(LABEL_CHARS, row[assembler->column]))
		assembler->column++;
	if (assembler->column - column > 0 && row[assembler->column] == LABEL_CHAR)
	{
		assembler->column++;
		if (!(token->content = ft_strsub(row, start, assembler->column - start)))
			exit(0); //todo:add term
		token->type = LABEL;
		add_token(&(assembler->tokens), token);
	}
	else if (assembler->column - column > 0 && is_delimiter(row[assembler->column]))
	{
		if (!(token->content = ft_strsub(row, start, assembler->column - start)))
			exit(0); //todo:add term
		if (token->type == INDIRECT)
			token->type = is_register(token->content) ? REGISTER : OPERATOR;
		add_token(&(assembler->tokens), token);
	}
	else
		exit(0); //todo:add lexical error

}

void 	parse_number(t_asm *assembler, char *row, unsigned int start, t_token *token)
{
	unsigned int column;

	token->column = start;
	if (row[assembler->column] == '-')
		assembler->column++;
	column = assembler->column;
	while (ft_isdigit(row[assembler->column]))
		assembler->column++;
	if (assembler->column - column > 0 && (token->type == DIRECT || is_delimiter(row[assembler->column])))
	{
		if (!(token->content = ft_strsub(row, start, assembler->column - start)))
			exit(0); //todo:add term
		add_token(&(assembler->tokens), token);
	}
	else if (token->type != DIRECT)
	{
		assembler->column = start;
		parse_symbols(assembler, row, start, token);
	}
	else
		exit(0); //todo:add lexical error
}

char 	*ft_strjoin_extended(char **str1, char **str2)
{
	char *result;

	result = ft_strjoin(*str1, *str2);
	if (!result)
		exit(0); //todo:add terminate;
	ft_strdel(str1);
	ft_strdel(str2);
	return (result);
}

void 	update_assembler_pos(t_asm *assembler, const char *str)
{
	unsigned int i;

	assembler->column++;
	i = assembler->column;
	while (str[i] != '\"')
	{
		if (str[i] == '\n')
		{
			assembler->row++;
			assembler->column = 0;
		}
		else
			assembler->column++;
		i++;
	}
}

void	reinit_row(char **row, char *str)
{
	char *s;

	s = ft_strdup(str);
	if (!s)
		exit(0);//todo:add term;
	ft_strdel(row);
	*row = s;
}

void	parse_string(t_asm *assembler, char **row, unsigned int start, t_token *token)
{
	int		size;
	char	*end;
	char 	*str;

	token->column = start;
	size = 1;
	while (!(end = ft_strchr(&((*row)[start + 1]), '\"')) && (size = read_row(assembler->fd, &str)) > 0)
		*row = ft_strjoin_extended(row, &str);
	update_assembler_pos(assembler, *row);
	if (size == -1)
		exit(0); //todo:add terminate;
	if (size == 0)
		exit(0); //todo:add lexical error
	token->content = ft_strsub(*row, start, end + 1 - &((*row)[start]));
	if (!token->content)
		exit(0);//todo:add term
	if (end - assembler->column != *row)
		reinit_row(row, end - assembler->column);
	assembler->column++;
	add_token(&(assembler->tokens), token);
}

void 	parse_token(t_asm *assembler, char **row)
{
	//ft_printf("try to parse_token %d \'%c\' \'%s\'\n", assembler->column, (*row)[assembler->column], *row);
	if ((*row)[assembler->column] == SEPARATOR_CHAR)
	{
		assembler->column++;
		add_token(&(assembler->tokens), new_token(assembler, SEPARATOR));
	}
	else if ((*row)[assembler->column] == '\n')
	{
		assembler->column++;
		add_token(&(assembler->tokens), new_token(assembler, NEW_LINE));
	}
	else if ((*row)[assembler->column] == '.')
	{
		//ft_printf("command\n");
		parse_symbols(assembler, *row, assembler->column++, new_token(assembler, COMMAND));
	}
	else if ((*row)[assembler->column] == DIRECT_CHAR)
	{
		assembler->column++;
		if ((*row)[assembler->column] == LABEL_CHAR)
		{
			assembler->column++;
			parse_symbols(assembler, *row, assembler->column - 2, new_token(assembler, DIRECT_LABEL));
		}
		else
			parse_number(assembler, *row, assembler->column - 1, new_token(assembler, DIRECT));
	}
	else if ((*row)[assembler->column] == '\"')
	{
		//ft_printf("parse string\n");
		parse_string(assembler, row, assembler->column, new_token(assembler, STRING));
	}
	else if ((*row)[assembler->column] == LABEL_CHAR)
	{
		assembler->column++;
		parse_symbols(assembler, *row, assembler->column - 1, new_token(assembler, INDIRECT_LABEL));
	}
	else
		parse_number(assembler, *row, assembler->column, new_token(assembler, INDIRECT));

}

void 	print_tokens(t_token *tokens)
{
	while (tokens)
	{
		ft_printf("\'{red}%20s{eoc}\' row:{cyan}%2d{eoc} column:{cyan}%2d{eoc} type:{green}",
				  tokens->content ? tokens->content : "", tokens->row, tokens->column);
		if (tokens->type == COMMAND)
			ft_printf("command");
		else if (tokens->type == STRING)
			ft_printf("string");
		else if (tokens->type == LABEL)
			ft_printf("label");
		else if (tokens->type == OPERATOR)
			ft_printf("operator");
		else if (tokens->type == REGISTER)
			ft_printf("register");
		else if (tokens->type == DIRECT)
			ft_printf("direct");
		else if (tokens->type == DIRECT_LABEL)
			ft_printf("direct_label");
		else if (tokens->type == INDIRECT_LABEL)
			ft_printf("inderect_label");
		else if (tokens->type == INDIRECT)
			ft_printf("inderect");
		else if (tokens->type == SEPARATOR)
			ft_printf("separator");
		else if (tokens->type == NEW_LINE)
			ft_printf("new_line");
		else if (tokens->type == END)
			ft_printf("end");
		ft_printf("{eoc}\n");
		tokens = tokens->next;
	}
}

void 	parse_file(t_asm *assembler)
{
	char	*row;
	int 	ret;

	while ((ret = read_row(assembler->fd, &row)) > 0)
	{
		//ft_printf("line read\n");
		assembler->row++;
		assembler->column = 0;
		while (row[assembler->column])
		{
			skip_whitespaces(assembler, row);
			skip_comment(assembler, row);
			//ft_printf("ready to parse\n");
			if (row[assembler->column])
				parse_token(assembler, &row);
			//print_tokens(assembler->tokens);
		}
		ft_strdel(&row);
	}
	if (ret == -1)
		exit(0);//todo:add term;
	add_token(&(assembler->tokens), new_token(assembler, END));
}


char *ft_get_token_name(int index, int upercase)
{
	char tokens[35][15];
	char *name;

	ft_bzero(tokens, 35 * 15);
	ft_strcpy(tokens[0x11], "command");
	ft_strcpy(tokens[0x12], "string");
	ft_strcpy(tokens[0x13], "label");
	ft_strcpy(tokens[0x14], "instruction");
	ft_strcpy(tokens[0x15], "register");
	ft_strcpy(tokens[0x16], "direct");
 	ft_strcpy(tokens[0x17], "direct_label");
	ft_strcpy(tokens[0x18], "indirect");
 	ft_strcpy(tokens[0x19], "indirect_label");
 	ft_strcpy(tokens[0x20], "separator");
 	ft_strcpy(tokens[0x21], "new_line");
	ft_strcpy(tokens[0x22], "end");
	if (!(name = ft_strdup(tokens[index])))
		ft_asm_exit(NULL, NULL, NULL);
	if (upercase)
		name = ft_str_to_upper(name);
	return (name);
}

/*
**	todo add "," to separator content
*/

void ft_asm_exit(char *str, int pos[2], t_token *token)
{
	char *token_name;

	if (token)
	{
		token_name = ft_get_token_name(token->type, 1);
		ft_printf("Syntax error at token [TOKEN][%.3d:%.3d] %s \"%s\"\n",
		token->row, token->column + 1, token_name, token->content);
		ft_strdel(&token_name);
	}
	else if (pos)
		ft_printf("Lexical error at [%d:%d]\n", pos[0], pos[1]);
	else if (str)
		ft_printf("%s\n", str);
	else
		perror("ERROR");
	exit (1);
}

void ft_skip_name(t_token **token, t_asm *assembler, int *heat)
{
	*token = (*token)->next;
	if (ft_strlen((*token)->content) - 2 > PROG_NAME_LENGTH)
	{
		ft_printf("Champion name too long (Max length %d)\n",
		PROG_NAME_LENGTH);
		exit (1);
	}
	ft_memmove(assembler->header.prog_name, (*token)->content + 1,
	ft_strlen((*token)->content) - 2);
	*token = (*token)->next;
	if ((*token)->type != NEW_LINE)
		ft_asm_exit(NULL, NULL, *token);
	*token = (*token)->next;
	*heat = *heat | 1;
}

void ft_skip_comment(t_token **token, t_asm *assembler, int *heat)
{
	*token = (*token)->next;
	if (ft_strlen((*token)->content) - 2 > COMMENT_LENGTH)
	{
		ft_printf("Champion comment too long (Max length %d)\n",
		COMMENT_LENGTH);
		exit (1);
	}
	ft_memmove(assembler->header.comment, (*token)->content + 1,
	ft_strlen((*token)->content) - 2);
	*token = (*token)->next;
	if ((*token)->type != NEW_LINE)
		ft_asm_exit(NULL, NULL, *token);
	*token = (*token)->next;
	*heat = *heat | 2;

}

int ft_get_op_code(const char *name)
{
	int i = 0;

	while (++i < 17)
	{
		if (ft_strequ(name, g_op_tab[i].name))
			return (i);
	}
	return 0;
}

void ft_skip_name_or_comment(t_token **token, t_asm *assembler, int *heat)
{
	if ((*token)->type != COMMAND)
		ft_asm_exit(NULL, NULL, *token);
	else if ((*token)->next->type != STRING)
		ft_asm_exit(NULL, NULL, *token);
	if (ft_strequ((*token)->content, NAME_CMD_STRING) && !(*heat & 1))
		ft_skip_name(token, assembler, heat);
	else if (ft_strequ((*token)->content, COMMENT_CMD_STRING) && !(*heat & 2))
		ft_skip_comment(token, assembler, heat);
	else
		ft_asm_exit(NULL, NULL, *token);
}

void ft_skip_labels(t_token **token, t_asm *assembler)
{
	t_list	*new_label_node;
	t_label	*label_content;

	while((*token)->type == LABEL)
	{
		if (!(label_content = (t_label *)ft_memalloc(sizeof(*label_content))))
			ft_asm_exit(NULL, NULL, NULL);
		label_content->label = (*token)->content;
		label_content->pos = assembler->pos;
		if (!(new_label_node = ft_lstnew(label_content, sizeof(*label_content))))
			ft_asm_exit(NULL, NULL, NULL);
		ft_lstadd(&assembler->labels, new_label_node);
		*token = (*token)->next->type == NEW_LINE ? (*token)->next->next : (*token)->next;
	}
}

int ft_get_arg_type_code(int type)
{
	if (type == REGISTER)
		return (1);
	else if (type == DIRECT || type == DIRECT_LABEL)
		return (2);
	else if (type == INDIRECT || type == INDIRECT_LABEL)
		return (3);
	return (0);
}


void ft_skip_args(int op, t_token **token, t_asm *assembler)
{
	int args[3];
	int i;
	char *token_name;

	i = 0;
	ft_bzero(args, sizeof(int) * 3);
	while ((*token)->type != NEW_LINE)
	{
		if (!(args[i] = ft_get_arg_type_code((*token)->type)))
			ft_asm_exit(NULL, NULL, *token);
		if (!g_op_tab[op].types[i][args[i]] || i >= g_op_tab[op].max_params)
		{
			token_name = ft_get_token_name((*token)->type, 0);
			ft_printf("Invalid parameter %d type %s for instruction %s\n", i, token_name, g_op_tab[op].name); //to do: replace type code with string
			ft_strdel(&token_name);
			exit (1);
		}
		i++;
		*token = (*token)->next->type == SEPARATOR ?
		(*token)->next->next : (*token)->next;
	}
	if (i == 0)
		ft_asm_exit(NULL, NULL, *token);
	assembler->pos += 1 + g_op_tab[op].acb + g_op_tab[op].types[0][args[0]] + g_op_tab[op].types[1][args[1]] + g_op_tab[op].types[2][args[2]];
	*token = (*token)->next;
}

void ft_skip_operator(t_token **token, t_asm *assembler)
{
	int op;

	ft_skip_labels(token, assembler);
	if ((*token)->type == END)
		return;
	if ((*token)->type != OPERATOR)
		ft_asm_exit(NULL, NULL, *token);
	if (!(op = ft_get_op_code((*token)->content)))
		ft_asm_exit(NULL, NULL, *token);
	*token = (*token)->next;
	ft_skip_args(op, token, assembler);
}

void check_syntax(t_asm *assembler, t_token *tokens)
{
	int heat;

	heat = 0;
	ft_skip_name_or_comment(&tokens, assembler, &heat);
	ft_skip_name_or_comment(&tokens, assembler, &heat);
	while (tokens && tokens->type != END)
	{
		ft_skip_operator(&tokens, assembler);
	}
}
void debug_print_labels(t_list *node)
{
	t_label *label;

	label = node->content;
	ft_printf("label = \"%s\", pos = %d\n", label->label, label->pos);
}
int main(int argc, char **argv)
{
	int		fd;
	t_asm	*assembler;

	if (argc != 2)
		print_usage();
	fd = open(argv[1], O_RDONLY);
	if (fd <= 0)
		exit(0); //todo:add terminate program
	assembler = init_asm(fd);
	parse_file(assembler);
	print_tokens(assembler->tokens);
	check_syntax(assembler, assembler->tokens);
	ft_lstiter(assembler->labels, debug_print_labels);
	return (0);
}
