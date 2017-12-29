#include <stdio.h>
#include <time.h>

#define ATTEMPTS_PER_TEST 15
#define MAX_STATEMENT_SIZE 30 // < 62

int calculate(int x, int size, char** arr)
{
    int stack[100];
    int stack_size = 0;
    
    int i;
    for (i = 0; i < size; i++)
    {
        if (arr[i][0] == '+')
        {
            stack[stack_size - 2] += stack[stack_size - 1];
            stack_size--;
        }
        else if (arr[i][0] == '-')
        {
            stack[stack_size - 2] -= stack[stack_size - 1];
            stack_size--;
        }
        else if (arr[i][0] == '*')
        {
            stack[stack_size - 2] *= stack[stack_size - 1];
            stack_size--;
        }
        else if (arr[i][0] == '/')
        {
            stack[stack_size - 2] /= stack[stack_size - 1];
            stack_size--;
        }
        else if (arr[i][0] == 'x')
        {
            stack[stack_size++] = x;
        }
        else
        {
            stack[stack_size++] = atoi(arr[i]);
        }
    }
    
    return stack[0];
}


void measure_time(char** args)
{
    double result[MAX_STATEMENT_SIZE];
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
                calculate(x, len, args);
            }
            
            double elapsed = ((clock() - start_time) / CLOCKS_PER_SEC);
            sum += elapsed;
            
            printf("Elapsed time: %.3f ms\n", elapsed);
        }
        printf("_________________________________\n");
        printf("Average elapsed time: %.3f ms\n", sum / attempt);
        result[len] = sum / attempt;
    }
    
    printf("\n");
    int i;
    for (i = 1; i < MAX_STATEMENT_SIZE; i += 2)
    {
        printf("(%d;%.3f)", i, result[i]);
    }
    printf("\n");
}


int main()
{
    /*
    char** args = (char*[])
    {
        "50",
        "x",
        "3",
        "*",
        "+",
        "2",
        "-",
        "7",
        "/",
        "6",
        "/",
        "6",
        "4",
        "x",
        "*",
        "-",
        "+",
    }; // x=12 -> -40
    */
    char** args = (char*[])
    {
        "24",
        "50",
        "*",
        "310",
        "-",
        "755",
        "*",
        "x",
        "+",
        "x",
        "-",
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
    
    
    printf("%d\n", calculate(12, 61, args));
    
    //measure_time(args);
    
    return 0;
}