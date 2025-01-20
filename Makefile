HEADERS = decode.h encode.h dbg.h prune.h
OBJECTS = program.o decode.o encode.o dbg.o prune.o

default: program

program.o: main.c $(HEADERS)
	gcc -c -g main.c -o program.o

decode.o: decode.c decode.h
	gcc -c -g decode.c -o decode.o

encode.o: encode.c encode.h
	gcc -c -g encode.c -o encode.o

program: $(OBJECTS)
	touch encode
	touch decode
	touch dbg
	touch prune
	rm -f decode
	rm -f encode
	rm -f dbg
	rm -f prune
	
	gcc $(OBJECTS) -o program

	ln -s program decode
	ln -s program encode
	ln -s program dbg
	ln -s program prune

clean:
	-rm -f $(OBJECTS)
	-rm -f program
	-rm -f decode
	-rm -f encode
	-rm -f dbg
	-rm -f DBG.encode
	-rm -f DBG.decode
	-rm -f prune

