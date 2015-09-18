rmdir /S /Q Debug Release .vs
del /Q *.ncb *.opt *.suo *.plg *.sdf

cd lua_checker
rmdir /S /Q Debug Release .vs
del /Q *.ncb *.opt *.suo *.plg *.sdf
cd ..

cd lua_simplifier
rmdir /S /Q Debug Release .vs
del /Q *.ncb *.opt *.suo *.plg *.sdf
cd ..
