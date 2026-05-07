all: js2ts

js2ts: parser.tab.c lex.yy.c ast.c
	gcc -o js2ts parser.tab.c lex.yy.c ast.c -w

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

run: js2ts
	./js2ts < input.js  && mv output.ts output1.ts
	./js2ts < input2.js && mv output.ts output2.ts
	./js2ts < input3.js && mv output.ts output3.ts
	@echo ""
	@echo "── Error input test ────────────────────────────────────"
	./js2ts < input_error.js || true

clean:
	rm -f js2ts parser.tab.c parser.tab.h lex.yy.c output.ts output1.ts output2.ts output3.ts
