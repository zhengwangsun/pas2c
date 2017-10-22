objects=main.o lex.o semantics.o grammar.o   
pas2c:$(objects)
	g++ -v -o pas2c $(objects)

main.o : lex.h grammar.h semantics.h
lex.o : lex.h
grammar.o : grammar.h
semantics.o : common.h grammar.h semantics.h

clean :
	rm pas2c $(objects)
