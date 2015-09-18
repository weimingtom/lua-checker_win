
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse         lua_parser_parse
#define yylex           lua_parser_lex
#define yyerror         lua_parser_error
#define yylval          lua_parser_lval
#define yychar          lua_parser_char
#define yydebug         lua_parser_debug
#define yynerrs         lua_parser_nerrs
#define yylloc          lua_parser_lloc

/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "lua_checker.y"


/*

Copyright (C) 2008 Google Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. 

*/

/*

Parser for a simplified Lua grammar that does various checks.

The following attributes can be added through special comments:
  * const - No assignments after the first declaration can be done.

TODO:
	* track types of variables at each point in the code.
		- warn when TYPE_UNASSIGNED variables go out of scope (useless variables).
		- when TYPE_UNASSIGNED variables assigned to functions and -const_functions
		  given then make those variables const.
		- warn when variables change type
	* allow forward declaration of functions when using -const_functions
	* allow global variable_list=expression_list to define new variables?
	* warning about defining/assigning new global functions in local scope
	* using ellipses in function that doesn't have it as an arg
	* in rules: [LOCAL] IDENTIFIER '=' expression, isn't an assignment
	  to expression_list allowed? allowing this would make the type checking
	  logic more complex, and anyway this represents a programming error
	  and should be properly warned about.
	* in rule: LOCAL identifier_list_2 '=' expression_list, assignments of
	  functions should make the variables constant (if -const_functions given).
	* If -const_functions given, "local function bar() end; bar=2" not warned
	  about since this becomes local bar; bar = function() end; bar=2", so we
	  don't detect that bar needs to be constant initially.
	* warn about any uses of 'dofile' here, as "correct" uses would have already
	  been expanded inline.
	* dofile() returns values returned by chunk (with return?). if the dofile
	  return is expanded inline, that wont work (it will terminate the dofile
	  caller early).
        * Warn about updating a table that is being traversed with for ... in pairs().

DONE:
	* -no-reassignment-of-function-variables (make them implicitly constant)

*/
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <map>
#include <vector>
#include <string>
#include "util.h"
#include "lua_checker.h"
#include "lua_checker_parser.h"

#ifdef _MSC_VER
#pragma warning(disable:4065)
#pragma warning(disable:4996)
#endif

using std::vector;
using std::map;

// Functions called by the parser.
int lua_parser_error(char *s);
int lua_parser_lex(YYSTYPE *yylval_param, YYLTYPE *yylloc_param);

// Type constants. TYPE_UNKNOWN means that the actual type either can't be
// inferred or changes at runtime.
enum {
  TYPE_UNKNOWN,
  TYPE_NIL,
  TYPE_BOOLEAN,
  TYPE_NUMBER,
  TYPE_STRING,
  TYPE_TABLE,
  TYPE_FUNCTION,
  TYPE_WRONG,			// For development: correct type not computed
};

// The type of a variable or expression.
struct TypeInfo {
  int type;			// A TYPE_xxx constant

  TypeInfo(int _type = TYPE_UNKNOWN) {
    type = _type;
  }
};

// Information stored for one variable.
struct VariableInfo {
  int line_number;		// Line this declared on, 0 if declared outside main chunk
  const char *filename;		// File this is declared in
  bool is_constant;		// Can its value be changed?

  VariableInfo(int _line_number = 0, const char *_filename = 0, bool _is_constant = false) {
    line_number = _line_number;
    filename = _filename;
    is_constant = _is_constant;
  }
};

// Information stored per scope.
struct ltstr {
  bool operator()(const char* s1, const char* s2) const
    { return strcmp(s1, s2) < 0; }
};
struct ScopeInfo {
  typedef map<const char*, VariableInfo, ltstr> vars_t;
  vars_t vars;
};

// Array of scopes.
vector<ScopeInfo> scope_vars;

// Forward declarations.
void Initialize();
void Finalize();
void PushScope();
void PopScope();
void AddVariable(const char *filename, int line_number, const char *name, bool is_constant = false);
void CheckVariable(const char *filename, int line_number, const char *name, bool is_assignment = false);
bool IsGlobalVariable(const char *name);



