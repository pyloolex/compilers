#include <libgccjit.h>

#include <stdio.h>

void create_code(gcc_jit_context* ctxt)
{
    gcc_jit_type* int_type = 
        gcc_jit_context_get_type(ctxt, GCC_JIT_TYPE_INT);
    gcc_jit_type* const_char_ptr_type = 
        gcc_jit_context_get_type(ctxt, GCC_JIT_TYPE_CONST_CHAR_PTR);
    gcc_jit_type* array = 
        gcc_jit_type_get_pointer(const_char_ptr_type);
        
    gcc_jit_rvalue* two = 
        gcc_jit_context_new_rvalue_from_int(ctxt, int_type, 2);
    
    gcc_jit_param* x_val = 
        gcc_jit_context_new_param(ctxt, NULL, int_type, "x_val");
    gcc_jit_param* seq_size = 
        gcc_jit_context_new_param(ctxt, NULL, int_type, "seq_size");
    gcc_jit_param* sequence = 
        gcc_jit_context_new_param(ctxt, NULL, array, "sequence");
        
    gcc_jit_param* arg[3] = {x_val, seq_size, sequence};
    
    gcc_jit_function* func = 
        gcc_jit_context_new_function(ctxt, NULL,
                                     GCC_JIT_FUNCTION_EXPORTED,
                                     int_type,
                                     "calculate",
                                     3, arg,
                                     0);

    gcc_jit_param* atoi_arg =
        gcc_jit_context_new_param(ctxt, NULL, const_char_ptr_type, "atoi_arg");
    gcc_jit_function* jit_atoi = 
        gcc_jit_context_new_function(ctxt, NULL,
                                     GCC_JIT_FUNCTION_IMPORTED,
                                     int_type,
                                     "atoi",
                                     1, &atoi_arg,
                                     0);

    gcc_jit_block* block = gcc_jit_function_new_block(func, NULL);
    
    gcc_jit_rvalue* answer = 
        gcc_jit_param_as_rvalue(x_val);
    
    /*
    gcc_jit_rvalue* arg_for_atoi = 
        gcc_jit_context_new_array_access(ctxt, NULL, 
            gcc_jit_param_as_rvalue(arg), two);

        
    
    gcc_jit_lvalue* after_atoi = 
        gcc_jit_function_new_local(func, NULL, int_type, "after_atoi");
    
    gcc_jit_block_add_assignment(block, NULL,
        after_atoi,
        gcc_jit_context_new_call(ctxt, NULL,
                                 jit_atoi, 
                                 1, &arg_for_atoi));
                                 
                                 */
    
    /*
    gcc_jit_block_add_assignment(block, NULL,
        after_atoi,
        gcc_jit_context_new_call(ctxt, NULL,
                                 jit_atoi, 
                                 1, &arg_for_atoi));
    */
    
    
    /*
    gcc_jit_rvalue* answer =
        gcc_jit_context_new_binary_op(ctxt, NULL,
            GCC_JIT_BINARY_OP_PLUS, int_type,
            gcc_jit_lvalue_as_rvalue(after_atoi),
            gcc_jit_lvalue_as_rvalue(gcc_jit_context_new_array_access(ctxt, NULL,
                    gcc_jit_param_as_rvalue(param2), gcc_jit_context_one)));
    */    
    
    
    
        
    gcc_jit_block_end_with_return(block, NULL, answer);
    
    /*
    gcc_jit_rvalue* answer =
        gcc_jit_context_new_binary_op(ctxt, NULL,
            GCC_JIT_BINARY_OP_PLUS, int_type,
            gcc_jit_lvalue_as_rvalue(gcc_jit_rvalue_dereference(gcc_jit_param_as_rvalue(param2), NULL)),
            gcc_jit_lvalue_as_rvalue(gcc_jit_context_new_array_access(ctxt, NULL,
                    gcc_jit_param_as_rvalue(param2), one)));
    */
    
    //gcc_jit_context_new_rvalue_from_int(ctxt, int_type, 51);
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
    
    char** args = (char*[]){"20", "3"};
    typedef int (*fn_type)(int, int, char**);
    fn_type calculate = (fn_type)fn_ptr;
    printf("Hi! Result: %d\n", calculate(11, 33, args));
    
    
    return 0;
}