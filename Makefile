all: js2ts

js2ts: parser.tab.c lex.yy.c ast.c
	gcc -o js2ts parser.tab.c lex.yy.c ast.c -w

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

run: js2ts
	./js2ts trace_input1.txt input.js  < input.js  && mv output.ts output1.ts
	./js2ts trace_input2.txt input2.js < input2.js && mv output.ts output2.ts
	./js2ts trace_input3.txt input3.js < input3.js && mv output.ts output3.ts
	@echo ""
	@echo "── Error input test ────────────────────────────────────"
	./js2ts /dev/null input_error.js < input_error.js || true

clean:
	rm -f js2ts parser.tab.c parser.tab.h lex.yy.c output.ts \
	      output1.ts output2.ts output3.ts \
	      trace_input1.txt trace_input2.txt trace_input3.txt
