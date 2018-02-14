#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "values.h"


int calc_max_match(lex_automaton_t* automaton, char* input, int start_pos, 
                   token_id_t* token)
{
    int cur_state = 0;
    int result = 0;
    int i;
    for (i = start_pos; i < strlen(input); i++)
    {
        
        cur_state = automaton->table[cur_state][input[i]];
        if (cur_state == ERROR_STATE)
        {
            return result;
        }
        if (automaton->final[cur_state] != INVALID_TOKEN)
        {
            *token = automaton->final[cur_state];
            result = i - start_pos + 1;
        }
    }
    
    return result;
}


void parse_data(token_t* token, char* input, int start_pos, int len)
{
    char* temp = malloc(len);
    memcpy(temp, input + start_pos, len);

    if (temp[0] == 'x')
    {
        token->data = X;
        return;
    }
    
    int i;
    for (i = 0; i < strlen(temp); i++)
    {
        if (!isdigit(temp[i]) && temp[i] != '.')
        {
            return;
        }
    }
    
    token->data = (long double)atof(temp);
}


token_t my_yylex(bool restart)
{
    lex_automaton_t automaton =
    {
        #include "lex_automaton.h"
    }; 
    
    static char input[] =
        //"((((("
        //"((((24 * 50 - 310) * 755 * x + x) / 19"
        //" + 69 + x) * 49 - 12 + 5) / 4 * x - 7";
        //" + 10) * x + x - 12) / 313 + x) / 4"
        //" * 9 - 2 + x - 5) / 16"
        //" * 8 + x - 3) / 77";
        
        "24.21 * 50 - 310 * 755 * x + x / 19"
        " + 69 + x * 49 - 12 + 5 / 4 * x - 7";
        //"24.21 * x - 310 + 2";
        
   
    static int i = 0;
    token_t end = 
    {
        .id = 0,
    };    
    if (restart)
    {
        i = 0;
        return end;
    }
    
    for ( ; i < strlen(input); )
    {
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n')
        {
            i++;
            continue;
        }
        
        token_t token;
        int max_match = calc_max_match(&automaton, input, i, &token.id);
        
        if (max_match == 0)
        {
            printf("[my_yylex.h] Error while parsing input ");
            printf("at %d position.\n", i);
            exit(0);
        }
        
        parse_data(&token, input, i, max_match);
        i += max_match;

        //printf("mm %d %c\n", i, input[i - 1]);
        return token;
    }
    
    
    return end;
}

