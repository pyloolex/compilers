#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "values.h"
#include "my_yylex.h"
#include "executions.h"


#define ATTEMPTS_PER_TEST 5
#define MAX_STATEMENT_SIZE 15  // < 21


void build_ast(tables_t* tables, token_t* input,
               ast_node_t* arena, int* arena_top)
{
    int state[MAX_STATES] = {0};
    int stack_top = -1;

    int input_ptr = 0;
    token_t token = input[input_ptr++];

    int arena_idx[BUFFER_SIZE];

    while (true)
    {        
        if (token.id == INVALID_TOKEN)
        {
            printf("Invalid token has been found at %d-th position of input.",
                   (int)token.data);
            exit(0);
        }
        
        int cur_state = state[stack_top];
        table_cell_t cell = tables->trans[cur_state][token.id];

        switch (cell.action)
        {
            case AC_SHIFT:
                stack_top++;
                (*arena_top)++;
                state[stack_top] = cell.num;
                if (token.data != X)
                {
                    arena[*arena_top].type = AT_LEAF;
                    arena[*arena_top].val = token.data;
                }
                else
                {
                    arena[*arena_top].type = AT_X;
                }
                token = input[input_ptr++];

                arena_idx[stack_top] = *arena_top;
                break;
                
            case AC_REDUCE:
                (*arena_top)++;
                arena[*arena_top].type = AT_FUNCTION;
                arena[*arena_top].fun_idx = cell.num;

                int i;
                for (i = 0; i < tables->grammar_size[cell.num]; i++)
                {
                    arena[*arena_top].child[2 - i] =
                        arena_idx[stack_top - i];
                }

                stack_top -= tables->grammar_size[cell.num];
                cur_state = state[stack_top];
                state[++stack_top] = tables->
                    trans[cur_state][tables->grammar_left[cell.num]].num;

                arena_idx[stack_top] = *arena_top;
                break;
                
            case AC_ACCEPT:
                return;
                
            case AC_ERROR:
                printf("Invalid token [id=%d] for the %d-th state.\n",
                       token.id, cur_state);
                exit(0);
        }
    }
}


long double calculate(ast_node_t* arena, int arena_top, long double x,
                      tables_t* tables)
{
    long double aux[3];
    int i;
    for (i = 0; i <= arena_top; i++)
    {
        if (arena[i].type == AT_FUNCTION)
        {
            int j;
            for (j = 0; j < 3; j++)
            {
                aux[j] = arena[arena[i].child[j]].val;
            }
            apply[arena[i].fun_idx](&aux, 2);

            arena[i].val = aux[3 - tables->grammar_size[arena[i].fun_idx]];
        }
        else if (arena[i].type == AT_X)
        {
            arena[i].val = x;
        }
    }

    return arena[arena_top].val;
}


void prepare_input(token_t* input, int expr_idx)
{
    int i;
    for (i = 0, input[i] = my_yylex(expr_idx); input[i].id;
         input[++i] = my_yylex(expr_idx));
}


void measure_time(ast_node_t* arena, tables_t* tables,
                  token_t* input)
{
    double timer[MAX_STATEMENT_SIZE];
    int len;
    for (len = 0; len < MAX_STATEMENT_SIZE; len++)
    {
        prepare_input(input, len);
        int arena_top = 0;
        build_ast(tables, input, arena, &arena_top);
        
        printf("[len = %d]\n", len + 1);
        double sum = 0;
        int attempt;
        for (attempt = 0; attempt < ATTEMPTS_PER_TEST; attempt++)
        {            
            double start_time = clock();
            
            int x;
            for (x = 0; x < 1e7; x++)
            {
                calculate(arena, arena_top, x, tables);
            }
            
            double elapsed = ((clock() - start_time) / CLOCKS_PER_SEC);
            sum += elapsed;
            
            printf("Elapsed time: %.3f sec\n", elapsed);
        }
        timer[len] = sum / attempt;
        
        printf("_________________________________\n");
        printf("Average elapsed time: %.3f sec\n", sum / attempt);
    }
    
    printf("\n");
    int i;
    for (i = 0; i < MAX_STATEMENT_SIZE; i++)
    {
        printf("ast,%d,%.3lf\n", i + 1, timer[i]);
    }
    printf("\n");

    
}


void print_input(token_t* input)
{    
    int i;
    for (i = 0; i < 70; i++)
    {
        printf("[%d] %d %Lf\n", i, input[i].id, input[i].data);
    }
}


void print_ast(ast_node_t* arena, int arena_top)
{
    int i;
    for (i = 0; i <= arena_top; i++)
    {
        ast_node_t a = arena[i];
        printf("- %d -\n", i);
        printf("type: %d\nfun_idx: %d\nval: %Lf\n",
            a.type, a.fun_idx, a.val);
        printf("child: ");
        int j;
        for (j = 0; j < 3; j++)
        {
            printf("%d ", a.child[j]);
        }
        printf("\n");
        printf("------\n");
    }
}


void test_for_12(token_t* input, tables_t* tables, ast_node_t* arena,
                 int arena_top)
{
    int i;
    for (i = 0; i < MAX_STATEMENT_SIZE; i++)
    {
        prepare_input(input, i);
        arena_top = 0;
        build_ast(tables, input, arena, &arena_top);
        printf("%Lf\n", calculate(arena, arena_top, 12, tables));
    }
}


void smoke_test(token_t* input, tables_t* tables, ast_node_t* arena,
                int arena_top)
{
    prepare_input(input, 19);
    build_ast(tables, input, arena, &arena_top);
    printf("%Lf\n", calculate(arena, arena_top, 12, tables));
}


int main()
{
    tables_t tables = 
    {
        #include "syn_tables.h"
    };

    token_t input[200];
    int arena_top = -1;
    ast_node_t arena[BUFFER_SIZE];

    //test_for_12(input, &tables, arena, arena_top);
    //print_ast(arena, arena_top);

    measure_time(arena, &tables, input);
    
    return 0;
}
