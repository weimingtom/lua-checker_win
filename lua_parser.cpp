
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
#line 1 "lua_parser.y"


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

Lua parser.

The Lua grammar has a well known ambiguity in the parsing of e.g. f(a)(b)(c),
in that this can be interpreted as either f(a);(b)(c) or (f(a)(b))(c).
The second parse is always preferred, which means that a function call
statement (that doesn't end in a semicolon) cannot be followed by a statement
that starts with an "(". This is expressed in this grammar by dividing some
rules into br_ and nobr_ versions, i.e. those that can start with a bracket '('
and those that can't. The 'statement list' rule is divided into parts to express
the disallowed transitions.

To avoid visual ambiguity in Lua code you cannot put a line break before
the '(' in a function call. This is not enforced here, since any code that is
compiled without error by Lua will be unambiguous here.

Two shift/reduce conflicts are expected. The first parses prefix_expression(...
as a function call rather than an expression followed by the start of a new
statement. The second calls out function call statements that looks like
'IDENTIFIER STRING', so that dofile files can be expanded inline.

The rule actions here output "normalized" Lua source code that is somewhat
easier for a following stage to parse: All output statements end with
semicolons, eliminating the above ambiguity, and various kinds of Lua
syntactic sugar are expanded out:

  a.name                          --> a['name']
  fn{fields}                      --> fn({fields})
  fn'string'                      --> fn('string')
  function fn() body              --> fn = function() body
  function t.a.b.c.f() body       --> t.a.b.c.f = function() body
  local function fn() body        --> local fn; fn = function() body
  function t.a.b.c:f(params) body --> t.a.b.c.f = function(self, params) body
  { foo=1 }                       --> { 'foo'=1 }

The following syntactic sugar expansion from the Lua manual is *not* done,
since 'v' must be evaluated only once:

  v:name(args) --> v.name(v,args)

*/

#include <string.h>
#include <stdarg.h>
#include "util.h"
#include "lua_simplifier.h"

#ifdef _MSC_VER
#pragma warning(disable:4065)
#pragma warning(disable:4996)

/*
用临时文件处理
totem-plparser
https://github.com/zsx/totem-plparser
*/

/*
用malloc分配内存，在程序结束后自动回收
see 
http://pdfrecompressor.googlecode.com/svn-history/r40/trunk/jbig2enc_modified/jbig2.cc
*/
// -----------------------------------------------------------------------------
// Windows, sadly, lacks asprintf
// -----------------------------------------------------------------------------
#include <stdarg.h>
static int asprintf(char **strp, const char *fmt, ...) 
{
    va_list va;
    va_start(va, fmt);
#if defined(_MSC_VER) && _MSC_VER <= 1200
    const int required = 65535;
    char *const buffer = (char *) malloc(required + 1);
    const int ret = _vsnprintf(buffer, required + 1, fmt, va);
#else
    const int required = vsnprintf(NULL, 0, fmt, va);
    char *const buffer = (char *) malloc(required + 1);
    const int ret = vsnprintf(buffer, required + 1, fmt, va);
#endif
    *strp = buffer;
    va_end(va);
    return ret;
}

static int vasprintf(char **strp, const char *fmt, va_list va)
{
#if defined(_MSC_VER) && _MSC_VER <= 1200
    const int required = 65535;
    char *const buffer = (char *) malloc(required + 1);
    const int ret = _vsnprintf(buffer, required + 1, fmt, va);
#else
    const int required = vsnprintf(NULL, 0, fmt, va);
    char *const buffer = (char *) malloc(required + 1);
    const int ret = vsnprintf(buffer, required + 1, fmt, va);
#endif
    *strp = buffer;
	return ret;
}

#endif

// Semantic values.
#define YYSTYPE char*

// Error function called by the parser.
int lua_parser_error(char *s);

// Take printf-style arguments and return a new string on the heap.
// The special word '@MarkerAndIndent@' is replaced by a filename:line_number marker and an
// indent appropriate for the current scope.
char *String(const char *filename, int line_number, const char *msg, ...)
  __attribute__((format (printf, 3, 4)));
#define INDENT "@MarkerAndIndent@"

// Concatenate two strings and return the result. If trim_trailing_newline is
// true then the trailing \n of s1 is removed and appended to the final string,
// unless both strings have zero length.
char *Concat(char *s1, const char *s2, bool trim_trailing_newline = false);



/* Line 189 of yacc.c  */
#line 230 "lua_parser.cpp"

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
typedef int YYSTYPE;
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
#line 321 "lua_parser.cpp"

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
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   735

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  57
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  43
/* YYNRULES -- Number of rules.  */
#define YYNRULES  137
/* YYNRULES -- Number of states.  */
#define YYNSTATES  245

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
       2,     2,     2,     2,     2,    52,     2,    42,     2,     2,
      50,    51,    40,    38,    47,    39,    49,    41,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    48,    45,
      36,    46,    37,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    53,     2,    54,    44,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    55,     2,    56,     2,     2,     2,     2,
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
       0,     0,     3,     5,     6,     9,    10,    12,    14,    17,
      19,    21,    24,    28,    31,    33,    36,    39,    41,    43,
      45,    47,    49,    52,    55,    58,    61,    63,    66,    69,
      71,    74,    77,    80,    83,    85,    88,    91,    95,    99,
     105,   110,   118,   128,   140,   148,   152,   158,   163,   166,
     171,   174,   178,   179,   181,   183,   186,   191,   192,   195,
     198,   202,   205,   207,   211,   215,   217,   221,   225,   227,
     231,   233,   235,   237,   239,   241,   243,   246,   248,   252,
     254,   258,   262,   266,   270,   274,   278,   282,   286,   290,
     294,   298,   302,   306,   310,   314,   317,   320,   323,   325,
     329,   331,   333,   335,   340,   344,   351,   357,   360,   365,
     370,   377,   380,   384,   386,   388,   394,   395,   397,   399,
     403,   406,   407,   413,   415,   419,   425,   429,   431,   432,
     434,   436,   438,   439,   441,   443,   447,   448
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      58,     0,    -1,    59,    -1,    -1,    60,    61,    -1,    -1,
      77,    -1,    66,    -1,    66,    77,    -1,    86,    -1,    87,
      -1,    71,    99,    -1,    71,    45,    99,    -1,    86,    45,
      -1,    72,    -1,    72,    45,    -1,    87,    45,    -1,    67,
      -1,    68,    -1,    69,    -1,    70,    -1,    62,    -1,    67,
      62,    -1,    68,    62,    -1,    69,    62,    -1,    70,    62,
      -1,    63,    -1,    69,    63,    -1,    70,    63,    -1,    64,
      -1,    67,    64,    -1,    68,    64,    -1,    69,    64,    -1,
      70,    64,    -1,    65,    -1,    69,    65,    -1,    70,    65,
      -1,    78,    46,    82,    -1,     5,    59,     8,    -1,    23,
      81,     5,    59,     8,    -1,    18,    59,    22,    81,    -1,
      12,    81,    20,    59,    73,    76,     8,    -1,    10,    35,
      46,    81,    47,    81,     5,    59,     8,    -1,    10,    35,
      46,    81,    47,    81,    47,    81,     5,    59,     8,    -1,
      10,    98,    13,    82,     5,    59,     8,    -1,    11,    80,
      89,    -1,    11,    80,    48,    35,    89,    -1,    14,    11,
      35,    89,    -1,    14,    98,    -1,    14,    98,    46,    82,
      -1,    35,    34,    -1,    79,    46,    82,    -1,    -1,    74,
      -1,    75,    -1,    74,    75,    -1,     7,    81,    20,    59,
      -1,    -1,     6,    59,    -1,    19,    95,    -1,    19,    82,
      95,    -1,     4,    95,    -1,    84,    -1,    78,    47,    84,
      -1,    78,    47,    85,    -1,    85,    -1,    79,    47,    84,
      -1,    79,    47,    85,    -1,    35,    -1,    80,    49,    35,
      -1,    15,    -1,     9,    -1,    21,    -1,    33,    -1,    34,
      -1,    25,    -1,    11,    89,    -1,    83,    -1,    50,    81,
      51,    -1,    91,    -1,    81,    24,    81,    -1,    81,    38,
      81,    -1,    81,    39,    81,    -1,    81,    40,    81,    -1,
      81,    41,    81,    -1,    81,    44,    81,    -1,    81,    42,
      81,    -1,    81,    36,    81,    -1,    81,    28,    81,    -1,
      81,    37,    81,    -1,    81,    27,    81,    -1,    81,    26,
      81,    -1,    81,    29,    81,    -1,    81,     3,    81,    -1,
      81,    17,    81,    -1,    16,    81,    -1,    39,    81,    -1,
      52,    81,    -1,    81,    -1,    82,    47,    81,    -1,    84,
      -1,    86,    -1,    35,    -1,    83,    53,    81,    54,    -1,
      83,    49,    35,    -1,    50,    81,    51,    53,    81,    54,
      -1,    50,    81,    51,    49,    35,    -1,    83,    88,    -1,
      83,    48,    35,    88,    -1,    50,    81,    51,    88,    -1,
      50,    81,    51,    48,    35,    88,    -1,    50,    51,    -1,
      50,    82,    51,    -1,    91,    -1,    34,    -1,    50,    90,
      51,    59,     8,    -1,    -1,    25,    -1,    98,    -1,    98,
      47,    25,    -1,    55,    56,    -1,    -1,    55,    92,    93,
      97,    56,    -1,    94,    -1,    93,    96,    94,    -1,    53,
      81,    54,    46,    81,    -1,    35,    46,    81,    -1,    81,
      -1,    -1,    45,    -1,    47,    -1,    45,    -1,    -1,    96,
      -1,    35,    -1,    98,    47,    35,    -1,    -1,    30,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   177,   177,   180,   180,   184,   185,   186,   187,   225,
     229,   233,   234,   235,   239,   240,   241,   245,   246,   247,
     248,   252,   253,   254,   255,   256,   260,   261,   262,   266,
     267,   268,   269,   270,   274,   275,   276,   282,   283,   284,
     285,   286,   288,   290,   292,   294,   295,   297,   305,   306,
     307,   323,   329,   330,   334,   335,   339,   343,   344,   350,
     351,   352,   358,   359,   360,   364,   365,   366,   372,   373,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
     389,   390,   391,   392,   393,   394,   395,   396,   397,   398,
     399,   400,   401,   402,   403,   404,   405,   406,   410,   411,
     418,   419,   426,   427,   428,   432,   433,   439,   440,   445,
     446,   450,   451,   452,   453,   457,   461,   462,   463,   464,
     470,   471,   471,   479,   480,   484,   485,   486,   499,   499,
     501,   501,   503,   503,   506,   507,   511,   512
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
  "'-'", "'*'", "'/'", "'%'", "UNARY_OPERATOR", "'^'", "';'", "'='", "','",
  "':'", "'.'", "'('", "')'", "'#'", "'['", "']'", "'{'", "'}'", "$accept",
  "file", "opt_block", "$@1", "opt_block_statements", "class_1_statement",
  "class_2_statement", "class_3_statement", "class_4_statement",
  "statement_list", "statement_list_1", "statement_list_2",
  "statement_list_3", "statement_list_4", "nobr_statement", "br_statement",
  "opt_elseif_block_list", "elseif_block_list", "elseif_block",
  "opt_else_block", "last_statement", "nobr_variable_list",
  "br_variable_list", "func_name_list", "expression", "expression_list",
  "nobr_prefix_expression", "nobr_variable", "br_variable",
  "nobr_function_call", "br_function_call", "arguments", "function_body",
  "opt_parameter_list", "table_constructor", "$@2", "field_list", "field",
  "opt_semicolon", "field_separator", "opt_field_separator",
  "identifier_list", "opt_special", 0
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
      42,    47,    37,   291,    94,    59,    61,    44,    58,    46,
      40,    41,    35,    91,    93,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    57,    58,    60,    59,    61,    61,    61,    61,    62,
      63,    64,    64,    64,    65,    65,    65,    66,    66,    66,
      66,    67,    67,    67,    67,    67,    68,    68,    68,    69,
      69,    69,    69,    69,    70,    70,    70,    71,    71,    71,
      71,    71,    71,    71,    71,    71,    71,    71,    71,    71,
      71,    72,    73,    73,    74,    74,    75,    76,    76,    77,
      77,    77,    78,    78,    78,    79,    79,    79,    80,    80,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    82,    82,
      83,    83,    84,    84,    84,    85,    85,    86,    86,    87,
      87,    88,    88,    88,    88,    89,    90,    90,    90,    90,
      91,    92,    91,    93,    93,    94,    94,    94,    95,    95,
      96,    96,    97,    97,    98,    98,    99,    99
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     0,     1,     1,     2,     1,
       1,     2,     3,     2,     1,     2,     2,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     1,     2,     2,     1,
       2,     2,     2,     2,     1,     2,     2,     3,     3,     5,
       4,     7,     9,    11,     7,     3,     5,     4,     2,     4,
       2,     3,     0,     1,     1,     2,     4,     0,     2,     2,
       3,     2,     1,     3,     3,     1,     3,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     2,     1,     3,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     2,     1,     3,
       1,     1,     1,     4,     3,     6,     5,     2,     4,     4,
       6,     2,     3,     1,     1,     5,     0,     1,     1,     3,
       2,     0,     5,     1,     3,     5,     3,     1,     0,     1,
       1,     1,     0,     1,     1,     3,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     5,     1,   128,     3,     0,     0,     0,
       0,     3,   128,     0,   102,     0,     4,    21,    26,    29,
      34,     7,    17,    18,    19,    20,   136,    14,     6,     0,
       0,     0,   100,    65,     9,    10,   129,    61,     0,   134,
       0,    68,     0,    71,     0,    70,     0,    72,    75,    73,
      74,   102,     0,     0,     0,   121,     0,    77,   100,   101,
      79,     0,   134,    48,     0,    98,   128,    59,     0,    50,
       0,     8,    22,    30,    23,    31,    24,    27,    32,    35,
      25,    28,    33,    36,   137,   136,    11,    15,     0,     0,
       0,     0,   114,     0,     0,     0,     0,   107,   113,    13,
      16,    38,     0,     0,     0,     0,     0,   116,    45,    76,
      95,    96,     0,    97,   120,     0,     0,     0,     3,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    60,     3,     0,    12,
      37,     0,   100,    64,    51,   100,    67,     0,   104,   111,
       0,     0,     0,     0,   135,     0,    69,   117,     0,   118,
      78,   102,     0,   127,   132,   123,    93,    94,    52,    80,
      91,    90,    88,    92,    87,    89,    81,    82,    83,    84,
      86,    85,    47,    49,    40,    99,     0,     0,     0,     0,
     109,     0,   108,   112,   103,     0,     3,    46,     3,     0,
       0,     0,   131,   130,   133,     0,     0,    57,    53,    54,
      39,     0,   106,     0,     0,     0,     0,     0,   119,   126,
       0,   124,   122,     0,     3,     0,    55,   110,   105,     3,
       0,    44,   115,     0,     3,    58,    41,     0,     0,   125,
      56,    42,     3,     0,    43
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     3,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,   207,   208,   209,   225,
      28,    29,    30,    42,    65,    66,    57,    58,    33,    59,
      35,    97,   108,   158,    60,   115,   164,   165,    37,   204,
     205,    40,    86
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -135
static const yytype_int16 yypact[] =
{
    -135,    11,  -135,   196,  -135,   -28,  -135,   -15,   -12,   429,
       4,  -135,   332,   429,    15,   429,  -135,  -135,  -135,  -135,
    -135,    36,    68,    68,   358,   358,    20,    31,  -135,    16,
      42,   384,    64,  -135,   425,    40,  -135,  -135,    73,    53,
      -4,  -135,    99,  -135,    89,  -135,   429,  -135,  -135,  -135,
    -135,  -135,   429,   429,   429,    95,   522,   384,  -135,  -135,
    -135,   110,  -135,   116,   132,   634,    -3,  -135,   550,  -135,
     242,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,   134,  -135,  -135,   429,    25,
     429,    25,  -135,   137,   147,   364,   429,  -135,  -135,  -135,
    -135,  -135,   429,   429,   149,   150,   151,    58,  -135,  -135,
     143,   143,   270,   143,  -135,   396,   429,   429,  -135,   429,
     429,   429,   429,   429,   429,   429,   429,   429,   429,   429,
     429,   429,    89,   429,   429,   429,  -135,  -135,   520,  -135,
     142,   429,   128,  -135,   142,   131,  -135,    -9,  -135,  -135,
     -29,    30,   494,     1,  -135,    89,  -135,  -135,   140,   146,
    -135,   156,   429,   634,    47,  -135,   672,   653,   197,   320,
     320,   691,   691,   320,   691,   691,    65,    65,   143,   143,
     143,   143,  -135,   142,   634,   634,   201,   176,   177,   429,
    -135,   298,  -135,  -135,  -135,   429,  -135,  -135,  -135,    62,
     429,   129,  -135,  -135,   396,   157,   429,   211,   197,  -135,
    -135,    -9,  -135,   213,   -25,   466,   212,   214,  -135,   634,
     175,  -135,  -135,   578,  -135,   216,  -135,  -135,  -135,  -135,
     429,  -135,  -135,   429,  -135,  -135,  -135,   217,   606,   634,
    -135,  -135,  -135,   218,  -135
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -135,  -135,    -6,  -135,  -135,   113,   135,   119,   155,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,    19,  -135,
     222,  -135,  -135,  -135,    -1,   -69,     7,    13,    61,   173,
    -135,  -134,   -42,  -135,   -30,  -135,  -135,    29,    -5,  -135,
    -135,    -7,   153
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -102
static const yytype_int16 yytable[] =
{
      38,    98,   109,    63,   190,    64,   196,    67,    56,   103,
      31,     4,    68,   192,    70,    61,    32,    36,   135,   140,
      39,   144,   193,    41,   188,    92,   150,    98,   189,    31,
      31,    31,    31,   116,   153,    32,    32,    32,    32,    62,
       5,    95,    36,   104,   135,   110,    55,   117,   135,    69,
      84,   111,   112,   113,   119,    12,   120,   121,   122,   123,
      51,   136,    88,    89,   183,    85,   124,   125,   126,   127,
     128,   129,   130,     6,   131,   141,    87,   227,     7,     8,
       9,   101,    10,   157,   194,   100,    11,   218,    90,    91,
     182,    13,   202,    62,   203,   151,    31,   154,    31,   102,
     159,   152,   142,    14,   145,   128,   129,   130,    98,   131,
     -62,   -62,   168,   197,   163,   166,   167,    98,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   186,   116,   184,   185,    72,    74,    76,    80,   107,
     191,    73,    75,    78,    82,   132,   117,   105,   106,   107,
     143,   114,   146,   119,   134,   120,   121,   122,   123,    77,
      81,   201,   133,   104,    84,   124,   125,   126,   127,   128,
     129,   130,   147,   131,   -63,   -63,    34,   -66,   -66,    79,
      83,    98,   148,   220,   154,   155,   156,   131,   213,   135,
     216,   198,   217,   199,   215,    34,    34,    34,    34,   219,
       5,     6,   200,   163,   206,   223,     7,     8,     9,   210,
      10,   211,   212,   222,    11,    12,   116,   224,   235,    13,
     231,   233,   232,   237,   236,   241,   244,   226,   240,   238,
     117,    14,   239,   221,     0,     0,   243,   119,   139,   120,
     121,   122,   123,    71,     0,   116,    15,     0,     0,   124,
     125,   126,   127,   128,   129,   130,     0,   131,     0,   117,
       0,     0,     0,     0,     0,     0,   119,   228,   120,   121,
     122,   123,     0,   116,     0,     0,     0,     0,   124,   125,
     126,   127,   128,   129,   130,     0,   131,   117,     0,     0,
       0,     0,     0,   138,   119,     0,   120,   121,   122,   123,
       0,   116,     0,     0,     0,     0,   124,   125,   126,   127,
     128,   129,   130,     0,   131,   117,     0,     0,     0,     0,
       0,   160,   119,     0,   120,   121,   122,   123,     0,     0,
       0,     0,     0,     0,   124,   125,   126,   127,   128,   129,
     130,    43,   131,    44,   119,     0,     0,    45,    46,   214,
       0,     0,     0,    47,     0,     0,     0,    48,   126,   127,
     128,   129,   130,     6,   131,    49,    50,    51,     7,     8,
       9,    52,    10,    43,     0,    44,    11,    36,     0,    45,
      46,    13,    53,     0,    54,    47,     0,    55,     0,    48,
       0,     0,     0,    14,     0,     0,     0,    49,    50,    51,
       0,     0,     0,    52,     0,    43,     0,    44,    15,     0,
       0,    45,    46,     0,    53,   149,    54,    47,    92,    55,
       0,    48,     0,     0,     0,     0,     0,     0,     0,    49,
      50,   161,    93,    94,    95,    52,     0,    96,    43,    55,
      44,     0,     0,     0,    45,    46,    53,     0,    54,   162,
      47,    55,     0,     0,    48,     0,     0,     0,     0,  -101,
       0,     0,    49,    50,    51,     0,     0,     0,    52,   116,
      99,   229,     0,  -101,  -101,  -101,     0,     0,  -101,    53,
    -101,    54,     0,   117,    55,     0,     0,     0,     0,     0,
     119,     0,   120,   121,   122,   123,     0,   116,     0,     0,
       0,     0,   124,   125,   126,   127,   128,   129,   130,     0,
     131,   117,     0,   230,     0,     0,     0,     0,   119,     0,
     120,   121,   122,   123,     0,   116,     0,     0,     0,     0,
     124,   125,   126,   127,   128,   129,   130,     0,   131,   117,
       0,   195,   118,     0,     0,     0,   119,     0,   120,   121,
     122,   123,     0,   116,    92,   137,     0,     0,   124,   125,
     126,   127,   128,   129,   130,     0,   131,   117,   187,   188,
      95,     0,     0,   189,   119,    55,   120,   121,   122,   123,
       0,   116,     0,     0,     0,     0,   124,   125,   126,   127,
     128,   129,   130,     0,   131,   117,     0,     0,   234,     0,
       0,     0,   119,     0,   120,   121,   122,   123,     0,   116,
       0,   242,     0,     0,   124,   125,   126,   127,   128,   129,
     130,     0,   131,   117,     0,     0,     0,     0,     0,     0,
     119,     0,   120,   121,   122,   123,     0,   116,     0,     0,
       0,     0,   124,   125,   126,   127,   128,   129,   130,     0,
     131,   117,     0,     0,     0,     0,   116,     0,   119,     0,
     120,   121,   122,   123,     0,     0,     0,     0,     0,     0,
     124,   125,   126,   127,   128,   129,   130,   119,   131,   120,
     121,   122,   123,     0,     0,     0,     0,     0,     0,   124,
     125,   126,   127,   128,   129,   130,   119,   131,   120,   121,
     122,   123,     0,     0,     0,     0,     0,     0,   124,   125,
     126,   127,   128,   129,   130,   119,   131,   120,     0,     0,
     123,     0,     0,     0,     0,     0,     0,     0,     0,   126,
     127,   128,   129,   130,     0,   131
};

static const yytype_int16 yycheck[] =
{
       6,    31,    44,    10,   138,    11,     5,    12,     9,    13,
       3,     0,    13,   147,    15,    11,     3,    45,    47,    88,
      35,    90,    51,    35,    49,    34,    95,    57,    53,    22,
      23,    24,    25,     3,   103,    22,    23,    24,    25,    35,
       4,    50,    45,    47,    47,    46,    55,    17,    47,    34,
      30,    52,    53,    54,    24,    19,    26,    27,    28,    29,
      35,    66,    46,    47,   133,    45,    36,    37,    38,    39,
      40,    41,    42,     5,    44,    50,    45,   211,    10,    11,
      12,     8,    14,    25,    54,    45,    18,    25,    46,    47,
     132,    23,    45,    35,    47,    96,    89,    35,    91,    46,
     107,   102,    89,    35,    91,    40,    41,    42,   138,    44,
      46,    47,   118,   155,   115,   116,   117,   147,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   137,     3,   134,   135,    22,    23,    24,    25,    50,
     141,    22,    23,    24,    25,    35,    17,    48,    49,    50,
      89,    56,    91,    24,    22,    26,    27,    28,    29,    24,
      25,   162,    46,    47,    30,    36,    37,    38,    39,    40,
      41,    42,    35,    44,    46,    47,     3,    46,    47,    24,
      25,   211,    35,    54,    35,    35,    35,    44,   189,    47,
     196,    51,   198,    47,   195,    22,    23,    24,    25,   200,
       4,     5,    46,   204,     7,   206,    10,    11,    12,     8,
      14,    35,    35,    56,    18,    19,     3,     6,   224,    23,
       8,    46,     8,   229,     8,     8,     8,   208,   234,   230,
      17,    35,   233,   204,    -1,    -1,   242,    24,    85,    26,
      27,    28,    29,    21,    -1,     3,    50,    -1,    -1,    36,
      37,    38,    39,    40,    41,    42,    -1,    44,    -1,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    24,    54,    26,    27,
      28,    29,    -1,     3,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    40,    41,    42,    -1,    44,    17,    -1,    -1,
      -1,    -1,    -1,    51,    24,    -1,    26,    27,    28,    29,
      -1,     3,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      40,    41,    42,    -1,    44,    17,    -1,    -1,    -1,    -1,
      -1,    51,    24,    -1,    26,    27,    28,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    40,    41,
      42,     9,    44,    11,    24,    -1,    -1,    15,    16,    51,
      -1,    -1,    -1,    21,    -1,    -1,    -1,    25,    38,    39,
      40,    41,    42,     5,    44,    33,    34,    35,    10,    11,
      12,    39,    14,     9,    -1,    11,    18,    45,    -1,    15,
      16,    23,    50,    -1,    52,    21,    -1,    55,    -1,    25,
      -1,    -1,    -1,    35,    -1,    -1,    -1,    33,    34,    35,
      -1,    -1,    -1,    39,    -1,     9,    -1,    11,    50,    -1,
      -1,    15,    16,    -1,    50,    51,    52,    21,    34,    55,
      -1,    25,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      34,    35,    48,    49,    50,    39,    -1,    53,     9,    55,
      11,    -1,    -1,    -1,    15,    16,    50,    -1,    52,    53,
      21,    55,    -1,    -1,    25,    -1,    -1,    -1,    -1,    34,
      -1,    -1,    33,    34,    35,    -1,    -1,    -1,    39,     3,
      45,     5,    -1,    48,    49,    50,    -1,    -1,    53,    50,
      55,    52,    -1,    17,    55,    -1,    -1,    -1,    -1,    -1,
      24,    -1,    26,    27,    28,    29,    -1,     3,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    40,    41,    42,    -1,
      44,    17,    -1,    47,    -1,    -1,    -1,    -1,    24,    -1,
      26,    27,    28,    29,    -1,     3,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    40,    41,    42,    -1,    44,    17,
      -1,    47,    20,    -1,    -1,    -1,    24,    -1,    26,    27,
      28,    29,    -1,     3,    34,     5,    -1,    -1,    36,    37,
      38,    39,    40,    41,    42,    -1,    44,    17,    48,    49,
      50,    -1,    -1,    53,    24,    55,    26,    27,    28,    29,
      -1,     3,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      40,    41,    42,    -1,    44,    17,    -1,    -1,    20,    -1,
      -1,    -1,    24,    -1,    26,    27,    28,    29,    -1,     3,
      -1,     5,    -1,    -1,    36,    37,    38,    39,    40,    41,
      42,    -1,    44,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      24,    -1,    26,    27,    28,    29,    -1,     3,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    40,    41,    42,    -1,
      44,    17,    -1,    -1,    -1,    -1,     3,    -1,    24,    -1,
      26,    27,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    40,    41,    42,    24,    44,    26,
      27,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    40,    41,    42,    24,    44,    26,    27,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    40,    41,    42,    24,    44,    26,    -1,    -1,
      29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,
      39,    40,    41,    42,    -1,    44
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    58,    59,    60,     0,     4,     5,    10,    11,    12,
      14,    18,    19,    23,    35,    50,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    77,    78,
      79,    83,    84,    85,    86,    87,    45,    95,    59,    35,
      98,    35,    80,     9,    11,    15,    16,    21,    25,    33,
      34,    35,    39,    50,    52,    55,    81,    83,    84,    86,
      91,    11,    35,    98,    59,    81,    82,    95,    81,    34,
      81,    77,    62,    64,    62,    64,    62,    63,    64,    65,
      62,    63,    64,    65,    30,    45,    99,    45,    46,    47,
      46,    47,    34,    48,    49,    50,    53,    88,    91,    45,
      45,     8,    46,    13,    47,    48,    49,    50,    89,    89,
      81,    81,    81,    81,    56,    92,     3,    17,    20,    24,
      26,    27,    28,    29,    36,    37,    38,    39,    40,    41,
      42,    44,    35,    46,    22,    47,    95,     5,    51,    99,
      82,    50,    84,    85,    82,    84,    85,    35,    35,    51,
      82,    81,    81,    82,    35,    35,    35,    25,    90,    98,
      51,    35,    53,    81,    93,    94,    81,    81,    59,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    89,    82,    81,    81,    59,    48,    49,    53,
      88,    81,    88,    51,    54,    47,     5,    89,    51,    47,
      46,    81,    45,    47,    96,    97,     7,    73,    74,    75,
       8,    35,    35,    81,    51,    81,    59,    59,    25,    81,
      54,    94,    56,    81,     6,    76,    75,    88,    54,     5,
      47,     8,     8,    46,    20,    59,     8,    59,    81,    81,
      59,     8,     5,    59,     8
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
#line 177 "lua_parser.y"
    { printf("%s", (yyvsp[(1) - (1)])); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 180 "lua_parser.y"
    { lua_parser_indent++; ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 181 "lua_parser.y"
    { lua_parser_indent--; (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 184 "lua_parser.y"
    { (yyval) = ""; ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 187 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 225 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (1)]).filename, (yylsp[(1) - (1)]).line, INDENT"%s;\n", (yyvsp[(1) - (1)])); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 229 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (1)]).filename, (yylsp[(1) - (1)]).line, INDENT"%s;\n", (yyvsp[(1) - (1)])); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 233 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]), true); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 234 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), true); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 235 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, INDENT"%s;\n", (yyvsp[(1) - (2)])); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 241 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, INDENT"%s;\n", (yyvsp[(1) - (2)])); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 253 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 254 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 255 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 256 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 261 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 262 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 267 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 268 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 269 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 270 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 275 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 276 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 282 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, INDENT"%s = %s;\n", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 283 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, INDENT"do\n%s" INDENT "end\n", (yyvsp[(2) - (3)])); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 284 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (5)]).filename, (yylsp[(1) - (5)]).line, INDENT"while %s do\n%s" INDENT "end\n", (yyvsp[(2) - (5)]), (yyvsp[(4) - (5)])); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 285 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (4)]).filename, (yylsp[(1) - (4)]).line, INDENT"repeat\n%s" INDENT "until %s;\n", (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)])); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 287 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (7)]).filename, (yylsp[(1) - (7)]).line, INDENT"if %s then\n%s%s%s" INDENT "end\n", (yyvsp[(2) - (7)]), (yyvsp[(4) - (7)]), (yyvsp[(5) - (7)]), (yyvsp[(6) - (7)])); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 289 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (9)]).filename, (yylsp[(1) - (9)]).line, INDENT"for %s = %s,%s,1 do\n%s" INDENT "end\n", (yyvsp[(2) - (9)]), (yyvsp[(4) - (9)]), (yyvsp[(6) - (9)]), (yyvsp[(8) - (9)])); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 291 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (11)]).filename, (yylsp[(1) - (11)]).line, INDENT"for %s = %s,%s,%s do\n%s" INDENT "end\n", (yyvsp[(2) - (11)]), (yyvsp[(4) - (11)]), (yyvsp[(6) - (11)]), (yyvsp[(8) - (11)]), (yyvsp[(10) - (11)])); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 293 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (7)]).filename, (yylsp[(1) - (7)]).line, INDENT"for %s in %s do\n%s" INDENT "end\n", (yyvsp[(2) - (7)]), (yyvsp[(4) - (7)]), (yyvsp[(6) - (7)])); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 294 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, INDENT"%s = function(%s;\n", (yyvsp[(2) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 296 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (5)]).filename, (yylsp[(1) - (5)]).line, INDENT"%s['%s'] = function(self, %s;\n", (yyvsp[(2) - (5)]), (yyvsp[(4) - (5)]), (yyvsp[(5) - (5)])); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 297 "lua_parser.y"
    {
						  if (flag_luac_mode) {
						    (yyval) = String((yylsp[(1) - (4)]).filename, (yylsp[(1) - (4)]).line, INDENT"local function %s (%s\n" , (yyvsp[(3) - (4)]), (yyvsp[(4) - (4)]));
						  } else {
						    // This generates extra LOADNIL instructions when passed to luac.
						    (yyval) = String((yylsp[(1) - (4)]).filename, (yylsp[(1) - (4)]).line, INDENT"local %s;\n" INDENT "%s = function(%s;\n" , (yyvsp[(3) - (4)]), (yyvsp[(3) - (4)]), (yyvsp[(4) - (4)]));
						  }
						;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 305 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, INDENT"local %s;\n", (yyvsp[(2) - (2)])); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 306 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (4)]).filename, (yylsp[(1) - (4)]).line, INDENT"local %s = %s;\n", (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)])); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 307 "lua_parser.y"
    {
						  // Separately handle these function call statements so that
						  // we can expand outer-scope dofiles inline.
						  if (strcmp((yyvsp[(1) - (2)]), "dofile") == 0 && lua_parser_indent == 0) {
						    CHECK(strcmp(yylval, (yyvsp[(2) - (2)])) == 0);	// Ensure lexxer hasn't read past the string
						    (yyval) = "";
						    LexxerPushFile((yyvsp[(2) - (2)]));
						  } else {
						    (yyval) = String((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, INDENT"%s(%s);\n", (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]));
						  }
						;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 323 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, INDENT"%s = %s;\n", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 329 "lua_parser.y"
    { (yyval) = ""; ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 335 "lua_parser.y"
    { (yyval) = Concat((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 339 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (4)]).filename, (yylsp[(1) - (4)]).line, INDENT"elseif %s then\n%s", (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)])); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 343 "lua_parser.y"
    { (yyval) = ""; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 344 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, INDENT"else\n%s", (yyvsp[(2) - (2)])); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 350 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, INDENT"return;\n"); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 351 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, INDENT"return %s;\n", (yyvsp[(2) - (3)])); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 352 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, INDENT"break;"); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 359 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s, %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 360 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s, %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 365 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s, %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 366 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s, %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 373 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s['%s']", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 379 "lua_parser.y"
    { (yyval) = "nil"; ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 380 "lua_parser.y"
    { (yyval) = "false"; ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 381 "lua_parser.y"
    { (yyval) = "true"; ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 384 "lua_parser.y"
    { (yyval) = "..."; ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 385 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, "function(%s", (yyvsp[(2) - (2)])); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 387 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "(%s)", (yyvsp[(2) - (3)])); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 389 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s .. %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 390 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s + %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 391 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s - %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 392 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s * %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 393 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s / %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 394 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s ^ %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 395 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s %% %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 396 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s < %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 397 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s <= %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 398 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s > %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 399 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s >= %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 400 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s == %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 401 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s ~= %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 402 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s and %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 403 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s or %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 404 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, "not %s", (yyvsp[(2) - (2)])); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 405 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, "- %s", (yyvsp[(2) - (2)])); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 406 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, "# %s", (yyvsp[(2) - (2)])); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 411 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s, %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 427 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (4)]).filename, (yylsp[(1) - (4)]).line, "%s[%s]", (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)])); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 428 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s['%s']", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 432 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (6)]).filename, (yylsp[(1) - (6)]).line, "(%s)[%s]", (yyvsp[(2) - (6)]), (yyvsp[(5) - (6)])); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 433 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (5)]).filename, (yylsp[(1) - (5)]).line, "(%s)['%s']", (yyvsp[(2) - (5)]), (yyvsp[(5) - (5)])); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 439 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (2)]).filename, (yylsp[(1) - (2)]).line, "%s(%s)", (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 441 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (4)]).filename, (yylsp[(1) - (4)]).line, "%s:%s(%s)", (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]), (yyvsp[(4) - (4)])); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 445 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (4)]).filename, (yylsp[(1) - (4)]).line, "(%s)(%s)", (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)])); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 446 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (6)]).filename, (yylsp[(1) - (6)]).line, "(%s):%s(%s)", (yyvsp[(2) - (6)]), (yyvsp[(5) - (6)]), (yyvsp[(6) - (6)])); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 450 "lua_parser.y"
    { (yyval) = ""; ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 451 "lua_parser.y"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 457 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (5)]).filename, (yylsp[(1) - (5)]).line, "%s)\n%s" INDENT "end", (yyvsp[(2) - (5)]), (yyvsp[(4) - (5)])); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 461 "lua_parser.y"
    { (yyval) = ""; ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 462 "lua_parser.y"
    { (yyval) = "..."; ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 464 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s, ...", (yyvsp[(1) - (3)])); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 470 "lua_parser.y"
    { (yyval) = "{}"; ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 471 "lua_parser.y"
    { lua_parser_indent++; array_index.push_back(1); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 471 "lua_parser.y"
    {
						  lua_parser_indent--;
						  array_index.pop_back();
						  (yyval) = String((yylsp[(1) - (5)]).filename, (yylsp[(1) - (5)]).line, "{\n%s\n" INDENT "}", (yyvsp[(3) - (5)]));
						;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 479 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (1)]).filename, (yylsp[(1) - (1)]).line, INDENT"%s", (yyvsp[(1) - (1)])); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 480 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s,\n" INDENT "%s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 484 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (5)]).filename, (yylsp[(1) - (5)]).line, "[%s] = %s", (yyvsp[(2) - (5)]), (yyvsp[(5) - (5)])); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 485 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "['%s'] = %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 486 "lua_parser.y"
    {
						  if (flag_luac_mode) {
						    (yyval) = (yyvsp[(1) - (1)]);
						  } else {
						    // This generates SETTABLE instead of SETLIST
						    // instructions when passed to luac.
						    (yyval) = String((yylsp[(1) - (1)]).filename, (yylsp[(1) - (1)]).line, "[%d] = %s", array_index.back()++, (yyvsp[(1) - (1)]));
						  }
						;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 507 "lua_parser.y"
    { (yyval) = String((yylsp[(1) - (3)]).filename, (yylsp[(1) - (3)]).line, "%s, %s", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 511 "lua_parser.y"
    { (yyval) = ""; ;}
    break;



/* Line 1455 of yacc.c  */
#line 2611 "lua_parser.cpp"
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
#line 515 "lua_parser.y"


int lua_parser_error (char *s) {
  Panic("%s, at %s:%d", s, LexxerFilename(), LexxerLinecount());
#ifdef _MSC_VER
  return 0;
#endif
}

char *String(const char *filename, int line_number, const char *msg, ...) {
  char *buffer = 0;
  // Replace the special word '@MarkerAndIndent@' with the filename:line_number
  // (if the -emit_lines flag is set) and indent spaces.
#ifndef _MSC_VER
  if (strstr(msg, "@MarkerAndIndent@")) {
#else
  //asprintf不支持循环替换
  while (strstr(msg, "@MarkerAndIndent@")) {	
#endif
	// Copy the format string and deactivate all %'s.
    char *tmp1 = strdup(msg);
    for (char *s_ = tmp1; *s_; s_++) {
      if (*s_ == '%') *s_ = -'%';
    }
    // Replace all @MarkerAndIndent@ markers by the appropriate printf() format string.
#ifndef _MSC_VER
	char *ind = tmp1 - 17;
    while((ind = strstr(ind + 17, "@MarkerAndIndent@"))) {
      if (flag_emit_lines) {
#else
	char *ind = tmp1;
    while((ind = strstr(ind, "@MarkerAndIndent@"))) {
      if (flag_emit_lines) {
#endif
#ifndef _MSC_VER
        memcpy(ind, "@%3$s:%4$d%2$*1$s", 17);
#else
		memcpy(ind, "@%s:%d %s", 9);//9
		int i;
		int len = strlen(tmp1) + 1 - (ind - tmp1) - 17;
		for(i = 0; i < len; i++)
		{
			// 9
			* (ind + 9 + i) = * (ind + 17 + i);
		}
		//TODO:这里会有多个替换的情况（促发bug）,所以用break跳出
		break;
#endif
      } else {
#ifndef _MSC_VER
		memcpy(ind, "          %2$*1$s", 17);
#else
		//memcpy(ind, "%s %d ", 6);
	    //memcpy(ind, "           %s", 13); //2
	    memcpy(ind, "%s", 2); //2
		int i;
		int len = strlen(tmp1) + 1 - (ind - tmp1) - 17;
		for(i = 0; i < len; i++)
		{
			//2
			//* (ind + 13 + i) = * (ind + 17 + i);
			* (ind + 2 + i) = * (ind + 17 + i);
		}
		//TODO:这里会有多个替换的情况（促发bug）,所以用break跳出
		break;
#endif
      }
	}
    // Do the @MarkerAndIndent@ substitution.
#ifndef _MSC_VER
    asprintf(&buffer, tmp1, lua_parser_indent*2 + 1, "", filename, line_number);
#else
	unsigned char *space = (unsigned char *)malloc(sizeof(unsigned char) * (lua_parser_indent*2 + 1 + 1));
	memset(space, ' ', lua_parser_indent*2 + 1);
	if(flag_emit_lines)
	{
		space[lua_parser_indent*2 + 1] = '\0';
		asprintf(&buffer, tmp1, filename, line_number, space);//lua_parser_indent*2 + 1);
	}
	else
	{
		space[lua_parser_indent*2 + 1] = '\0';
		asprintf(&buffer, tmp1, space);
	}
	//printf("%s\n", tmp1);
	//printf("==========================\n");
	free(space);
#endif
	free(tmp1);
    // Reactivate the original %'s.
    for (char *s = buffer; *s; s++) {
      if (*s == -'%') *s = '%';
    }
    msg = buffer;
  }

  // Run sprintf.
  va_list ap;
  va_start(ap, msg);
  char *ret = 0;
  vasprintf(&ret, msg, ap);
  if (buffer) free(buffer);
  return ret;
}

char *Concat(char *s1, const char *s2, bool trim_trailing_newline) {
  if (trim_trailing_newline && (s1[0] || s2[0])) {
    int len = strlen(s1);
    CHECK(len > 0 && s1[len-1] == '\n');
    s1[len-1] = 0;
    return String(0, 0, "%s%s\n", s1, s2);
  } else {
    return String(0, 0, "%s%s", s1, s2);
  }
}

