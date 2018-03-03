#define MAX_STACK_DEPTH 100


void fill_statement(char** input,
                    char* statement[][MAX_STACK_DEPTH],
                    int* st_size, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        char stack[MAX_STACK_DEPTH];
        int stack_ptr = 0;

        int j;
        char temp[MAX_STACK_DEPTH];
        int temp_ptr = 0;
        for (j = 0; j < strlen(input[i]); j++)
        {
            if (isdigit(input[i][j]) || input[i][j] == '.')
            {                
                temp[temp_ptr++] = input[i][j];
            }
            else if (input[i][j] == 'x')
            {
                statement[i][st_size[i]] = malloc(2);
                statement[i][st_size[i]][0] = 'x';
                statement[i][st_size[i]++][1] = 0;
            }
            else
            {
                if (temp_ptr)
                {                    
                    temp[temp_ptr] = 0;
                    statement[i][st_size[i]] = malloc(strlen(temp));
                    strcpy(statement[i][st_size[i]++], temp);
                    temp_ptr = 0;            
                }
                if (input[i][j] == ' ') continue;
                else if (input[i][j] == ')')
                {
                    while (!(stack[stack_ptr - 1] == '('))
                    {
                        statement[i][st_size[i]] = malloc(2);
                        statement[i][st_size[i]][0] = stack[--stack_ptr];
                        statement[i][st_size[i]++][1] = 0;
                    }
                    stack_ptr--;
                }
                else
                {
                    char cur = input[i][j];
                    while (true)
                    {
                        char last;
                        if (stack_ptr)
                            last = stack[stack_ptr - 1];
                        else break;
                        
                        if ((cur == '+' || cur == '-') && last != '(' ||
                            (cur == '*' || cur == '/') &&
                            (last == '*' || last == '/'))
                        {
                            statement[i][st_size[i]] = malloc(2);
                            statement[i][st_size[i]][0] = last;
                            statement[i][st_size[i]++][1] = 0;
                        }
                        else break;
                        stack_ptr--;
                    }
                    stack[stack_ptr++] = input[i][j];
                }
            }
        }

        if (temp_ptr)
        {                    
            temp[temp_ptr] = 0;
            statement[i][st_size[i]] = malloc(strlen(temp));
            strcpy(statement[i][st_size[i]++], temp);
            temp_ptr = 0;            
        }

        while (stack_ptr)
        {           
            statement[i][st_size[i]] = malloc(2);
            statement[i][st_size[i]][0] = stack[--stack_ptr];
            statement[i][st_size[i]++][1] = 0;
        }
    }   
}
