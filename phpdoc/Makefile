ifdef DEBUG
CPPFLAGS = $(INCLUDES) -fPIC -ggdb -Wall -DDEBUG=1 -DYYDEBUG=1 -Dparse.trace
BISONFLAGS = --debug -r all --graph
FLEXFLAGS = -d --trace
else
CPPFLAGS = $(INCLUDES) -Wall -fPIC -g -O3 -minline-all-stringops
BISONFLAGS = 
FLEXFLAGS = 
endif

ifdef PROFILE
CPPFLAGS += -pg
endif

all: libphpdoc.a libphpdoc.so

clean:
	-rm libphpdoc.a phpdoct scanner.lex.cpp scanner.lex.hpp parser.yacc.* *.o 2>/dev/null

parser.yacc.cpp: parser.y
	bison $(BISONFLAGS) -d -o $@ $<

parser.yacc.hpp: parser.yacc.cpp

scanner.lex.cpp: scanner.l
	`which flex35 2>/dev/null || which flex 2>/dev/null` \
	  -C --header-file=scanner.lex.hpp -o $@ $(FLEXFLAGS) $<

scanner.lex.hpp: scanner.lex.cpp

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

phpdoc.o: phpdoc.cc scanner.lex.hpp parser.yacc.hpp
parser.yacc.o: scanner.lex.hpp
scanner.lex.o: parser.yacc.hpp

libphpdoc.a: scanner.lex.o parser.yacc.o phpdoc.o
	$(AR) -crs $@ $^

libphpdoc.so: scanner.lex.o parser.yacc.o phpdoc.o
	g++ -shared -Wl,-soname,libphpdoc.so -o libphpdoc.so $^

phpdoct: phpdoct.cpp libphpdoc.a
	g++ $(CPPFLAGS) -o $@ $^

.PHONY: all clean tags
