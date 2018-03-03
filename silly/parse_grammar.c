#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "values.h"


char* parse_lexeme(char* input, int start_pos)
{
    char temp[BUFFER_SIZE];
    int i;
    for (i = start_pos; input[i] != ' ' && input[i] != '\t' && input[i] != ':'
             && input[i] != '\n'; i++)
    {
        temp[i - start_pos] = input[i];
    }
    temp[i] = '\0';
    
    char* result = temp;
    return result;
}


int get_token_id(int* tokens, char token_names[][BUFFER_SIZE])
{
    int i;
    for (i = 0; i < *tokens; i++)
    {
        if (strcmp(token_names[i], token_names[*tokens]) == 0)
        {
            return i;
        }
    }
    (*tokens)++;
    
    return *tokens - 1;
}


int parse_until(char* input, char symbol, int* start_pos, int* tokens,
                char token_names[][BUFFER_SIZE])
{
    int i;
    for (i = *start_pos; input[i] != ' ' && input[i] != '\t' && 
             input[i] != symbol; i++)
    {
        token_names[*tokens][i - *start_pos] = input[i];
    }
    token_names[*tokens][i - *start_pos] = '\0';
    
    for ( ; input[i] == ' ' || input[i] == '\t' || input[i] == ':'; i++);
    *start_pos = i;
    
    return get_token_id(tokens, token_names);
}

void parse_line(char* input, int* count, int* tokens,
                char token_names[][BUFFER_SIZE])
{
    int i;
    for (i = 0; input[i] == ' ' || input[i] == '\t'; i++);
    if (input[i] == '$' || input[i] == '\n') return;
    
    (*count)++;
    int left = parse_until(input, ':', &i, tokens, token_names);
    
    printf("    {\n        .left = %d,\n", left);
    printf("        .list = (token_id_t[])\n        {\n            ");
    
    int size = 0;
    while (input[i] != '\n')
    {
        size++;
        int token = parse_until(input, '\n', &i, tokens, token_names);
        printf("%d, ", token);
    }
    printf("\n        },\n        .size = %d,\n    },\n", size);
}


int main()
{
    char token_names[MAX_TOKENS][BUFFER_SIZE];
    strcpy(token_names[0], "$");
    strcpy(token_names[1], "^");
    int tokens = 2;
    
    printf(".prod = (production_t[])\n{\n");
    printf("    {\n        .left = 1,\n        .list = (int[])\n");
    printf("        {\n            2,\n        },\n");
    printf("        .size = 1,\n    },\n");
    
    int count = 1;
    char input[BUFFER_SIZE];
    while (fgets(input, sizeof input, stdin))
    {
        parse_line(input, &count, &tokens, token_names);
    }
    
    printf("},\n.size = %d,\n.token_names = \n{\n", count);
    printf("    .size = %d,\n    .list = (char*[])\n    {\n", tokens);
    int i;
    for (i = 0; i < tokens; i++)
    {
        printf("        \"%s\",\n", token_names[i]);
    }
    printf("    },\n},\n");
}
