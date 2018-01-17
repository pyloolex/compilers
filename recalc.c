#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "values.h"
#include "my_yylex.h"
#include "executions.h"


bool parse(tables_t* tables)
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
            return false;
        }
        
        int cur_state = state[stack_top];
        table_cell_t cell = tables->trans[cur_state][token.id];
        
        switch (cell.action)
        {
            case AC_SHIFT:
                stack_top++;
                state[stack_top] = cell.num;
                result[stack_top] = token.data;
                token = my_yylex(0);
                //printf("%d\n", token.id);
                break;
                
            case AC_REDUCE:
                apply[cell.num](&result, stack_top);                
                stack_top -= tables->grammar_size[cell.num];
                cur_state = state[stack_top];
                state[++stack_top] = tables->
                    trans[cur_state][tables->grammar_left[cell.num]].num;
                break;
                
            case AC_ACCEPT:
                printf("Result: %d\n", result[1]);
                return true;
                
            case AC_ERROR:
                printf("Invalid token [id=%d] for the %d-th state.\n",
                       token.id, cur_state);
                return false;
        }
    }
}


int main()
{
    tables_t tables = 
    {
        #include "syn_tables.h"
    };
    
    parse(&tables);
    
    return 0;
}
