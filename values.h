#ifndef _VALUES_H_
#define _VALUES_H_

#define BUFFER_SIZE 1000
#define ERROR_STATE -1
#define INVALID_TOKEN -1
#define MAX_STATES 300
#define MAX_TOKENS 300
#define X -2

const char NOT_CHAR = 254;

typedef int token_id_t;

typedef enum
{
    AC_SHIFT,
    AC_REDUCE,
    AC_ACCEPT,
    AC_ERROR,
} action_t;

typedef enum
{
    NT_CHAR,
    NT_CAT,
    NT_STAR,
    NT_OR,
    NT_END,
    NT_LPAREN,
    NT_RPAREN,
    NT_EPS,
} node_type_t;

typedef struct rule_token_t
{
    node_type_t type;
    char symbol;
} rule_token_t;

typedef struct lex_rule_t
{
    char* abbrev;
    int size;
    rule_token_t* list;
} lex_rule_t;

typedef struct buffer_t
{
    int count;
    lex_rule_t* rule;
} buffer_t;

typedef struct node_t
{
    node_type_t type;
    char symbol;
    int prior;
    bool is_nullable;
    int first_ptr, last_ptr, follow_ptr;
    struct node_t* first[BUFFER_SIZE];
    struct node_t* last[BUFFER_SIZE];
    struct node_t* follow[BUFFER_SIZE];
    struct node_t* left;
    struct node_t* right;
    struct node_t* parent;
} node_t;


const int default_prior[] = 
{
    [NT_CHAR] = 0,
    [NT_END] = 0,
    [NT_EPS] = 0,
    [NT_STAR] = 1,
    [NT_CAT] = 2,
    [NT_OR] = 3,
    [NT_LPAREN] = 4,
    [NT_RPAREN] = 4,
};


typedef struct lex_automaton_t
{
    int size;
    int* final;
    int** table;
} lex_automaton_t;



typedef struct production_skeleton_t
{
    int size;
    char* list[MAX_TOKENS];
} production_skeleton_t;

typedef struct grammar_skeleton_t
{
    int size;
    production_skeleton_t prod[MAX_TOKENS];
} grammar_skeleton_t;

typedef struct table_cell_t
{
    action_t action;
    int num;
} table_cell_t;

typedef struct transit_t
{
    int size;
    table_cell_t list[MAX_STATES][MAX_TOKENS];
} transit_t;

typedef struct item_t
{
    int prod;
    int pos;
    token_id_t end;
} item_t;

typedef struct item_list_t
{
    int size;
    item_t list[MAX_STATES];
} item_list_t;

typedef struct first_t
{
    int size;
    token_id_t* list;
} first_t;

typedef struct production_t
{
    token_id_t left;
    token_id_t* list;
    int size;
} production_t;

typedef struct token_names_t
{
    int size;
    char** list;
} token_names_t;

typedef struct grammar_t
{
    token_names_t token_names;
    production_t* prod;
    int size;
} grammar_t;

typedef struct token_t
{
    int id;
    long double data;
} token_t;

typedef struct tables_t
{
    int* grammar_left;
    int* grammar_size;
    table_cell_t** trans;
} tables_t;

#endif /* _VALUES_H_ */
