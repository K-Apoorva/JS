all: js2ts

js2ts: parser.tab.c lex.yy.c symtable.c
	gcc -o js2ts parser.tab.c lex.yy.c symtable.c -w

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

run: js2ts
	./js2ts < input.js  && mv output.ts output1.ts
	./js2ts < input2.js && mv output.ts output2.ts
	./js2ts < input3.js && mv output.ts output3.ts
	./js2ts < input.js  | tee parse_trace_input.txt
	./js2ts < input2.js | tee parse_trace_input2.txt
	./js2ts < input3.js | tee parse_trace_input3.txt

clean:
	rm -f js2ts parser.tab.c parser.tab.h lex.yy.c parse_trace_input.txt parse_trace_input2.txt parse_trace_input3.txt