/* Line 189 of yacc.c  */
#line 227 "lua_checker_parser.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     AND = 258,
     BREAK = 259,
     DO = 260,
     ELSE = 261,
     ELSEIF = 262,
     END = 263,
     FALSE = 264,
     FOR = 265,
     FUNCTION = 266,
     IF = 267,
     IN = 268,
     LOCAL = 269,
     NIL = 270,
     NOT = 271,
     OR = 272,
     REPEAT = 273,
     RETURN = 274,
     THEN = 275,
     TRUE = 276,
     UNTIL = 277,
     WHILE = 278,
     CONCAT = 279,
     ELLIPSES = 280,
     EQ = 281,
     GE = 282,
     LE = 283,
     NE = 284,
     SPECIAL = 285,
     SPECIAL_CONST = 286,
     SPECIAL_NUMBER = 287,
     NUMBER = 288,
     STRING = 289,
     IDENTIFIER = 290,
     UNARY_OPERATOR = 291
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 147 "lua_checker.y"

  char *string;			// Also used for identifiers
  double number;
  bool is_constant;		// For opt_const
  struct TypeInfo *type_info;



/* Line 214 of yacc.c  */
#line 308 "lua_checker_parser.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 333 "lua_checker_parser.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   686

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  56
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  37
/* YYNRULES -- Number of rules.  */
#define YYNRULES  95
/* YYNRULES -- Number of states.  */
#define YYNSTATES  200

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   291

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    50,     2,    42,     2,     2,
      51,    52,    40,    38,    47,    39,     2,    41,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    55,    46,
      36,    45,    37,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    53,     2,    54,    44,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    48,     2,    49,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    43
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    13,    18,    21,    22,    23,
      29,    30,    31,    39,    40,    41,    49,    50,    51,    61,
      62,    75,    83,    90,    94,   100,   104,   107,   109,   110,
     116,   117,   118,   122,   124,   126,   128,   130,   132,   134,
     135,   139,   141,   145,   149,   153,   157,   161,   165,   169,
     173,   177,   181,   185,   189,   193,   197,   201,   205,   208,
     211,   214,   216,   220,   222,   224,   229,   234,   241,   246,
     252,   258,   266,   272,   274,   277,   279,   282,   284,   288,
     290,   294,   296,   300,   302,   306,   310,   311,   313,   314,
     316,   317,   319,   320,   322,   323
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      57,     0,    -1,    -1,    58,    88,    -1,    35,    45,    74,
      46,    92,    -1,    84,    45,    85,    46,    -1,    78,    46,
      -1,    -1,    -1,     5,    60,    88,    61,     8,    -1,    -1,
      -1,    23,    74,     5,    62,    88,    63,     8,    -1,    -1,
      -1,    18,    64,    88,    65,    22,    74,    46,    -1,    -1,
      -1,    12,    74,    20,    66,    88,    67,    89,    72,     8,
      -1,    -1,    69,    35,    68,    45,    74,    47,    74,    47,
      74,     5,    88,     8,    -1,    69,    83,    13,    85,     5,
      88,     8,    -1,    14,    35,    45,    74,    46,    92,    -1,
      14,    83,    46,    -1,    14,    87,    45,    85,    46,    -1,
      19,    90,    46,    -1,     4,    46,    -1,    10,    -1,    -1,
       7,    74,    20,    71,    88,    -1,    -1,    -1,     6,    73,
      88,    -1,    15,    -1,     9,    -1,    21,    -1,    33,    -1,
      34,    -1,    25,    -1,    -1,    11,    75,    79,    -1,    76,
      -1,    48,    91,    49,    -1,    74,    24,    74,    -1,    74,
      38,    74,    -1,    74,    39,    74,    -1,    74,    40,    74,
      -1,    74,    41,    74,    -1,    74,    44,    74,    -1,    74,
      42,    74,    -1,    74,    36,    74,    -1,    74,    28,    74,
      -1,    74,    37,    74,    -1,    74,    27,    74,    -1,    74,
      26,    74,    -1,    74,    29,    74,    -1,    74,     3,    74,
      -1,    74,    17,    74,    -1,    16,    74,    -1,    39,    74,
      -1,    50,    74,    -1,    77,    -1,    51,    74,    52,    -1,
      78,    -1,    35,    -1,    76,    53,    74,    54,    -1,    76,
      51,    90,    52,    -1,    76,    55,    35,    51,    90,    52,
      -1,    51,    52,    88,     8,    -1,    51,    25,    52,    88,
       8,    -1,    51,    83,    52,    88,     8,    -1,    51,    83,
      47,    25,    52,    88,     8,    -1,    53,    74,    54,    45,
      74,    -1,    59,    -1,    81,    59,    -1,    70,    -1,    82,
      70,    -1,    35,    -1,    83,    47,    35,    -1,    77,    -1,
      84,    47,    77,    -1,    74,    -1,    85,    47,    74,    -1,
      80,    -1,    86,    47,    80,    -1,    83,    47,    35,    -1,
      -1,    81,    -1,    -1,    82,    -1,    -1,    85,    -1,    -1,
      86,    -1,    -1,    31,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   184,   184,   184,   187,   203,   204,   205,   205,   205,
     206,   206,   206,   207,   207,   207,   208,   208,   208,   209,
     209,   211,   213,   218,   219,   220,   221,   224,   227,   227,
     230,   231,   231,   237,   238,   239,   240,   241,   242,   243,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   269,   270,   271,   278,   279,   285,   286,   290,   291,
     292,   293,   298,   303,   304,   308,   309,   313,   314,   318,
     319,   323,   324,   328,   329,   335,   340,   340,   341,   341,
     342,   342,   343,   343,   344,   344
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "AND", "BREAK", "DO", "ELSE", "ELSEIF",
  "END", "FALSE", "FOR", "FUNCTION", "IF", "IN", "LOCAL", "NIL", "NOT",
  "OR", "REPEAT", "RETURN", "THEN", "TRUE", "UNTIL", "WHILE", "CONCAT",
  "ELLIPSES", "EQ", "GE", "LE", "NE", "SPECIAL", "SPECIAL_CONST",
  "SPECIAL_NUMBER", "NUMBER", "STRING", "IDENTIFIER", "'<'", "'>'", "'+'",
  "'-'", "'*'", "'/'", "'%'", "UNARY_OPERATOR", "'^'", "'='", "';'", "','",
  "'{'", "'}'", "'#'", "'('", "')'", "'['", "']'", "':'", "$accept",
  "file", "$@1", "statement", "$@2", "$@3", "$@4", "$@5", "$@6", "$@7",
  "$@8", "$@9", "$@10", "for", "elseif_block", "$@11", "opt_else_block",
  "$@12", "expression", "$@13", "prefix_expression", "variable",
  "function_call", "function_body", "field", "statement_list",
  "elseif_block_list", "identifier_list", "variable_list",
  "expression_list", "field_list", "identifier_list_2",
  "opt_statement_list", "opt_elseif_block_list", "opt_expression_list",
  "opt_field_list", "opt_const", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,    60,    62,    43,    45,
      42,    47,    37,   291,    94,    61,    59,    44,   123,   125,
      35,    40,    41,    91,    93,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    56,    58,    57,    59,    59,    59,    60,    61,    59,
      62,    63,    59,    64,    65,    59,    66,    67,    59,    68,
      59,    59,    59,    59,    59,    59,    59,    69,    71,    70,
      72,    73,    72,    74,    74,    74,    74,    74,    74,    75,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    76,    76,    76,    77,    77,    78,    78,    79,    79,
      79,    79,    80,    81,    81,    82,    82,    83,    83,    84,
      84,    85,    85,    86,    86,    87,    88,    88,    89,    89,
      90,    90,    91,    91,    92,    92
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     5,     4,     2,     0,     0,     5,
       0,     0,     7,     0,     0,     7,     0,     0,     9,     0,
      12,     7,     6,     3,     5,     3,     2,     1,     0,     5,
       0,     0,     3,     1,     1,     1,     1,     1,     1,     0,
       3,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       2,     1,     3,     1,     1,     4,     4,     6,     4,     5,
       5,     7,     5,     1,     2,     1,     2,     1,     3,     1,
       3,     1,     3,     1,     3,     3,     0,     1,     0,     1,
       0,     1,     0,     1,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,    86,     1,     0,     7,    27,     0,     0,    13,
      90,     0,    64,     0,    73,     0,     0,    61,    63,    87,
       0,     3,    26,    86,    34,    39,    33,     0,    35,    38,
      36,    37,    64,     0,    92,     0,     0,    41,    61,    63,
      77,     0,     0,    86,    81,    91,     0,     0,     0,     0,
      77,     0,    90,     0,     0,     6,    74,     0,     0,     8,
       0,    58,    59,     0,    83,    93,     0,    60,     0,     0,
      16,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,     0,    14,     0,
      25,    10,     0,    62,     0,     0,     0,     0,     0,     0,
       0,    61,     0,     0,    40,     0,     0,    42,    56,    57,
      86,    43,    54,    53,    51,    55,    50,    52,    44,    45,
      46,    47,    49,    48,     0,    78,     0,     0,    82,    86,
      94,     0,     0,    78,    66,    65,    90,     5,     9,     0,
      77,    86,     0,     0,    84,    17,    94,    24,     0,    11,
      95,     4,     0,    86,     0,    86,     0,     0,    86,     0,
      88,    22,     0,     0,     0,     0,    67,     0,    68,     0,
       0,    72,     0,    75,    89,    30,    15,    12,     0,    21,
      69,    86,    70,     0,    76,    31,     0,     0,     0,    28,
      86,    18,     0,    71,    86,    32,    86,    29,     0,    20
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    14,    23,   102,   129,   163,    43,   127,
     110,   160,    94,    15,   173,   194,   186,   190,    44,    60,
      37,    38,    39,   104,    64,    19,   174,    41,    20,    45,
      65,    42,    21,   175,    46,    66,   151
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -52
static const yytype_int16 yypact[] =
{
     -52,    13,    87,   -52,   -26,   -52,   -52,   463,   -11,   -52,
     463,   463,   -10,   463,   -52,    -3,   -39,     1,    -7,    87,
       7,   -52,   -52,    87,   -52,   -52,   -52,   463,   -52,   -52,
     -52,   -52,   -52,   463,   -20,   463,   492,   -39,   -52,   -52,
      14,    22,    19,    87,   604,    -4,    20,   520,   463,   277,
      32,    -9,   463,   463,    10,   -52,   -52,   463,   -28,   -52,
     -17,    40,    40,   463,   -52,    39,    51,    40,   463,   463,
     -52,   463,   463,   463,   463,   463,   463,   463,   463,   463,
     463,   463,   463,   463,   463,   -52,    54,   463,   -52,   463,
     -52,   -52,   373,   -52,    57,   463,    68,    52,    34,    56,
      47,     8,   103,    15,   -52,   246,   -20,   -52,   623,   182,
      87,    41,    41,   642,   642,    41,   642,   642,   -14,   -14,
      40,    40,    40,    40,   401,    67,    49,    91,   604,    87,
      83,   463,     0,   -52,   -52,   -52,   463,   -52,   -52,    63,
     -52,    87,   -37,    73,   -52,   -52,    83,   -52,   463,   -52,
     -52,   -52,   317,    87,    69,    87,   111,   -16,    87,   463,
     113,   -52,   429,   115,   463,   116,   -52,   117,   -52,    74,
     122,   604,   463,   -52,   113,   125,   -52,   -52,   345,   -52,
     -52,    87,   -52,   548,   -52,   -52,   124,   463,   126,   -52,
      87,   -52,   576,   -52,    87,   -52,    87,   -52,   128,   -52
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -52,   -52,   -52,   114,   -52,   -52,   -52,   -52,   -52,   -52,
     -52,   -52,   -52,   -52,   -32,   -52,   -52,   -52,   164,   -52,
      -2,    -1,     6,   -52,    31,   -52,   -52,   -13,   -52,   -51,
     -52,   -52,   -12,   -52,   -49,   -52,     2
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -86
static const yytype_int16 yytable[] =
{
      16,    17,    51,    97,    95,   153,   100,    32,    18,   169,
     157,    59,    52,     3,    53,   158,    54,    16,    17,   133,
      22,    16,    17,    13,    40,    18,    80,    81,    82,    18,
      83,    88,    50,    63,   103,    48,   126,    68,    96,    55,
     139,    16,    17,    89,   132,    99,   -79,    89,   -79,    18,
     140,    69,    57,   -80,    58,   -80,    16,   101,    71,    84,
      72,    73,    74,    75,    87,    71,    90,   141,    85,    86,
      76,    77,    78,    79,    80,    81,    82,   -19,    83,    78,
      79,    80,    81,    82,    83,    83,   106,   154,   135,   125,
     142,     4,     5,   137,    89,   147,    89,     6,   145,     7,
     107,     8,   131,   133,   134,     9,    10,   136,    16,    17,
      11,   138,   -85,   148,   150,   155,    18,   149,   159,   168,
     172,   166,    12,   177,   179,   180,   181,    16,    17,   156,
     182,   185,   191,    56,   193,    18,   199,   144,    13,    16,
      17,   165,   184,   167,     0,     0,   170,    18,   161,     0,
       0,    16,    17,    16,    17,     0,    16,    17,     0,    18,
       0,    18,     0,     0,    18,     0,     0,     0,     0,   188,
       0,    36,     0,     0,     0,    47,     0,    49,   195,    16,
      17,     0,   197,     0,   198,    68,     0,    18,    16,    17,
       0,    61,    16,    17,    16,    17,    18,    62,     0,    67,
      18,     0,    18,     0,     0,     0,    71,     0,    72,    73,
      74,    75,    92,     0,     0,     0,     0,    98,    76,    77,
      78,    79,    80,    81,    82,     0,    83,   105,     0,     0,
       0,     0,   108,   109,     0,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,    68,
       0,     0,     0,   128,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    69,     0,     0,     0,     0,     0,     0,
      71,     0,    72,    73,    74,    75,     0,     0,     0,     0,
      68,     0,    76,    77,    78,    79,    80,    81,    82,     0,
      83,     0,     0,     0,    69,   152,     0,     0,     0,     0,
     143,    71,     0,    72,    73,    74,    75,     0,     0,     0,
       0,     0,   162,    76,    77,    78,    79,    80,    81,    82,
      68,    83,     0,   171,     0,     0,     0,     0,   178,    93,
       0,     0,     0,     0,    69,     0,   183,     0,     0,     0,
       0,    71,     0,    72,    73,    74,    75,     0,    68,     0,
       0,   192,     0,    76,    77,    78,    79,    80,    81,    82,
       0,    83,    69,     0,   164,     0,     0,     0,     0,    71,
       0,    72,    73,    74,    75,     0,    68,     0,     0,     0,
       0,    76,    77,    78,    79,    80,    81,    82,     0,    83,
      69,     0,   187,     0,     0,     0,     0,    71,     0,    72,
      73,    74,    75,     0,    68,     0,     0,     0,     0,    76,
      77,    78,    79,    80,    81,    82,     0,    83,    69,   130,
       0,     0,     0,     0,     0,    71,     0,    72,    73,    74,
      75,     0,    68,     0,     0,     0,     0,    76,    77,    78,
      79,    80,    81,    82,     0,    83,    69,   146,     0,     0,
       0,     0,     0,    71,     0,    72,    73,    74,    75,     0,
       0,     0,     0,     0,     0,    76,    77,    78,    79,    80,
      81,    82,    24,    83,    25,   176,     0,     0,    26,    27,
       0,     0,     0,     0,    28,     0,     0,     0,    29,     0,
       0,     0,     0,     0,     0,    68,    30,    31,    32,     0,
       0,     0,    33,     0,     0,     0,     0,     0,     0,    69,
       0,    34,    70,    35,    13,     0,    71,     0,    72,    73,
      74,    75,     0,    68,     0,    91,     0,     0,    76,    77,
      78,    79,    80,    81,    82,     0,    83,    69,     0,     0,
       0,     0,     0,     0,    71,     0,    72,    73,    74,    75,
       0,    68,     0,     0,     0,     0,    76,    77,    78,    79,
      80,    81,    82,     0,    83,    69,     0,     0,   189,     0,
       0,     0,    71,     0,    72,    73,    74,    75,     0,    68,
       0,   196,     0,     0,    76,    77,    78,    79,    80,    81,
      82,     0,    83,    69,     0,     0,     0,     0,     0,     0,
      71,     0,    72,    73,    74,    75,     0,    68,     0,     0,
       0,     0,    76,    77,    78,    79,    80,    81,    82,     0,
      83,    69,     0,     0,     0,     0,     0,     0,    71,     0,
      72,    73,    74,    75,     0,     0,     0,     0,     0,     0,
      76,    77,    78,    79,    80,    81,    82,    71,    83,    72,
      73,    74,    75,     0,     0,     0,     0,     0,     0,    76,
      77,    78,    79,    80,    81,    82,    71,    83,    72,     0,
       0,    75,     0,     0,     0,     0,     0,     0,     0,     0,
      78,    79,    80,    81,    82,     0,    83
};

static const yytype_int16 yycheck[] =
{
       2,     2,    15,    52,    13,     5,    57,    35,     2,    25,
      47,    23,    51,     0,    53,    52,    55,    19,    19,    35,
      46,    23,    23,    51,    35,    19,    40,    41,    42,    23,
      44,    43,    35,    53,    51,    45,    87,     3,    47,    46,
      25,    43,    43,    47,    95,    35,    45,    47,    47,    43,
      35,    17,    45,    45,    47,    47,    58,    58,    24,    45,
      26,    27,    28,    29,    45,    24,    46,    52,    46,    47,
      36,    37,    38,    39,    40,    41,    42,    45,    44,    38,
      39,    40,    41,    42,    44,    44,    47,   136,    54,    35,
     103,     4,     5,    46,    47,    46,    47,    10,   110,    12,
      49,    14,    45,    35,    52,    18,    19,    51,   110,   110,
      23,     8,    45,    22,    31,    52,   110,   129,    45,     8,
       7,    52,    35,     8,     8,     8,    52,   129,   129,   141,
       8,     6,     8,    19,     8,   129,     8,   106,    51,   141,
     141,   153,   174,   155,    -1,    -1,   158,   141,   146,    -1,
      -1,   153,   153,   155,   155,    -1,   158,   158,    -1,   153,
      -1,   155,    -1,    -1,   158,    -1,    -1,    -1,    -1,   181,
      -1,     7,    -1,    -1,    -1,    11,    -1,    13,   190,   181,
     181,    -1,   194,    -1,   196,     3,    -1,   181,   190,   190,
      -1,    27,   194,   194,   196,   196,   190,    33,    -1,    35,
     194,    -1,   196,    -1,    -1,    -1,    24,    -1,    26,    27,
      28,    29,    48,    -1,    -1,    -1,    -1,    53,    36,    37,
      38,    39,    40,    41,    42,    -1,    44,    63,    -1,    -1,
      -1,    -1,    68,    69,    -1,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,     3,
      -1,    -1,    -1,    89,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      24,    -1,    26,    27,    28,    29,    -1,    -1,    -1,    -1,
       3,    -1,    36,    37,    38,    39,    40,    41,    42,    -1,
      44,    -1,    -1,    -1,    17,   131,    -1,    -1,    -1,    -1,
      54,    24,    -1,    26,    27,    28,    29,    -1,    -1,    -1,
      -1,    -1,   148,    36,    37,    38,    39,    40,    41,    42,
       3,    44,    -1,   159,    -1,    -1,    -1,    -1,   164,    52,
      -1,    -1,    -1,    -1,    17,    -1,   172,    -1,    -1,    -1,
      -1,    24,    -1,    26,    27,    28,    29,    -1,     3,    -1,
      -1,   187,    -1,    36,    37,    38,    39,    40,    41,    42,
      -1,    44,    17,    -1,    47,    -1,    -1,    -1,    -1,    24,
      -1,    26,    27,    28,    29,    -1,     3,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    40,    41,    42,    -1,    44,
      17,    -1,    47,    -1,    -1,    -1,    -1,    24,    -1,    26,
      27,    28,    29,    -1,     3,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    40,    41,    42,    -1,    44,    17,    46,
      -1,    -1,    -1,    -1,    -1,    24,    -1,    26,    27,    28,
      29,    -1,     3,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    40,    41,    42,    -1,    44,    17,    46,    -1,    -1,
      -1,    -1,    -1,    24,    -1,    26,    27,    28,    29,    -1,
      -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    40,
      41,    42,     9,    44,    11,    46,    -1,    -1,    15,    16,
      -1,    -1,    -1,    -1,    21,    -1,    -1,    -1,    25,    -1,
      -1,    -1,    -1,    -1,    -1,     3,    33,    34,    35,    -1,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    17,
      -1,    48,    20,    50,    51,    -1,    24,    -1,    26,    27,
      28,    29,    -1,     3,    -1,     5,    -1,    -1,    36,    37,
      38,    39,    40,    41,    42,    -1,    44,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    24,    -1,    26,    27,    28,    29,
      -1,     3,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      40,    41,    42,    -1,    44,    17,    -1,    -1,    20,    -1,
      -1,    -1,    24,    -1,    26,    27,    28,    29,    -1,     3,
      -1,     5,    -1,    -1,    36,    37,    38,    39,    40,    41,
      42,    -1,    44,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      24,    -1,    26,    27,    28,    29,    -1,     3,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    40,    41,    42,    -1,
      44,    17,    -1,    -1,    -1,    -1,    -1,    -1,    24,    -1,
      26,    27,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    40,    41,    42,    24,    44,    26,
      27,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    40,    41,    42,    24,    44,    26,    -1,
      -1,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    39,    40,    41,    42,    -1,    44
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    57,    58,     0,     4,     5,    10,    12,    14,    18,
      19,    23,    35,    51,    59,    69,    76,    77,    78,    81,
      84,    88,    46,    60,     9,    11,    15,    16,    21,    25,
      33,    34,    35,    39,    48,    50,    74,    76,    77,    78,
      35,    83,    87,    64,    74,    85,    90,    74,    45,    74,
      35,    83,    51,    53,    55,    46,    59,    45,    47,    88,
      75,    74,    74,    53,    80,    86,    91,    74,     3,    17,
      20,    24,    26,    27,    28,    29,    36,    37,    38,    39,
      40,    41,    42,    44,    45,    46,    47,    45,    88,    47,
      46,     5,    74,    52,    68,    13,    47,    90,    74,    35,
      85,    77,    61,    51,    79,    74,    47,    49,    74,    74,
      66,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    35,    85,    65,    74,    62,
      46,    45,    85,    35,    52,    54,    51,    46,     8,    25,
      35,    52,    83,    54,    80,    88,    46,    46,    22,    88,
      31,    92,    74,     5,    90,    52,    88,    47,    52,    45,
      67,    92,    74,    63,    47,    88,    52,    88,     8,    25,
      88,    74,     7,    70,    82,    89,    46,     8,    74,     8,
       8,    52,     8,    74,    70,     6,    72,    47,    88,    20,
      73,     8,    74,     8,    71,    88,     5,    88,    88,     8
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 184 "lua_checker.y"
    { Initialize(); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 184 "lua_checker.y"
    { Finalize(); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 188 "lua_checker.y"
    {
	  // If a single identifier is assigned for the first time in the outer
	  // scope then this is considered to be a valid variable declaration.
	  // This rule creates an expected shift/reduce conflict.
	  // @@@ If a variable is assigned multiple times, check that its type is the same.
	  if (scope_vars.size() == 1 && !IsGlobalVariable((yyvsp[(1) - (5)].string))) {
	    bool is_const = (yyvsp[(5) - (5)].is_constant) || (flag_const_functions && (yyvsp[(3) - (5)].type_info)->type == TYPE_FUNCTION);
	    AddVariable((yylsp[(1) - (5)]).filename, (yylsp[(1) - (5)]).line, (yyvsp[(1) - (5)].string), is_const);
	  } else {
	    CheckVariable((yylsp[(1) - (5)]).filename, (yylsp[(1) - (5)]).line, (yyvsp[(1) - (5)].string), true);
	    if ((yyvsp[(5) - (5)].is_constant)) {
	      printf("%s:%d: Only declaration of variable '%s' may use 'const'\n", (yylsp[(1) - (5)]).filename, (yylsp[(1) - (5)]).line, (yyvsp[(1) - (5)].string));
	    }
	  }
	;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 205 "lua_checker.y"
    { PushScope(); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 205 "lua_checker.y"
    { PopScope(); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 206 "lua_checker.y"
    { PushScope(); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 206 "lua_checker.y"
    { PopScope(); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 207 "lua_checker.y"
    { PushScope(); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 207 "lua_checker.y"
    { PopScope(); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 208 "lua_checker.y"
    { PushScope(); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 208 "lua_checker.y"
    { PopScope(); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 209 "lua_checker.y"
    { AddVariable((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, (yyvsp[(2) - (2)].string)); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 210 "lua_checker.y"
    { PopScope(); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 212 "lua_checker.y"
    { PopScope(); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 214 "lua_checker.y"
    {
	  bool is_const = (yyvsp[(6) - (6)].is_constant) || (flag_const_functions && (yyvsp[(4) - (6)].type_info)->type == TYPE_FUNCTION);
	  AddVariable((yylsp[(1) - (6)]).filename, (yylsp[(1) - (6)]).line, (yyvsp[(2) - (6)].string), is_const);
	;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 224 "lua_checker.y"
    { PushScope(); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 227 "lua_checker.y"
    { PushScope(); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 227 "lua_checker.y"
    { PopScope(); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 231 "lua_checker.y"
    { PushScope(); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 231 "lua_checker.y"
    { PopScope(); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 237 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_NIL); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 238 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_BOOLEAN); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 239 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_BOOLEAN); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 240 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_NUMBER); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 241 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_STRING); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 242 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_UNKNOWN); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 243 "lua_checker.y"
    { PushScope(); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 243 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_FUNCTION); PopScope(); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 245 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_TABLE); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 246 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_STRING); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 247 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_NUMBER); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 248 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_NUMBER); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 249 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_NUMBER); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 250 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_NUMBER); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 251 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_NUMBER); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 252 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_NUMBER); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 253 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_BOOLEAN); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 254 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_BOOLEAN); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 255 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_BOOLEAN); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 256 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_BOOLEAN); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 257 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_BOOLEAN); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 258 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_BOOLEAN); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 259 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_WRONG); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 260 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_WRONG); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 261 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_WRONG); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 262 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_NUMBER); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 263 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_NUMBER); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 270 "lua_checker.y"
    { (yyval.type_info) = (yyvsp[(2) - (3)].type_info); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 278 "lua_checker.y"
    { CheckVariable((yylsp[(1) - (1)]).filename, (yylsp[(1) - (1)]).line, (yyvsp[(1) - (1)].string)); (yyval.type_info) = new TypeInfo(TYPE_WRONG); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 279 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_WRONG); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 285 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_WRONG); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 286 "lua_checker.y"
    { (yyval.type_info) = new TypeInfo(TYPE_WRONG); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 313 "lua_checker.y"
    { AddVariable((yylsp[(1) - (1)]).filename, (yylsp[(1) - (1)]).line, (yyvsp[(1) - (1)].string)); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 314 "lua_checker.y"
    { AddVariable((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, (yyvsp[(3) - (3)].string)); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 335 "lua_checker.y"
    { AddVariable((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, (yyvsp[(3) - (3)].string)); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 344 "lua_checker.y"
    { (yyval.is_constant) = false; ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 344 "lua_checker.y"
    { (yyval.is_constant) = true; ;}
    break;



/* Line 1455 of yacc.c  */
#line 2251 "lua_checker_parser.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 346 "lua_checker.y"


int lua_parser_error (char *s) {
  Panic("%s, at %s:%d of simplified lua source", s, LexxerFilename(), LexxerLinecount());
#ifdef _MSC_VER
  return 0;
#endif
}

void Initialize() {
  scope_vars.resize(1);
  // Add global functions and variables.
  scope_vars.back().vars["error"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["print"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["pairs"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["ipairs"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["assert"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["type"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["loadstring"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["tonumber"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["tostring"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["rawget"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["next"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["dofile"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["setmetatable"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["_G"] = VariableInfo(0, 0, true);
  // Add library function tables.
  scope_vars.back().vars["string"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["io"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["os"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["table"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["math"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["coroutine"] = VariableInfo(0, 0, true);
  // Add mylua/lmake extensions. @@@ need a different way to specify these.
  scope_vars.back().vars["arg"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["FLAGS"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["ErrorHandler"] = VariableInfo(0);	// Can be assigned to
  scope_vars.back().vars["Rule"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["Cmd"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["CmdNoError"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["CmdGrabOutput"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["FileExists"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["StringToFile"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["FileAsString"] = VariableInfo(0, 0, true);
  scope_vars.back().vars["ExpandVariables"] = VariableInfo(0, 0, true);
}

void Finalize() {
  CHECK(scope_vars.size() == 1);
}

void PushScope() {
  scope_vars.resize(scope_vars.size() + 1);
}

void PopScope() {
  scope_vars.pop_back();
}

// Add a variable to the current scope.

void AddVariable(const char *filename, int line_number, const char *name, bool is_constant) {
  CHECK(line_number > 0);
  CHECK(scope_vars.size() > 0);

  // Check for this variable in the current scope, and optionally outer
  // scopes too.
  for (int i = scope_vars.size()-1 ; i >= 0; i--) {
    if (scope_vars[i].vars.count(name) > 0) {
      const char *f = scope_vars[i].vars[name].filename;
      int l = scope_vars[i].vars[name].line_number;
      if (l > 0) {
        printf("%s:%d: variable '%s' already declared at %s:%d\n",
               filename, line_number, name, f, l);
      } else {
        printf("%s:%d: variable '%s' is declared external to this file\n",
               filename, line_number, name);
      }
      break;
    }
    if (!flag_no_reuse_varnames) break;
  }
  scope_vars.back().vars[name] = VariableInfo(line_number, filename, is_constant);
}

// Check that a variable exists in the current or previous scopes. Print a
// warning if this variable is being assigned to and has a constant value.

void CheckVariable(const char *filename, int line_number, const char *name, bool is_assignment) {
  for (int i = scope_vars.size()-1 ; i >= 0; i--) {
    ScopeInfo::vars_t::iterator it = scope_vars[i].vars.find(name);
    if (it != scope_vars[i].vars.end()) {
      if (is_assignment && it->second.is_constant) {
        if (it->second.line_number > 0) {
          printf("%s:%d: Assignment to constant '%s' (which was declared at %s:%d)\n",
                 filename, line_number, name, it->second.filename, it->second.line_number);
        } else {
          printf("%s:%d: Assignment to constant '%s' (which was declared external to this file)\n",
                 filename, line_number, name);
        }
      }
      return;
    }
  }
  printf("%s:%d: Variable '%s' not declared\n", filename, line_number, name);
}

// Return true if the given name is a global variable.

bool IsGlobalVariable(const char *name) {
  CHECK(scope_vars.size() > 0);
  return scope_vars[0].vars.find(name) != scope_vars[0].vars.end();
}

