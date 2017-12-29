#include <libgccjit.h>

#include <stdio.h>
#include <time.h>


#define MAX_STACK_DEPTH 100


void push_to_stack(gcc_jit_context* ctxt, gcc_jit_block* block, 
                   gcc_jit_type* int_type, gcc_jit_lvalue* stack, 
                   gcc_jit_lvalue* stack_size, gcc_jit_rvalue* number)
{
    gcc_jit_block_add_assignment(block, NULL,
        gcc_jit_context_new_array_access(ctxt, NULL,
            gcc_jit_lvalue_as_rvalue(stack),
            gcc_jit_lvalue_as_rvalue(stack_size)),
        number
    );
            
    gcc_jit_block_add_assignment_op(block, NULL,
        stack_size,
        GCC_JIT_BINARY_OP_PLUS,
        gcc_jit_context_one(ctxt, int_type)
    );
}


void get_from_stack(gcc_jit_context* ctxt, gcc_jit_block* block, 
                    gcc_jit_type* int_type, gcc_jit_lvalue* stack,
                    gcc_jit_lvalue* stack_size, gcc_jit_lvalue* number)
{
    gcc_jit_block_add_assignment_op(block, NULL,
        stack_size,
        GCC_JIT_BINARY_OP_MINUS,
        gcc_jit_context_one(ctxt, int_type)
    );
        
    gcc_jit_block_add_assignment(block, NULL,
        number,
        gcc_jit_lvalue_as_rvalue(gcc_jit_context_new_array_access(ctxt, NULL,
            gcc_jit_lvalue_as_rvalue(stack),
            gcc_jit_lvalue_as_rvalue(stack_size))
        )
    );
}


void condition_block(gcc_jit_context* ctxt, gcc_jit_block* cur_block,
                     gcc_jit_block* block_true, gcc_jit_block* block_false,
                     gcc_jit_type* int_type, char* sign, gcc_jit_rvalue* input)
{
    gcc_jit_rvalue* operation = 
        gcc_jit_context_new_string_literal(ctxt, sign);
    gcc_jit_block_end_with_conditional(
        cur_block, NULL,
        gcc_jit_context_new_comparison(ctxt, NULL,
            GCC_JIT_COMPARISON_EQ,
            gcc_jit_lvalue_as_rvalue(gcc_jit_context_new_array_access(
                ctxt, NULL, input, gcc_jit_context_zero(
                    ctxt, int_type))),
            gcc_jit_lvalue_as_rvalue(gcc_jit_context_new_array_access(
                ctxt, NULL, operation, gcc_jit_context_zero(
                    ctxt, int_type)))),
        block_true,
        block_false
    );
}


void operation_block(gcc_jit_context* ctxt, gcc_jit_block* block,
                     gcc_jit_block* b_cycle_end, gcc_jit_type* int_type,
                     gcc_jit_lvalue* stack, gcc_jit_lvalue* stack_size,
                     gcc_jit_lvalue* A, gcc_jit_lvalue* B,
                     enum gcc_jit_binary_op operation)
{
    get_from_stack(ctxt, block, int_type, stack, stack_size, B);
    get_from_stack(ctxt, block, int_type, stack, stack_size, A);
    
    gcc_jit_block_add_assignment_op(block, NULL,
        A,
        operation,
        gcc_jit_lvalue_as_rvalue(B)
    );
    
    push_to_stack(ctxt, block, int_type, stack, stack_size, 
        gcc_jit_lvalue_as_rvalue(A));
    
    gcc_jit_block_end_with_jump(block, NULL, b_cycle_end);
}


