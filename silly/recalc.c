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


long double parse(tables_t* tables, int x, token_t* input, int len)
{
    int state[MAX_STATES] = {0};
    long double result[MAX_STATES] = {0};
    int stack_top = 0;

    int input_ptr = 0;
    token_t token = input[input_ptr++];

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
                state[stack_top] = cell.num;
                if (token.data != X)
                {
                    result[stack_top] = token.data;
                }
                else
                {
                    result[stack_top] = x;
                }
                token = input[input_ptr++];
                break;
                
            case AC_REDUCE:
                apply[cell.num](&result, stack_top);
                stack_top -= tables->grammar_size[cell.num];
                cur_state = state[stack_top];
                state[++stack_top] = tables->
                    trans[cur_state][tables->grammar_left[cell.num]].num;
                break;
                
            case AC_ACCEPT:
                return result[1];
                
            case AC_ERROR:
                printf("Invalid token [id=%d] for the %d-th state.\n",
                       token.id, cur_state);
                exit(0);
        }
    }
}


void measure_time(tables_t* tables, token_t* input)
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
            double start_time = clock();
            
            int x;
            for (x = 0; x < 1e7; x++)
            {
                parse(tables, x, input, len);
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
        printf("recalc,%d,%.3lf\n", i, timer[i]);
    }
    printf("\n");

    
}


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
    
    measure_time(&tables, input);

    //printf("%Lf\n", parse(&tables, 12, input, 29));
    
    return 0;
}
