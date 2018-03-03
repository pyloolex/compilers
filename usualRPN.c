#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "rpn.h"


#define ATTEMPTS_PER_TEST 5
#define MAX_STATEMENT_SIZE 15  // < 21


long double calculate(char* statement[][MAX_STACK_DEPTH],
                      int* st_size, long double x, int len)
{
    long double stack[100];
    int stack_size = 0;
    
    int i;
    for (i = 0; i < st_size[len]; i++)
    {
        if (statement[len][i][0] == '+')
        {
            stack[stack_size - 2] += stack[stack_size - 1];
            stack_size--;
        }
        else if (statement[len][i][0] == '-')
        {
            stack[stack_size - 2] -= stack[stack_size - 1];
            stack_size--;
        }
        else if (statement[len][i][0] == '*')
        {
            stack[stack_size - 2] *= stack[stack_size - 1];
            stack_size--;
        }
        else if (statement[len][i][0] == '/')
        {
            stack[stack_size - 2] /= stack[stack_size - 1];
            stack_size--;
        }
        else if (statement[len][i][0] == 'x')
        {
            stack[stack_size++] = x;
        }
        else
        {
            stack[stack_size++] = (long double)atof(statement[len][i]);
        }
    }
    
    return stack[0];
}


void measure_time(char* statement[][MAX_STACK_DEPTH], int* st_size)
{
    double timer[MAX_STATEMENT_SIZE];
    int len;
    for (len = 0; len < MAX_STATEMENT_SIZE; len++)
    {
        printf("[len = %d]\n", len + 1);
        double sum = 0;
        int attempt;
        for (attempt = 0; attempt < ATTEMPTS_PER_TEST; attempt++)
        {
            double start_time = clock();
            
            int x;
            for (x = 0; x < 1e7; x++)
            {
                calculate(statement, st_size, x, len);
            }
            
            double elapsed = ((clock() - start_time) / CLOCKS_PER_SEC);
            sum += elapsed;
            
            printf("Elapsed time: %.3f sec\n", elapsed);
        }
        timer[len] = sum / attempt;
        
        printf("_________________________________\n");
        printf("Average elapsed time: %.3f sec\n", sum / attempt);
    }
    
    printf("\n");
    int i;
    for (i = 0; i < MAX_STATEMENT_SIZE; i++)
    {
        printf("usual_rpn,%d,%.3lf\n", i + 1, timer[i]);
    }
    printf("\n");
}


void test_for_12(char* statement[][MAX_STACK_DEPTH], int* st_size)
{
    int i;
    for (i = 0; i < MAX_STATEMENT_SIZE; i++)
    {
        printf("%Lf\n", calculate(statement, st_size, 12, i));
    }
}


int main()
{
    char* input[] =
    {
        #include "statements.h"
    };
    int st_size[MAX_STATEMENT_SIZE] = {};
    char* statement[MAX_STATEMENT_SIZE][MAX_STACK_DEPTH];

    fill_statement(input, statement, st_size, MAX_STATEMENT_SIZE);

    
    //test_for_12(statement, st_size);
    //printf("%Lf\n", calculate(statement, st_size, 12, 19));
    
    measure_time(statement, st_size);
    
    return 0;
}
