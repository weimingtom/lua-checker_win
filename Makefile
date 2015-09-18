
APPS=lua_simplifier lua_checker

GENERATED_FILES=lua_lexxer.cpp  lua_parser.cpp  lua_parser.h lua_checker_parser.cpp lua_checker_parser.h

CFLAGS=-Wall -fpermissive -g -Ilua/src -MMD

all: $(APPS)

lua_simplifier: lua_simplifier.o lua_lexxer.o lua_parser.o util.o
	g++ $(CFLAGS) -o $@ $(OBJS) $^ lua/src/liblua.a

lua_checker: lua_checker.o lua_lexxer.o lua_checker_parser.o util.o
	g++ $(CFLAGS) -o $@ $(OBJS) $^ lua/src/liblua.a

%.o: %.cpp
	g++ -c $(CFLAGS) $<

lua_lexxer.cpp: lua.lex
	flex -s -8 --bison-bridge --bison-locations $<
	mv lex.yy.c $@

lua_parser.cpp lua_parser.h: lua.y
	bison --defines=lua_parser.h $< -o lua_parser.cpp

lua_checker_parser.cpp lua_checker_parser.h: lua_checker.y
	bison --defines=lua_checker_parser.h $< -o lua_checker_parser.cpp

clean:
	rm -f *.o *.d $(GENERATED_FILES) $(APPS)

lua_simplifier.o: lua_parser.h
lua_checker.o: lua_checker_parser.h

-include *.d