void create_code(gcc_jit_context* ctxt)
{   
    /* Types declaration */
    gcc_jit_type* int_type = 
        gcc_jit_context_get_type(ctxt, GCC_JIT_TYPE_INT);
    gcc_jit_type* const_char_ptr_type = 
        gcc_jit_context_get_type(ctxt, GCC_JIT_TYPE_CONST_CHAR_PTR);
    gcc_jit_type* char_array_type = 
        gcc_jit_type_get_pointer(const_char_ptr_type);
    gcc_jit_type* stack_type = 
        gcc_jit_context_new_array_type(ctxt, NULL, int_type, MAX_STACK_DEPTH);
    
    /* Function declaration */
    gcc_jit_param* x_val = 
        gcc_jit_context_new_param(ctxt, NULL, int_type, "x_val");
    gcc_jit_param* seq_size = 
        gcc_jit_context_new_param(ctxt, NULL, int_type, "seq_size");
    gcc_jit_param* sequence = 
        gcc_jit_context_new_param(ctxt, NULL, char_array_type, "sequence");
        
    gcc_jit_param* arg[3] = 
        {x_val, seq_size, sequence};
    gcc_jit_function* func = 
        gcc_jit_context_new_function(ctxt, NULL,
                                     GCC_JIT_FUNCTION_EXPORTED,
                                     int_type,
                                     "calculate",
                                     3, arg,
                                     0);

    /* Auxiliary imported functions */
    gcc_jit_param* atoi_param =
        gcc_jit_context_new_param(ctxt, NULL, const_char_ptr_type, "atoi_param");
    gcc_jit_function* jit_atoi = 
        gcc_jit_context_new_function(ctxt, NULL,
                                     GCC_JIT_FUNCTION_IMPORTED,
                                     int_type,
                                     "atoi",
                                     1, &atoi_param,
                                     0);

    /* Blocks declaration */
    gcc_jit_block* b_init = 
        gcc_jit_function_new_block(func, "b_init");
    gcc_jit_block* b_cycle_cond_end = 
        gcc_jit_function_new_block(func, "b_cycle_cond_end");
    gcc_jit_block* b_cycle_cond_plus = 
        gcc_jit_function_new_block(func, "b_cycle_cond_plus");
    gcc_jit_block* b_plus = 
        gcc_jit_function_new_block(func, "b_plus");
    gcc_jit_block* b_cycle_cond_minus = 
        gcc_jit_function_new_block(func, "b_cycle_cond_minus");
    gcc_jit_block* b_minus = 
        gcc_jit_function_new_block(func, "b_minus");
    gcc_jit_block* b_cycle_cond_mul = 
        gcc_jit_function_new_block(func, "b_cycle_cond_mul");
    gcc_jit_block* b_mul = 
        gcc_jit_function_new_block(func, "b_mul");
    gcc_jit_block* b_cycle_cond_div = 
        gcc_jit_function_new_block(func, "b_cycle_cond_div");
    gcc_jit_block* b_div = 
        gcc_jit_function_new_block(func, "b_div");
    gcc_jit_block* b_cycle_cond_x = 
        gcc_jit_function_new_block(func, "b_cycle_cond_x");
    gcc_jit_block* b_x = 
        gcc_jit_function_new_block(func, "b_x");
    gcc_jit_block* b_number = 
        gcc_jit_function_new_block(func, "b_number");
    gcc_jit_block* b_cycle_end = 
        gcc_jit_function_new_block(func, "b_cycle_end");
    gcc_jit_block* b_finish = 
        gcc_jit_function_new_block(func, "b_finish");
        
    /* Init block */
    gcc_jit_lvalue* i = 
        gcc_jit_function_new_local(func, NULL, int_type, "i");
    gcc_jit_block_add_assignment(b_init, NULL, i, 
                                 gcc_jit_context_zero(ctxt, int_type));
    gcc_jit_lvalue* stack = gcc_jit_function_new_local(func, NULL, 
                                                       stack_type, "stack");
    gcc_jit_lvalue* stack_size = gcc_jit_function_new_local(func, NULL,
        int_type, "stack_size");
    gcc_jit_block_add_assignment(b_init, NULL, stack_size,
                                 gcc_jit_context_zero(ctxt, int_type));
    gcc_jit_block_end_with_jump(b_init, NULL, b_cycle_cond_end);
    gcc_jit_lvalue* A = gcc_jit_function_new_local(func, NULL, int_type, "A");
    gcc_jit_lvalue* B = gcc_jit_function_new_local(func, NULL, int_type, "B");
    
    /* Cycle_cond_end block */  
    gcc_jit_block_end_with_conditional(
        b_cycle_cond_end, NULL,
        gcc_jit_context_new_comparison(ctxt, NULL, 
            GCC_JIT_COMPARISON_LT,
            gcc_jit_lvalue_as_rvalue(i),
            gcc_jit_param_as_rvalue(seq_size)),
        b_cycle_cond_plus,
        b_finish
    );
                                       
    /* Cycle_cond_plus block */
    gcc_jit_lvalue* elem = 
        gcc_jit_function_new_local(func, NULL, 
                                   const_char_ptr_type, "elem");
    gcc_jit_block_add_assignment(b_cycle_cond_plus, NULL, 
        elem,
        gcc_jit_lvalue_as_rvalue(
            gcc_jit_context_new_array_access(ctxt, NULL, 
                gcc_jit_param_as_rvalue(sequence), 
                gcc_jit_lvalue_as_rvalue(i))
        )
    );
    
    condition_block(ctxt, b_cycle_cond_plus, b_plus, b_cycle_cond_minus,
                    int_type, "+", gcc_jit_lvalue_as_rvalue(elem));
    
    /* Plus block */
    operation_block(ctxt, b_plus, b_cycle_end, int_type, stack, stack_size,
                    A, B, GCC_JIT_BINARY_OP_PLUS);
    
    /* Cycle_cond_minus block */
    condition_block(ctxt, b_cycle_cond_minus, b_minus, b_cycle_cond_mul,
                    int_type, "-", gcc_jit_lvalue_as_rvalue(elem));
    
    /* Minus block */
    operation_block(ctxt, b_minus, b_cycle_end, int_type, stack, stack_size,
                    A, B, GCC_JIT_BINARY_OP_MINUS);
                    
    /* Cycle_cond_mul block */
    condition_block(ctxt, b_cycle_cond_mul, b_mul, b_cycle_cond_div,
                    int_type, "*", gcc_jit_lvalue_as_rvalue(elem));
    
    /* Mul block */
    operation_block(ctxt, b_mul, b_cycle_end, int_type, stack, stack_size,
                    A, B, GCC_JIT_BINARY_OP_MULT);
                    
    /* Cycle_cond_div block */
    condition_block(ctxt, b_cycle_cond_div, b_div, b_cycle_cond_x,
                    int_type, "/", gcc_jit_lvalue_as_rvalue(elem));
    
    /* Div block */
    operation_block(ctxt, b_div, b_cycle_end, int_type, stack, stack_size,
                    A, B, GCC_JIT_BINARY_OP_DIVIDE);
    
    /* Cycle_cond_x block */
    condition_block(ctxt, b_cycle_cond_x, b_x, b_number, int_type, "x",
                    gcc_jit_lvalue_as_rvalue(elem));
    
    /* X block */
    push_to_stack(ctxt, b_x, int_type, stack, stack_size, 
        gcc_jit_param_as_rvalue(x_val));
    
    gcc_jit_block_end_with_jump(b_x, NULL, b_cycle_end);
    
    /* Number block */
    gcc_jit_lvalue* number = gcc_jit_function_new_local(func, NULL,
                                                        int_type, "number");
    gcc_jit_block_add_assignment(b_number, NULL,
        number,
        gcc_jit_context_new_call(ctxt, NULL, jit_atoi, 1, 
            (gcc_jit_rvalue**)&elem));
    
    push_to_stack(ctxt, b_number, int_type, stack, stack_size, 
        gcc_jit_lvalue_as_rvalue(number));
    
    gcc_jit_block_end_with_jump(b_number, NULL, b_cycle_end);
    
    /* Cycle_end block */
    gcc_jit_block_add_assignment_op(
        b_cycle_end, NULL,
        i,
        GCC_JIT_BINARY_OP_PLUS,
        gcc_jit_context_one(ctxt, int_type));
    gcc_jit_block_end_with_jump(b_cycle_end, NULL, b_cycle_cond_end);
    
    /* Finish block */
    gcc_jit_lvalue* answer = 
        gcc_jit_function_new_local(func, NULL, 
                                   int_type, "answer");
    
    gcc_jit_block_add_assignment(
        b_finish, NULL,
        answer,
        gcc_jit_lvalue_as_rvalue(
            gcc_jit_context_new_array_access(ctxt, NULL,
                gcc_jit_lvalue_as_rvalue(stack),
                gcc_jit_context_new_rvalue_from_int(ctxt, int_type, 0))
        )
    );
    
    gcc_jit_block_end_with_return(b_finish, NULL, 
        gcc_jit_lvalue_as_rvalue(answer));
}


int main()
{
    gcc_jit_context* ctxt;
    
    ctxt = gcc_jit_context_acquire();
    if (!ctxt)
    {
        printf("Bad ctxt\n");
        return 1;
    }
    
    create_code(ctxt);
    
    gcc_jit_result* result = gcc_jit_context_compile(ctxt);
    if (!result)
    {
        printf("Bad result\n");
        return 1;
    }
    
    gcc_jit_context_release(ctxt);
    ctxt = NULL;
    
    void* fn_ptr = gcc_jit_result_get_code(result, "calculate");
    if (!fn_ptr)
    {
        printf("Bad fn_ptr\n");
        return 1;
    }
    
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
    
    typedef int (*fn_type)(int, int, char**);
    fn_type calculate = (fn_type)fn_ptr;
    
    double start_time = clock();
    //printf("Hi! Result: %d\n", calculate(12, 17, args));
    
    int i = 0;
    for (i = 0; i < 1e7; i++)
    {
        calculate(i, 17, args);
    }
    
    printf("Elapsed time: %f ms\n", ((clock() - start_time) / CLOCKS_PER_SEC));
    
    
    return 0;
}