#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define ATTEMPTS_PER_TEST 5
#define MAX_STATEMENT_SIZE 30 // < 62


char** statement = (char*[])
{
    "24.21",
    "50",
    "*",
    "310",
    "-",
    "755",
    "*",
    "x",
    "*",
    "x",
    "+",
    "19",
    "/",
    "69",
    "+",
    "x",
    "+",
    "49",
    "*",
    "12",
    "-",
    "5",
    "+",
    "4",
    "/",
    "x",
    "*",
    "7",
    "-",
    "10",
    "+",
    "x",
    "*",
    "x",
    "+",
    "12",
    "-",
    "313",
    "/",
    "x",
    "+",
    "4",
    "/",
    "9",
    "*",
    "2",
    "-",
    "x",
    "+",
    "5",
    "-",
    "16",
    "/",
    "8",
    "*",
    "x",
    "+",
    "3",
    "-",
    "77",
    "/",
}; 


long double calculate(long double x, int size)
{
    long double stack[100];
    int stack_size = 0;
    
    int i;
    for (i = 0; i < size; i++)
    {
        if (statement[i][0] == '+')
        {
            stack[stack_size - 2] += stack[stack_size - 1];
            stack_size--;
        }
        else if (statement[i][0] == '-')
        {
            stack[stack_size - 2] -= stack[stack_size - 1];
            stack_size--;
        }
        else if (statement[i][0] == '*')
        {
            stack[stack_size - 2] *= stack[stack_size - 1];
            stack_size--;
        }
        else if (statement[i][0] == '/')
        {
            stack[stack_size - 2] /= stack[stack_size - 1];
            stack_size--;
        }
        else if (statement[i][0] == 'x')
        {
            stack[stack_size++] = x;
        }
        else
        {
            stack[stack_size++] = (long double)atof(statement[i]);
        }
    }
    
    return stack[0];
}


void measure_time()
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
                calculate(x, len);
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
        printf("usual_rpn,%d,%.3lf\n", i, timer[i]);
    }
    printf("\n");
}


int main()
{
    printf("%Lf\n", calculate(12, 30));
    
    //measure_time();
    
    return 0;
}
