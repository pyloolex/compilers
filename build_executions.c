#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "values.h"


int parse_number(char* input, int* pos)
{
    int i;
    for (i = *pos; isdigit(input[i]); i++);
    
    char* temp = malloc(i - *pos);
    memcpy(temp, input + *pos, i - *pos);
    
    *pos = i;
    return atoi(temp);
}


void parse_line(char* input, tables_t* tables, int* count)
{
    int i;
    for (i = 0; input[i] == ' ' || input[i] == '\t'; i++);
    if (input[i] != '$' || input[i] == '\n') return;
    
    (*count)++;
    printf("void apply_prod_%d(int* result, int stack_top)\n{\n    ", *count);
    printf("result[stack_top - %d] =", tables->grammar_size[*count] - 1);
    for ( ; input[i] != '='; i++);
    i++;
    
    for ( ; input[i] != '\n'; )
    {
        if (input[i] == '$')
        {
            i++;
            int arg = parse_number(input, &i);
            printf("result[stack_top - %d]", tables->grammar_size[*count] 
                   - arg);
        }
        else
        {
            printf("%c", input[i++]);
        }
    }
    
    printf(";\n}\n");
}

int main()
{
    printf("void apply_prod_0(int* result, int stack_top) {}\n");
    
    tables_t tables = 
    {
        #include "syn_tables.h"
    };
    
    int count = 0;
    char input[BUFFER_SIZE];
    while (fgets(input, sizeof input, stdin))
    {
        parse_line(input, &tables, &count);
    }
    
    printf("\nvoid (*apply[])() =\n{\n");
    int i;
    for (i = 0; i <= count; i++)
    {
        printf("    apply_prod_%d,\n", i);
    }
    printf("};\n");
}
