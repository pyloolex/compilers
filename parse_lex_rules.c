#include <stdbool.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "values.h"


void print_token(char* type, char symbol)
{
    if (type == "NT_END")
    {
        printf("            {\n                .type = NT_END,\n");
        printf("                .symbol = %d,\n", symbol);
    }
    else
    {
        printf("            {\n                .type = %s,\n", type);
        
        if (symbol != NOT_CHAR)
        {
            printf("                .symbol = \'");
            if (symbol == '\n')
            {
                printf("\\n");
            }
            else if (symbol == '\t')
            {
                printf("\\t");
            }
            else if (symbol == '\r')
            {
                printf("\\r");
            }
            else if (symbol == '\\')
            {
                printf("\\\\");
            }
            else if (symbol == '\'')
            {
                printf("\\'");
            }
            else if (symbol == '\"')
            {
                printf("\\\"");
            }
            else
            {
                printf("%c", symbol);
            }
            printf("\',\n");
        }
    }
    
    printf("            },\n");
}


bool dont_need_cat(char* last, char* cur)
{
    if (last == "NT_OR" || last == "NT_LPAREN")
    {
        return true;
    }
    if (cur == "NT_OR" || cur == "NT_RPAREN" || cur == "NT_STAR")
    {
        return true;
    }
    return false;
}


void parse_rule(char input[BUFFER_SIZE], int idx)
{
    int semic_pos = 0;
    int i;
    for (i = 0; semic_pos == 0; i++)
    {
        if (input[i] == ':')
        {
            semic_pos = i;
            break;
        }
    }
    
    char s[BUFFER_SIZE];
    int s_ptr = 0;
    for (i = semic_pos + 1; i < strlen(input); )
    {
        if (input[i] == '\\')
        {
            if (input[i + 1] == 'd')
            {
                s[s_ptr++] = '(';
                char j;
                for (j = '0'; j < '9'; j++)
                {
                    s[s_ptr++] = j;
                    s[s_ptr++] = '|';
                }
                s[s_ptr++] = '9';
                s[s_ptr++] = ')';
                
                i += 2;
            }
            else if (input[i + 1] == 'w')
            {
                s[s_ptr++] = '(';
                char j;
                for (j = 'a'; j <= 'z'; j++)
                {
                    s[s_ptr++] = j;
                    s[s_ptr++] = '|';
                }
                for (j = 'A'; j < 'Z'; j++)
                {
                    s[s_ptr++] = j;
                    s[s_ptr++] = '|';
                }
                s[s_ptr++] = 'Z';
                s[s_ptr++] = ')';
                
                i += 2;
            }
            else if (input[i + 1] == 's')
            {
                s[s_ptr++] = '(';
                s[s_ptr++] = ' ';
                s[s_ptr++] = '|';
                s[s_ptr++] = '\\';
                s[s_ptr++] = 'n';
                s[s_ptr++] = '|';
                s[s_ptr++] = '\\';
                s[s_ptr++] = 't';
                s[s_ptr++] = '|';
                s[s_ptr++] = '\\';
                s[s_ptr++] = 'r';
                s[s_ptr++] = ')';
                
                i += 2;
            }
            else if (input[i + 1] == ' ')
            {
                s[s_ptr++] = ' ';
                
                i+= 2;
            }
            else
            {
                s[s_ptr++] = input[i++];
            }
        }
        else if (input[i] != ' ')
        {
            s[s_ptr++] = input[i++];
        }
        else
        {
            i++;
        }
    }
    s[s_ptr - 1] = '\0';
    
    char temp[BUFFER_SIZE];
    for (i = 0; i < semic_pos; i++)
    {
        temp[i] = input[i];
    }
    temp[i] = '\0';
    
    printf("    {\n        .abbrev = \"%s\",\n", temp);
    printf("        .list = (rule_token_t[])\n        {\n");
    
    char* last = "NT_LPAREN";
    print_token(last, NOT_CHAR);
    
    int size = 4;
    for (i = 0; i < strlen(s); )
    {
        size++;
        char* cur = "";
        char symbol = NOT_CHAR;
        if (s[i] == '\\')
        {
            if (s[i + 1] == 'e')
            {
                cur = "NT_EPS";
            }
            else if (s[i + 1] == 'n')
            {
                cur = "NT_CHAR";
                symbol = '\n';
            }
            else if (s[i + 1] == 't')
            {
                cur = "NT_CHAR";
                symbol = '\t';
            }
            else if (s[i + 1] == 'r')
            {
                cur = "NT_CHAR";
                symbol = '\r';
            }
            else
            {
                cur = "NT_CHAR";
                symbol = s[i + 1];
            }
            
            i += 2;
        }
        else
        {
            if (s[i] == '|')
            {
                cur = "NT_OR";
            }
            else if (s[i] == '(')
            {
                cur = "NT_LPAREN";
            }
            else if (s[i] == ')')
            {
                cur = "NT_RPAREN";
            }
            else if (s[i] == '*')
            {
                cur = "NT_STAR";
            }
            else
            {
                cur = "NT_CHAR";
                symbol = s[i];
            }
            
            i++;
        }
        
        if (!dont_need_cat(last, cur))
        {
            size++;
            print_token("NT_CAT", NOT_CHAR);
        }
        print_token(cur, symbol);
        
        last = cur;
    }
    
    print_token("NT_RPAREN", NOT_CHAR);
    print_token("NT_CAT", NOT_CHAR);
    print_token("NT_END", idx);
    
    printf("        },\n        .size = %d,\n    },\n", size);    
}


int main()
{
    printf(".rule = (lex_rule_t[])\n{\n");
    
    char input[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    int count = 0;
    while (fgets(temp, sizeof temp, stdin))
    {
        strcat(input, temp);
        parse_rule(temp, count);
        count++;
    }
    
    printf("},\n.count = %d,\n", count);
}
