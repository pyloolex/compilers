#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "values.h"


void print_items(grammar_t* grammar, item_list_t* scheme, transit_t* transit)
{
    int i;
    for (i = 0; i < transit->size; i++)
    {
        printf("(%d)----------------\n", i);
        int j;
        for (j = 0; j < scheme[i].size; j++)
        {
            item_t item = scheme[i].list[j];
            printf("%s -> ", grammar->token_names.
                   list[grammar->prod[item.prod].left]);
            
            int k;
            for (k = 0; k != item.pos; k++)
            {
                printf("%s ", grammar->token_names.
                       list[grammar->prod[item.prod].list[k]]);
            }
            printf("* ");
            for ( ; k < grammar->prod[item.prod].size; k++)
            {
                printf("%s ", grammar->token_names.
                       list[grammar->prod[item.prod].list[k]]);
            }
            printf("| %s\n", grammar->token_names.list[item.end]);
        }
    }
    
    printf("----------\n");
    for (i = 0; i < transit->size; i++)
    {
        int j;
        for (j = 0; j < grammar->token_names.size; j++)
        {
            table_cell_t cur = transit->list[i][j];
            switch (cur.action)
            {
            case AC_SHIFT:
                printf("s%02d ", cur.num);
                break;
            case AC_REDUCE:
                printf("r%02d ", cur.num);
                break;
            case AC_ACCEPT:
                printf("acc ");
                break;
            case AC_ERROR:
                printf("    ");
                break;
            }
        }
        printf("\n");
    }
}

bool contains(first_t* first, int elem)
{
    int i;
    for (i = 0; i < first->size; i++)
    {
        if (first->list[i] == elem)
        {
            return true;
        }
    }
    
    return false;
}


void merge_first_sets(first_t* left, first_t* right)
{
    int i;
    for (i = 0; i < right->size; i++)
    {
        if (!contains(left, right->list[i]))
        {
            left->list[left->size++] = right->list[i];
        }
    }
}


void calc_first(token_id_t token, grammar_t* grammar, first_t* first)
{
    if (first[token].size)
    {
        return;
    }
    
    bool is_terminal = true;
    int i;
    for (i = 0; i < grammar->size; i++)
    {
        if (grammar->prod[i].left == token && 
            grammar->prod[i].list[0] != token)
        {
            is_terminal = false;
            int neigh = grammar->prod[i].list[0];
            calc_first(neigh, grammar, first);
            merge_first_sets(&first[token], &first[neigh]);
        }
    }
    
    if (is_terminal)
    {
        first[token].list[first[token].size++] = token;
    }
}


void malloc_added(bool*** added, int tokens_count)
{
    *added = malloc(tokens_count * sizeof(bool*));
    int i;
    for (i = 0; i < tokens_count; i++)
    {
        (*added)[i] = malloc(tokens_count * sizeof(bool));
        int j;
        for (j = 0; j < tokens_count; j++)
        {
            (*added)[i][j] = false;
        }
    }
}


void add_items(token_id_t head, token_id_t end, item_list_t* set, bool** added,
               grammar_t* grammar)
{
    if (added[head][end]) return;
    
    int i;
    for (i = 0; i < grammar->size; i++)
    {
        if (grammar->prod[i].left == head)
        {
            set->list[set->size].prod = i;
            set->list[set->size].pos = 0;
            set->list[set->size].end = end;
            set->size++;
        }
    }
    
    added[head][end] = true;
}


int comparator(const void* f, const void* s)
{
	item_t* a = ((item_t*) f);
	item_t* b = ((item_t*) s);
	if (a->pos != b->pos)
	{
	    return b->pos - a->pos;
	}
	if (a->prod != b->prod)
	{
	    return a->prod - b->prod;
	}
	return a->end - b->end;
}


void closure(int state, grammar_t* grammar, item_list_t* scheme, first_t* first)
{
    bool** added;
    malloc_added(&added, grammar->token_names.size);
    
    int i;
    for (i = 0; i < scheme[state].size; i++)
    {
        item_t item = scheme[state].list[i];
        if (item.pos == grammar->prod[item.prod].size) continue;
        token_id_t head = grammar->prod[item.prod].list[item.pos];
        if (item.pos + 1 < grammar->prod[item.prod].size)
        {
            token_id_t neigh = grammar->prod[item.prod].list[item.pos + 1];
            calc_first(neigh, grammar, first);
            int j;
            for (j = 0; j < first[neigh].size; j++)
            {
                add_items(head, first[neigh].list[j], &scheme[state], added, 
                          grammar);
            }
        }
        else
        {
            add_items(head, item.end, &scheme[state], added, grammar);
        }
    }
    
    qsort(scheme[state].list, scheme[state].size,
		  sizeof(item_t), comparator);
}


bool is_equal_states(item_list_t* a, item_list_t* b)
{
	if (a->size != b->size)
	{
		return false;
	}

	int i;
	for (i = 0; i < a->size; i++)
	{
		if (a->list[i].prod != b->list[i].prod ||
			a->list[i].pos != b->list[i].pos ||
			a->list[i].end != b->list[i].end)
		{
			return false;
		}
	}

	return true;
}


