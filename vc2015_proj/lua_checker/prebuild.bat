@echo ��ע�⣺Ŀ¼�����԰����ո񣬷���flex����

@REM 1 [main] flex 5332 fork: child -1 - CreateProcessW failed, errno 2 flex: fork failed

cd ..\..\

@set PATH=%PATH%;%CD%\bin

flex.exe -8 --bison-bridge --bison-locations -olua_lexxer.cpp lua_lexxer.lex
bison.exe --defines=lua_parser.h -o lua_parser.cpp lua_parser.y
bison.exe --defines=lua_checker_parser.h -o lua_checker_parser.cpp lua_checker.y

@pause



