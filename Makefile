all:	jinja_expression

.PHONY: test

test:	jinja_expression
	cd test; ./autotest.rb

clean:
	rm jinja_expression jinja_expression.tab.c jinja_expression.tab.h lex.yy.c

jinja_expression:	jinja_expression.y jinja_expression.l main.c parameter.c parameter.h ast.h common.h ast.c
	bison -d jinja_expression.y
	flex jinja_expression.l
	gcc -std=gnu99 -D_GNU_SOURCE -W -Wall -Wno-aggregate-return -Wno-suggest-attribute=format -Wno-undef -fms-extensions -g -fstack-protector-all -Wstack-protector -fno-omit-frame-pointer \
	jinja_expression.tab.c lex.yy.c main.c parameter.c ast.c -o jinja_expression -lfl

