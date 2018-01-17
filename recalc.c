#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "values.h"
#include "my_yylex.h"
#include "executions.h"


int parse(tables_t* tables, int x)
{
    int state[MAX_STATES] = {0};
    int result[MAX_STATES] = {0};
    int stack_top = 0;
    
    token_t token = my_yylex(0);
    
    while (true)
    {
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
                token = my_yylex(0);
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
    int i;
    for (i = 0; i < 10; i++)
    {
        my_yylex(1);
        printf("%d\n", parse(tables, i));
    }
}


int main()
{
    tables_t tables = 
    {
        #include "syn_tables.h"
    };

    measure_time(&tables);
    //printf("%d\n", parse(&tables, 100));
    
    return 0;
}
