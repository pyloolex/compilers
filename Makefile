CFLAGS+=-I/usr/lib/gcc/x86_64-linux-gnu/5/include/ -L/usr/lib/gcc/x86_64-linux-gnu/5/
LDLIBS+=-lgccjit

all: jitcalc usualRPN recalc

jitcalc: jitcalc_Lf.c
	$(CC) $< -o $@ $(LDLIBS) $(CFLAGS)

usualRPN: usualRPN.c
	$(CC) $< -o $@



recalc: recalc.c syn_tables.h my_yylex.h lex_automaton.h executions.h
	$(CC) $< -lm -o $@


syn_tables.h: GT
	./$< > $@

GT: gen_tables.c grammar.h
	$(CC) $< -o $@

grammar.h: PG grammar.txt
	./$< < $(word 2, $^) > $@

PG: parse_grammar.c
	$(CC) $< -o $@


lex_automaton.h: BLA
	./$< > $@

BLA: build_lex_autom.c lex_sequence.h grammar.h
	$(CC) $< -o $@

lex_sequence.h: PLR lex_rules.txt
	./$< < $(word 2, $^) > $@

PLR: parse_lex_rules.c
	$(CC) $< -o $@


executions.h: BE grammar.txt
	./$< < $(word 2, $^) > $@

BE: build_executions.c
	$(CC) $< -o $@



clean:
	rm -f PG grammar.h GT syn_tables.h
	rm -f PLR lex_sequence.h BLA lex_automaton.h
	rm -f BE executions.h
	rm -f output.txt