void make_transitions(int state, token_id_t token, item_list_t* scheme, 
                      transit_t* transit, grammar_t* grammar, first_t* first)
{
    scheme[transit->size].size = 0;
    bool is_empty = true;
    int i;
    for (i = 0; i < scheme[state].size; i++)
    {
        item_t item = scheme[state].list[i];
        if (item.pos < grammar->prod[item.prod].size &&
            grammar->prod[item.prod].list[item.pos] == token)
        {
            scheme[transit->size].list[scheme[transit->size].size] = item;
            scheme[transit->size].list[scheme[transit->size].size].pos++;
            scheme[transit->size].size++;
            is_empty = false;
        }
    }
    
    if (is_empty)
    {
        return;
    }
    
    closure(transit->size, grammar, scheme, first);
    transit->list[state][token].action = AC_SHIFT;
    
    bool is_new_state = true;
    for (i = 0; i < transit->size; i++)
    {
        if (is_equal_states(&scheme[transit->size], &scheme[i]))
        {
            is_new_state = false;
            transit->list[state][token].num = i;
            break;
        }
    }
    
    if (is_new_state)
    {
        transit->list[state][token].num = transit->size++;
    }
}


void find_reduce(int state, grammar_t* grammar, item_list_t* scheme, 
                 transit_t* transit)
{
    int i;
    for (i = 0; i < scheme[state].size; i++)
    {
        item_t item = scheme[state].list[i];
        if (item.pos == grammar->prod[item.prod].size)
        {
            if (transit->list[state][item.end].action != AC_ERROR)
            {
                printf("[gen_tables.c] Invalid grammar. ");
                printf("The conflict for {%d, %s}\n", state,
                       grammar->token_names.list[item.end]);
            }
            
            if (item.prod == 0)
            {
                transit->list[state][item.end].action = AC_ACCEPT;
            }
            else
            {
                transit->list[state][item.end].action = AC_REDUCE;
                transit->list[state][item.end].num = item.prod;
            }
        }
    }
}


void build_automaton(grammar_t* grammar, item_list_t* scheme,
                     transit_t* transit, first_t* first)
{
    
    scheme[0].list[0].prod =
        scheme[0].list[0].pos =
        scheme[0].list[0].end = 0;
    scheme[0].size = 1;
    transit->size = 1;
    closure(0, grammar, scheme, first);
    
    int i;
    for (i = 0; i < transit->size; i++)
    {
        int j;
        for (j = 0; j < grammar->token_names.size; j++)
        {
            make_transitions(i, j, scheme, transit, grammar, first);
        }
        
        find_reduce(i, grammar, scheme, transit);
    }
}


void malloc_first(first_t** first, int tokens_count)
{
    *first = malloc(tokens_count * sizeof(first_t*));
    int i;
    for (i = 0; i < tokens_count; i++)
    {
        (*first)[i].list = malloc(tokens_count * sizeof(int));
    }
}


void beautify_tables(grammar_t* grammar, transit_t* transit)
{
    printf(".grammar_left = (token_id_t[])\n{\n    ");
    int i;
    for (i = 0; i < grammar->size; i++)
    {
        printf("%d, ", grammar->prod[i].left);
    }
    printf("\n},\n");
    
    printf(".grammar_size = (int[])\n{\n    ");
    for (i = 0; i < grammar->size; i++)
    {
        printf("%d, ", grammar->prod[i].size);
    }
    printf("\n},\n");
    
    printf(".trans = (table_cell_t*[])\n{\n");
    for (i = 0; i < transit->size; i++)
    {
        printf("    (table_cell_t[])\n    {\n");
        int j;
        for (j = 0; j < grammar->token_names.size; j++)
        {
            table_cell_t* cell = &transit->list[i][j];
            printf("        {");
            switch (cell->action)
            {
                case AC_SHIFT:
                    printf("AC_SHIFT, %d", cell->num);
                    break;
                case AC_REDUCE:
                    printf("AC_REDUCE, %d", cell->num);
                    break;
                case AC_ACCEPT:
                    printf("AC_ACCEPT, 0");
                    break;
                case AC_ERROR:
                    printf("AC_ERROR, 0");
                    break;
            }
            printf("},\n");
        }
        printf("    },\n");
    }
    printf("},\n");
}


int main()
{
    grammar_t grammar = 
    {
        #include "grammar.h"
    };
    
    first_t* first;
    malloc_first(&first, grammar.token_names.size);
    
    transit_t transit;
    int i;
    for (i = 0; i < MAX_STATES; i++)
    {
        int j;
        for (j = 0; j < MAX_TOKENS; j++)
        {
            transit.list[i][j].action = AC_ERROR;
        }
    }
    
    item_list_t scheme[MAX_STATES];
    build_automaton(&grammar, scheme, &transit, first);    
    
    beautify_tables(&grammar, &transit);
}
