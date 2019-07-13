all:	jinja_expression

clean:
	rm jinja_expression jinja_expression.tab.c jinja_expression.tab.h lex.yy.c

jinja_expression:	jinja_expression.y jinja_expression.l main.c
	bison -d jinja_expression.y
	flex jinja_expression.l
	gcc jinja_expression.tab.c lex.yy.c main.c -o jinja_expression -lfl

