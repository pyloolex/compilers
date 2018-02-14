#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "values.h"
#include "my_yylex.h"
#include "executions.h"


#define ATTEMPTS_PER_TEST 5
#define MAX_STATEMENT_SIZE 30 // < 31


void build_ast(tables_t* tables, token_t* input, int len,
                  ast_node_t* arena, int* arena_top)
{
    int state[MAX_STATES] = {0};
    int stack_top = 0;

    int input_ptr = 0;
    token_t token = input[input_ptr++];

    int arena_idx[BUFFER_SIZE];

    len++;
    while (input_ptr <= len)
    {
        if (input_ptr == len)
        {
            token.id = 0;
        }
        
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
                if (tables->grammar_size[cell.num] == 3)
                {
                    arena[*arena_top].lf = arena_idx[stack_top - 2];
                    arena[*arena_top].rg = arena_idx[stack_top];
                }
                else
                {
                    arena[*arena_top].rg = arena_idx[stack_top];
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
            aux[0] = arena[arena[i].lf].val;
            aux[2] = arena[arena[i].rg].val;
            apply[arena[i].fun_idx](&aux, 2);

            if (tables->grammar_size[arena[i].fun_idx] == 3)
            {
                arena[i].val = aux[0];
            }
            else
            {
                arena[i].val = aux[2];
            }            
        }
        else if (arena[i].type == AT_X)
        {
            arena[i].val = x;
        }
    }

    return arena[arena_top].val;
}



void measure_time(ast_node_t* arena, int arena_top, tables_t* tables,
                  token_t* input)
{
    double timer[MAX_STATEMENT_SIZE];
    int len;
    for (len = 1; len < MAX_STATEMENT_SIZE; len += 2)
    {
        printf("[len = %d]\n", len);
        double sum = 0;
        int attempt;
        for (attempt = 0; attempt < ATTEMPTS_PER_TEST; attempt++)
        {            
            arena_top = 0;
            build_ast(tables, input, len, arena, &arena_top);
            
            double start_time = clock();
            
            int x;
            for (x = 0; x < 1e7; x++)
            {
                calculate(arena, arena_top, x, tables);
            }
            
            double elapsed = ((clock() - start_time) / CLOCKS_PER_SEC);
            sum += elapsed;
            
            printf("Elapsed time: %.3f ms\n", elapsed);
        }
        timer[len] = sum / attempt;
        
        printf("_________________________________\n");
        printf("Average elapsed time: %.3f ms\n", sum / attempt);
    }
    
    printf("\n");
    int i;
    for (i = 1; i < MAX_STATEMENT_SIZE; i += 2)
    {
        printf("ast,%d,%.3lf\n", i, timer[i]);
    }
    printf("\n");

    
}



/*
{
    arena_top = 0;
    build_ast(tables, input, 30, arena, &arena_top);
    printf("%Lf\n", calculate(arena, arena_top, 50, tables));
    printf("%Lf\n", calculate(arena, arena_top, 0, tables));

    arena_top = 0;
    build_ast(tables, input, 3, arena, &arena_top);
    printf("%Lf\n", calculate(arena, arena_top, 50, tables));

    
    arena_top = 0;
    build_ast(tables, input, 5, arena, &arena_top);
    printf("%Lf\n", calculate(arena, arena_top, 50, tables));
}
*/

void prepare_input(token_t* input)
{
    int i;
    for (i = 0, input[i] = my_yylex(0); input[i].id;
         input[++i] = my_yylex(0)); 
}


int main()
{
    tables_t tables = 
    {
        #include "syn_tables.h"
    };

    token_t input[200];
    prepare_input(input);

    int i;
    for (i = 0; i < 70; i++)
    {
        //printf("[%d] %d %Lf\n", i, input[i].id, input[i].data);
    }
    
    //measure_time(&tables, input);

    int arena_top = 0;
    ast_node_t arena[BUFFER_SIZE];

    //printf("suck\n");
    build_ast(&tables, input, 30, arena, &arena_top);

    for (i = 0; i <= arena_top; i++)
    {
        ast_node_t a = arena[i];
        printf("- %d -\n", i);
        printf("type: %d\nfun_idx: %d\nval: %Lf\nlf: %d\nrg: %d\n",
            a.type, a.fun_idx, a.val, a.lf, a.rg);
        printf("------\n");
    }

    //printf("%Lf\n", calculate(arena, arena_top, 12, &tables));
    measure_time(arena, arena_top, &tables, input);
    
    return 0;
}
