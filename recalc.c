#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "values.h"
#include "my_yylex.h"
#include "executions.h"


#define MAX_STATEMENT_SIZE 30


int parse(tables_t* tables, int x, token_t* input, int len)
{
    int state[MAX_STATES] = {0};
    int result[MAX_STATES] = {0};
    int stack_top = 0;

    int input_ptr = 0;
    token_t token = input[input_ptr++];

    len++;
    while (input_ptr <= len)
    {
        if (input_ptr == len)
        {
            //input_ptr++;
            token.id = 0;
        }
        
        if (token.id == INVALID_TOKEN)
        {
            printf("Invalid token has been found at %d-th position of input.",
                   token.data);
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


void measure_time(tables_t* tables)
{
    double timer[MAX_STATEMENT_SIZE];
    int len;
    //for (len = 1; len < MAX_STATEMENT_SIZE; le
    int i;
    for (i = 0; i < 10; i++)
    {
        my_yylex(1);
        //printf("%d\n", parse(tables, i));
    }
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
        printf("[%d] %d %d\n", i, input[i].id, input[i].data);
    }
    //measure_time(&tables);

    //my_yylex(1);
    printf("%d\n", parse(&tables, 12, input, 29));
    
    return 0;
}
