#include <libgccjit.h>

#include <stdio.h>

#define MAX_STACK_DEPTH 100

void push_to_stack(gcc_jit_context* ctxt, gcc_jit_block* block, 
                   gcc_jit_type* int_type, gcc_jit_function* jit_atoi,
                   gcc_jit_lvalue* stack, gcc_jit_lvalue* stack_size,
                   gcc_jit_lvalue* elem)
{
    gcc_jit_block_add_assignment(block, NULL,
        gcc_jit_context_new_array_access(ctxt, NULL,
            gcc_jit_lvalue_as_rvalue(stack),
            gcc_jit_lvalue_as_rvalue(stack_size)),
        gcc_jit_context_new_call(ctxt, NULL, jit_atoi, 1, 
            (gcc_jit_rvalue**)&elem));
            
    gcc_jit_block_add_assignment_op(block, NULL,
        stack_size,
        GCC_JIT_BINARY_OP_PLUS,
        gcc_jit_context_one(ctxt, int_type));
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
        
    gcc_jit_param* arg[3] = {x_val, seq_size, sequence};
    
    gcc_jit_function* func = 
        gcc_jit_context_new_function(ctxt, NULL,
                                     GCC_JIT_FUNCTION_EXPORTED,
                                     int_type,
                                     "calculate",
                                     3, arg,
                                     0);

    /* Auxiliary internal functions */
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
    gcc_jit_block* b_number = 
        gcc_jit_function_new_block(func, "b_number");
    gcc_jit_block* b_cycle_end = 
        gcc_jit_function_new_block(func, "b_cycle_end");
    gcc_jit_block* b_finish = 
        gcc_jit_function_new_block(func, "b_finish");
        
    /* Init block */
    gcc_jit_lvalue* i = gcc_jit_function_new_local(func, NULL, int_type, "i");
    gcc_jit_block_add_assignment(b_init, NULL, i, 
                                 gcc_jit_context_zero(ctxt, int_type));
    gcc_jit_lvalue* stack = gcc_jit_function_new_local(func, NULL, 
                                                       stack_type, "stack");
    gcc_jit_lvalue* stack_size = gcc_jit_function_new_local(func, NULL,
        int_type, "stack_size");
    gcc_jit_block_add_assignment(b_init, NULL, stack_size,
                                 gcc_jit_context_zero(ctxt, int_type));
    gcc_jit_block_end_with_jump(b_init, NULL, b_cycle_cond_end);
    
    /* Cycle_cond_end block */  
    gcc_jit_block_end_with_conditional(
        b_cycle_cond_end, NULL,
        gcc_jit_context_new_comparison(ctxt, NULL, 
            GCC_JIT_COMPARISON_LT,
            gcc_jit_lvalue_as_rvalue(i),
            gcc_jit_param_as_rvalue(seq_size)),
        b_cycle_cond_plus,
        b_finish);
                                       
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
    
    gcc_jit_rvalue* plus_string = gcc_jit_context_new_string_literal(ctxt, "+");
    
    gcc_jit_block_end_with_conditional(
        b_cycle_cond_plus, NULL,
        gcc_jit_context_new_comparison(ctxt, NULL,
            GCC_JIT_COMPARISON_EQ,
            gcc_jit_lvalue_as_rvalue(gcc_jit_context_new_array_access(
                ctxt, NULL, gcc_jit_lvalue_as_rvalue(elem), gcc_jit_context_zero(
                    ctxt, int_type))),
            gcc_jit_lvalue_as_rvalue(gcc_jit_context_new_array_access(
                ctxt, NULL, plus_string, gcc_jit_context_zero(
                    ctxt, int_type)))),
        b_plus,
        b_number
    );
    
    /* Plus block */
    gcc_jit_lvalue* plus_sign = 
        gcc_jit_function_new_local(func, NULL, 
                                   const_char_ptr_type, "plus_sign");
                                   
    gcc_jit_block_add_assignment(b_plus, NULL, 
        plus_sign,
        gcc_jit_context_new_string_literal(ctxt, "777"));
    
    push_to_stack(ctxt, b_plus, int_type, jit_atoi, stack, 
                  stack_size, plus_sign);
    
    gcc_jit_block_end_with_jump(b_plus, NULL, b_cycle_end);
    
    /* Number block */
    push_to_stack(ctxt, b_number, int_type, jit_atoi, stack, 
                  stack_size, elem);
    
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
                gcc_jit_context_new_rvalue_from_int(ctxt, int_type, 2))));
    
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
    
    char** args = (char*[]){"20", "50", "+"};
    typedef int (*fn_type)(int, int, char**);
    fn_type calculate = (fn_type)fn_ptr;
    printf("Hi! Result: %d\n", calculate(11, 3, args));
    
    
    return 0;
}