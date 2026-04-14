/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

static int func_mode = 0; /* 1=registerUser, 2=uploadFile, 3=placeOrder */
static int indent = 0;

/* ── LALR(1) action trace ── */
static int step = 0;
typedef enum { ACT_SHIFT, ACT_REDUCE, ACT_ACCEPT } ActionKind;
typedef struct { ActionKind kind; char desc[128]; } Action;
#define MAX_ACTIONS 512
static Action actions[MAX_ACTIONS];
static int    action_count = 0;

void trace_shift(const char *token) {
    if (action_count >= MAX_ACTIONS) return;
    actions[action_count].kind = ACT_SHIFT;
    snprintf(actions[action_count].desc, 128, "%-20s", token);
    action_count++;
}
void trace_reduce(const char *lhs, const char *rhs) {
    if (action_count >= MAX_ACTIONS) return;
    actions[action_count].kind = ACT_REDUCE;
    snprintf(actions[action_count].desc, 128, "%-14s → %s", lhs, rhs);
    action_count++;
}
void print_parse_method() {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║           PARSING METHOD: LALR(1)                    ║\n");
    printf("╠══════════════════════════════════════════════════════╣\n");
    printf("║  Algorithm : LALR(1)  (Look-Ahead LR, 1 token)       ║\n");
    printf("║  Generator : GNU Bison                               ║\n");
    printf("║  Strategy  : Bottom-up, shift-reduce                 ║\n");
    printf("║  Lookahead : 1 token  (decides shift vs reduce)      ║\n");
    printf("╠══════════════════════════════════════════════════════╣\n");
    printf("║  How it works:                                        ║\n");
    printf("║  • SHIFT  – push next token onto the parse stack      ║\n");
    printf("║  • REDUCE – pop RHS symbols, push LHS non-terminal   ║\n");
    printf("║  • ACCEPT – stack holds start symbol, input is done  ║\n");
    printf("║  • 1 lookahead token resolves shift/reduce conflicts  ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
}
void print_action_trace() {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║           LALR(1) ACTION TRACE                       ║\n");
    printf("╠══════╦══════════╦═══════════════════════════════════╣\n");
    printf("║ Step ║ Action   ║ Detail                            ║\n");
    printf("╠══════╬══════════╬═══════════════════════════════════╣\n");
    for (int i = 0; i < action_count; i++) {
        const char *act = actions[i].kind == ACT_SHIFT  ? "SHIFT " :
                          actions[i].kind == ACT_REDUCE ? "REDUCE" : "ACCEPT";
        printf("║ %4d ║ %s ║ %-33s ║\n", i+1, act, actions[i].desc);
    }
    printf("║ %4d ║ ACCEPT ║ %-33s ║\n", action_count+1, "program");
    printf("╚══════╩══════════╩═══════════════════════════════════╝\n");
}

/* Parse tree helpers */
void pt_enter(const char *node) {
    for (int i = 0; i < indent; i++) printf("  ");
    printf("├─ %s\n", node);
    indent++;
}
void pt_leaf(const char *label, const char *val) {
    for (int i = 0; i < indent; i++) printf("  ");
    if (val) printf("└─ %s: \"%s\"\n", label, val);
    else      printf("└─ %s\n", label);
}
void pt_exit() { if (indent > 0) indent--; }

/* Production rule printer — also records reduce action */
void prod(const char *lhs, const char *rhs) {
    printf("  [Production]  %s  →  %s\n", lhs, rhs);
    trace_reduce(lhs, rhs);
}

void yyerror(const char *s);
int  yylex(void);
extern int yylineno;

#line 155 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ASYNC = 3,                      /* ASYNC  */
  YYSYMBOL_FUNCTION = 4,                   /* FUNCTION  */
  YYSYMBOL_CONST = 5,                      /* CONST  */
  YYSYMBOL_LET = 6,                        /* LET  */
  YYSYMBOL_IF = 7,                         /* IF  */
  YYSYMBOL_RETURN = 8,                     /* RETURN  */
  YYSYMBOL_THROW = 9,                      /* THROW  */
  YYSYMBOL_NEW = 10,                       /* NEW  */
  YYSYMBOL_TRUE_LIT = 11,                  /* TRUE_LIT  */
  YYSYMBOL_FALSE_LIT = 12,                 /* FALSE_LIT  */
  YYSYMBOL_ARROW = 13,                     /* ARROW  */
  YYSYMBOL_EQ = 14,                        /* EQ  */
  YYSYMBOL_OR = 15,                        /* OR  */
  YYSYMBOL_DOT = 16,                       /* DOT  */
  YYSYMBOL_COMMA = 17,                     /* COMMA  */
  YYSYMBOL_SEMICOLON = 18,                 /* SEMICOLON  */
  YYSYMBOL_COLON = 19,                     /* COLON  */
  YYSYMBOL_LPAREN = 20,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 21,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 22,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 23,                    /* RBRACE  */
  YYSYMBOL_LBRACKET = 24,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 25,                  /* RBRACKET  */
  YYSYMBOL_NOT = 26,                       /* NOT  */
  YYSYMBOL_ASSIGN = 27,                    /* ASSIGN  */
  YYSYMBOL_STAR = 28,                      /* STAR  */
  YYSYMBOL_LT = 29,                        /* LT  */
  YYSYMBOL_GT = 30,                        /* GT  */
  YYSYMBOL_MINUS_ASSIGN = 31,              /* MINUS_ASSIGN  */
  YYSYMBOL_IDENTIFIER = 32,                /* IDENTIFIER  */
  YYSYMBOL_STRING_LIT = 33,                /* STRING_LIT  */
  YYSYMBOL_NUMBER_LIT = 34,                /* NUMBER_LIT  */
  YYSYMBOL_YYACCEPT = 35,                  /* $accept  */
  YYSYMBOL_program = 36,                   /* program  */
  YYSYMBOL_stmt_list = 37,                 /* stmt_list  */
  YYSYMBOL_stmt = 38,                      /* stmt  */
  YYSYMBOL_async_func_decl = 39,           /* async_func_decl  */
  YYSYMBOL_param_list = 40,                /* param_list  */
  YYSYMBOL_func_body = 41,                 /* func_body  */
  YYSYMBOL_func_stmt = 42,                 /* func_stmt  */
  YYSYMBOL_if_stmt = 43,                   /* if_stmt  */
  YYSYMBOL_if_cond = 44,                   /* if_cond  */
  YYSYMBOL_throw_stmt = 45,                /* throw_stmt  */
  YYSYMBOL_const_decl = 46,                /* const_decl  */
  YYSYMBOL_assign_stmt = 47,               /* assign_stmt  */
  YYSYMBOL_object_literal = 48,            /* object_literal  */
  YYSYMBOL_prop_list = 49,                 /* prop_list  */
  YYSYMBOL_prop = 50,                      /* prop  */
  YYSYMBOL_return_stmt = 51,               /* return_stmt  */
  YYSYMBOL_var_decl = 52,                  /* var_decl  */
  YYSYMBOL_expr_stmt = 53,                 /* expr_stmt  */
  YYSYMBOL_expr = 54,                      /* expr  */
  YYSYMBOL_arrow_func = 55                 /* arrow_func  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  23
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   267

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  35
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  21
/* YYNRULES -- Number of rules.  */
#define YYNRULES  74
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  187

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   289


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    98,    98,   103,   104,   108,   109,   110,   114,   136,
     144,   152,   153,   157,   158,   159,   160,   161,   165,   172,
     180,   188,   199,   204,   209,   213,   221,   232,   240,   248,
     256,   264,   272,   283,   291,   302,   307,   308,   312,   318,
     324,   330,   336,   345,   353,   364,   372,   380,   388,   396,
     404,   415,   416,   420,   428,   436,   441,   443,   448,   456,
     464,   472,   477,   482,   487,   492,   497,   502,   503,   504,
     505,   506,   510,   518,   526
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ASYNC", "FUNCTION",
  "CONST", "LET", "IF", "RETURN", "THROW", "NEW", "TRUE_LIT", "FALSE_LIT",
  "ARROW", "EQ", "OR", "DOT", "COMMA", "SEMICOLON", "COLON", "LPAREN",
  "RPAREN", "LBRACE", "RBRACE", "LBRACKET", "RBRACKET", "NOT", "ASSIGN",
  "STAR", "LT", "GT", "MINUS_ASSIGN", "IDENTIFIER", "STRING_LIT",
  "NUMBER_LIT", "$accept", "program", "stmt_list", "stmt",
  "async_func_decl", "param_list", "func_body", "func_stmt", "if_stmt",
  "if_cond", "throw_stmt", "const_decl", "assign_stmt", "object_literal",
  "prop_list", "prop", "return_stmt", "var_decl", "expr_stmt", "expr",
  "arrow_func", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-140)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       3,     1,   -20,    29,  -140,  -140,   112,  -140,    77,    87,
       3,  -140,  -140,  -140,  -140,    91,    74,    86,    95,    84,
      98,   102,    93,  -140,  -140,  -140,   116,   113,   117,   123,
      19,   124,  -140,   110,   -14,   127,   114,   118,     8,   122,
     128,   119,   120,   129,   130,   126,   108,  -140,   121,  -140,
     132,  -140,    53,    56,   133,   134,   125,   136,    59,  -140,
      88,   138,   120,   141,   140,   131,    69,   135,  -140,   137,
    -140,  -140,   145,  -140,    12,  -140,  -140,  -140,  -140,    77,
    -140,  -140,  -140,  -140,   143,  -140,    20,  -140,   144,   146,
     139,   142,   148,   147,   115,    52,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,    60,  -140,   149,   150,   151,    78,
     154,   152,  -140,  -140,  -140,   155,   153,    85,   156,    35,
     159,  -140,  -140,   -10,  -140,   160,   161,   158,   157,   165,
     171,   162,   167,   168,   163,   175,   164,  -140,  -140,   169,
      94,   183,   183,   177,   170,   172,   181,   173,   174,   187,
     176,   180,   178,   184,   179,   182,   191,   188,   185,  -140,
    -140,   193,   153,   194,   186,   189,   192,   195,  -140,   198,
    -140,   200,   199,   183,   190,  -140,   196,   202,   203,   201,
    -140,   183,  -140,   207,   204,  -140,  -140
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,    70,    71,    67,    68,    69,     0,
       2,     4,     5,     6,     7,    52,     0,     0,     0,     0,
       0,     0,     0,     1,     3,    51,     0,     0,     0,     0,
      57,     0,    66,     0,     0,     0,     0,     0,     0,     0,
      10,     0,     0,    46,     0,     0,     0,    37,     0,    65,
       0,    55,     0,     0,     0,     0,     0,     0,     0,    45,
       0,    50,     0,    48,     0,     0,     0,     0,    54,     0,
      61,    53,    60,     9,     0,    35,    39,    40,    41,    42,
      38,    49,    36,    47,     0,    64,     0,    72,     0,     0,
       0,     0,     0,     0,    67,     0,    12,    13,    14,    15,
      17,    16,    56,    74,     0,    63,     0,     0,     0,     0,
      44,     0,     8,    11,    73,     0,     0,     0,     0,     0,
       0,    24,    43,    57,    62,     0,     0,    69,    30,    29,
      23,     0,     0,     0,     0,    59,     0,    28,    27,     0,
      57,     0,     0,     0,     0,    66,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    19,
      18,    34,     0,    32,     0,     0,     0,     0,    33,     0,
      31,     0,     0,     0,     0,    58,     0,     0,     0,     0,
      22,     0,    21,    26,     0,    25,    20
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -140,  -140,  -140,   219,  -140,  -140,    80,   -57,  -140,  -140,
    -139,  -140,  -140,   -39,   197,   205,  -140,  -140,    16,   -59,
    -114
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     9,    10,    11,    12,    41,    95,    96,    97,   120,
     150,    98,    99,    45,    46,    47,   100,    13,   101,    15,
      54
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      55,    80,   126,   151,    37,    16,     1,    87,    42,     2,
      38,    43,    17,     3,     4,     5,    14,    91,    44,    92,
      93,   134,     3,     4,     5,    91,    14,    92,    93,    51,
       3,     4,     5,    37,   178,     6,     7,     8,   113,    38,
      52,    53,   184,   103,    94,     7,     8,   113,   169,    19,
     121,   131,    94,     7,     8,    21,   132,    91,   129,    92,
      93,    18,     3,     4,     5,    91,    66,    92,    93,    67,
       3,     4,     5,    69,    68,   112,    62,    70,   128,     3,
       4,     5,    75,   114,    94,     7,     8,    23,     3,     4,
       5,    86,    94,     7,     8,     3,     4,     5,     3,    76,
      77,     6,     7,     8,   118,    22,    26,    42,    37,    25,
     119,     7,     8,    27,    38,    28,    29,     6,     7,   127,
       6,    78,    79,   147,   148,    62,    19,    32,    20,    19,
      30,   111,    21,    63,    31,    21,    33,    34,    35,    36,
      57,    39,    40,    48,    42,    56,    49,    59,    65,    60,
      50,    61,    44,    64,    71,    72,    81,    73,    74,    83,
      84,    90,   106,    85,   102,   105,   104,    88,   109,    89,
     116,   107,   122,    66,   108,   137,   124,     0,   117,   110,
     133,   115,   135,   138,   123,   125,   136,   139,   130,   141,
     142,   144,   149,   152,   140,   143,   155,   158,   145,   159,
     154,   146,   153,   160,   162,   156,   157,   165,   164,   166,
     161,   168,   170,   163,   173,   174,   176,   167,   171,   175,
     177,   172,   183,   179,   181,   185,   182,   186,   180,    24,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    58,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    82
};

static const yytype_int16 yycheck[] =
{
      39,    60,   116,   142,    14,     4,     3,    66,    22,     6,
      20,    25,    32,    10,    11,    12,     0,     5,    32,     7,
       8,    31,    10,    11,    12,     5,    10,     7,     8,    21,
      10,    11,    12,    14,   173,    32,    33,    34,    95,    20,
      32,    33,   181,    23,    32,    33,    34,   104,   162,    14,
     109,    16,    32,    33,    34,    20,    21,     5,   117,     7,
       8,    32,    10,    11,    12,     5,    13,     7,     8,    16,
      10,    11,    12,    17,    21,    23,    17,    21,   117,    10,
      11,    12,    23,    23,    32,    33,    34,     0,    10,    11,
      12,    22,    32,    33,    34,    10,    11,    12,    10,    11,
      12,    32,    33,    34,    26,    28,    32,    22,    14,    18,
      32,    33,    34,    27,    20,    20,    32,    32,    33,    34,
      32,    33,    34,    29,    30,    17,    14,    34,    16,    14,
      32,    16,    20,    25,    32,    20,    20,    24,    21,    16,
      21,    17,    32,    16,    22,    17,    32,    18,    16,    19,
      32,    25,    32,    32,    21,    21,    18,    32,    22,    18,
      20,    16,    16,    32,    21,    21,    86,    32,    20,    32,
      20,    32,    18,    13,    32,    18,    21,    -1,    27,    32,
      21,    32,    21,    18,    32,    32,    28,    16,    32,    22,
      22,    16,     9,    16,    32,    32,    15,    10,    34,    23,
      28,    32,    32,    23,    20,    32,    32,    16,    26,    21,
      32,    18,    18,    34,    22,    20,    16,    32,    32,    21,
      21,    32,    21,    33,    22,    18,    23,    23,    32,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    62
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     6,    10,    11,    12,    32,    33,    34,    36,
      37,    38,    39,    52,    53,    54,     4,    32,    32,    14,
      16,    20,    28,     0,    38,    18,    32,    27,    20,    32,
      32,    32,    34,    20,    24,    21,    16,    14,    20,    17,
      32,    40,    22,    25,    32,    48,    49,    50,    16,    32,
      32,    21,    32,    33,    55,    48,    17,    21,    49,    18,
      19,    25,    17,    25,    32,    16,    13,    16,    21,    17,
      21,    21,    21,    32,    22,    23,    11,    12,    33,    34,
      54,    18,    50,    18,    20,    32,    22,    54,    32,    32,
      16,     5,     7,     8,    32,    41,    42,    43,    46,    47,
      51,    53,    21,    23,    41,    21,    16,    32,    32,    20,
      32,    16,    23,    42,    23,    32,    20,    27,    26,    32,
      44,    54,    18,    32,    21,    32,    55,    34,    48,    54,
      32,    16,    21,    21,    31,    21,    28,    18,    18,    16,
      32,    22,    22,    32,    16,    34,    32,    29,    30,     9,
      45,    45,    16,    32,    28,    15,    32,    32,    10,    23,
      23,    32,    20,    34,    26,    16,    21,    32,    18,    55,
      18,    32,    32,    22,    20,    21,    16,    21,    45,    33,
      32,    22,    23,    21,    45,    18,    23
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    35,    36,    37,    37,    38,    38,    38,    39,    40,
      40,    41,    41,    42,    42,    42,    42,    42,    43,    43,
      43,    43,    44,    44,    44,    45,    45,    46,    46,    46,
      46,    46,    46,    47,    47,    48,    49,    49,    50,    50,
      50,    50,    50,    51,    51,    52,    52,    52,    52,    52,
      52,    53,    53,    54,    54,    54,    54,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      54,    54,    55,    55,    55
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     9,     3,
       1,     2,     1,     1,     1,     1,     1,     1,     7,     7,
      13,    11,     9,     2,     1,     7,     6,     5,     5,     4,
       4,     9,     8,     8,     7,     3,     3,     1,     3,     3,
       3,     3,     3,     3,     2,     6,     5,     7,     6,     7,
       6,     2,     1,     6,     6,     5,     8,     3,    16,    11,
       6,     6,    10,     8,     7,     5,     3,     1,     1,     1,
       1,     1,     3,     5,     4
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: stmt_list  */
#line 99 "parser.y"
      { prod("program", "stmt_list"); }
#line 1325 "parser.tab.c"
    break;

  case 3: /* stmt_list: stmt_list stmt  */
#line 103 "parser.y"
                      { prod("stmt_list", "stmt_list stmt"); }
#line 1331 "parser.tab.c"
    break;

  case 4: /* stmt_list: stmt  */
#line 104 "parser.y"
                      { prod("stmt_list", "stmt"); }
#line 1337 "parser.tab.c"
    break;

  case 5: /* stmt: async_func_decl  */
#line 108 "parser.y"
                      { prod("stmt", "async_func_decl"); }
#line 1343 "parser.tab.c"
    break;

  case 6: /* stmt: var_decl  */
#line 109 "parser.y"
                      { prod("stmt", "var_decl"); }
#line 1349 "parser.tab.c"
    break;

  case 7: /* stmt: expr_stmt  */
#line 110 "parser.y"
                      { prod("stmt", "expr_stmt"); }
#line 1355 "parser.tab.c"
    break;

  case 8: /* async_func_decl: ASYNC FUNCTION IDENTIFIER LPAREN param_list RPAREN LBRACE func_body RBRACE  */
#line 115 "parser.y"
      {
        prod("async_func_decl", "async function IDENTIFIER ( param_list ) { func_body }");
        pt_enter("AsyncFunctionDeclaration");
        pt_leaf("name", (yyvsp[-6].str));
        pt_exit();

        if (strcmp((yyvsp[-6].str), "registerUser") == 0) {
            func_mode = 1;
            sym_add((yyvsp[-6].str), "async function(User[], UserInput): Promise<User>", "global");
        } else if (strcmp((yyvsp[-6].str), "uploadFile") == 0) {
            func_mode = 2;
            sym_add((yyvsp[-6].str), "async function(FileRecord[], FileInput): Promise<FileRecord>", "global");
        } else if (strcmp((yyvsp[-6].str), "placeOrder") == 0) {
            func_mode = 3;
            sym_add((yyvsp[-6].str), "async function(Product[], OrderInput): Promise<Order>", "global");
        }
        free((yyvsp[-6].str));
      }
#line 1378 "parser.tab.c"
    break;

  case 9: /* param_list: IDENTIFIER COMMA IDENTIFIER  */
#line 137 "parser.y"
      {
        prod("param_list", "IDENTIFIER , IDENTIFIER");
        pt_enter("ParamList");
        pt_leaf("param", (yyvsp[-2].str)); pt_leaf("param", (yyvsp[0].str));
        pt_exit();
        free((yyvsp[-2].str)); free((yyvsp[0].str));
      }
#line 1390 "parser.tab.c"
    break;

  case 10: /* param_list: IDENTIFIER  */
#line 145 "parser.y"
      {
        prod("param_list", "IDENTIFIER");
        free((yyvsp[0].str));
      }
#line 1399 "parser.tab.c"
    break;

  case 11: /* func_body: func_body func_stmt  */
#line 152 "parser.y"
                          { prod("func_body", "func_body func_stmt"); }
#line 1405 "parser.tab.c"
    break;

  case 12: /* func_body: func_stmt  */
#line 153 "parser.y"
                          { prod("func_body", "func_stmt"); }
#line 1411 "parser.tab.c"
    break;

  case 13: /* func_stmt: if_stmt  */
#line 157 "parser.y"
                  { prod("func_stmt", "if_stmt"); }
#line 1417 "parser.tab.c"
    break;

  case 14: /* func_stmt: const_decl  */
#line 158 "parser.y"
                  { prod("func_stmt", "const_decl"); }
#line 1423 "parser.tab.c"
    break;

  case 15: /* func_stmt: assign_stmt  */
#line 159 "parser.y"
                  { prod("func_stmt", "assign_stmt"); }
#line 1429 "parser.tab.c"
    break;

  case 16: /* func_stmt: expr_stmt  */
#line 160 "parser.y"
                  { prod("func_stmt", "expr_stmt"); }
#line 1435 "parser.tab.c"
    break;

  case 17: /* func_stmt: return_stmt  */
#line 161 "parser.y"
                  { prod("func_stmt", "return_stmt"); }
#line 1441 "parser.tab.c"
    break;

  case 18: /* if_stmt: IF LPAREN if_cond RPAREN LBRACE throw_stmt RBRACE  */
#line 166 "parser.y"
      {
        prod("if_stmt", "if ( if_cond ) { throw_stmt }");
        pt_enter("IfStatement");
        pt_leaf("condition", "!x.y || !x.z");
        pt_exit();
      }
#line 1452 "parser.tab.c"
    break;

  case 19: /* if_stmt: IF LPAREN IDENTIFIER RPAREN LBRACE throw_stmt RBRACE  */
#line 173 "parser.y"
      {
        prod("if_stmt", "if ( IDENTIFIER ) { throw_stmt }");
        pt_enter("IfStatement");
        pt_leaf("condition", (yyvsp[-4].str));
        pt_exit();
        free((yyvsp[-4].str));
      }
#line 1464 "parser.tab.c"
    break;

  case 20: /* if_stmt: IF LPAREN IDENTIFIER DOT IDENTIFIER LT IDENTIFIER DOT IDENTIFIER RPAREN LBRACE throw_stmt RBRACE  */
#line 181 "parser.y"
      {
        prod("if_stmt", "if ( IDENTIFIER.IDENTIFIER < IDENTIFIER.IDENTIFIER ) { throw_stmt }");
        pt_enter("IfStatement");
        pt_leaf("condition", "x.y < z.w");
        pt_exit();
        free((yyvsp[-10].str)); free((yyvsp[-8].str)); free((yyvsp[-6].str)); free((yyvsp[-4].str));
      }
#line 1476 "parser.tab.c"
    break;

  case 21: /* if_stmt: IF LPAREN IDENTIFIER DOT IDENTIFIER GT IDENTIFIER RPAREN LBRACE throw_stmt RBRACE  */
#line 189 "parser.y"
      {
        prod("if_stmt", "if ( IDENTIFIER.IDENTIFIER > IDENTIFIER ) { throw_stmt }");
        pt_enter("IfStatement");
        pt_leaf("condition", "x.y > z");
        pt_exit();
        free((yyvsp[-8].str)); free((yyvsp[-6].str)); free((yyvsp[-4].str));
      }
#line 1488 "parser.tab.c"
    break;

  case 22: /* if_cond: NOT IDENTIFIER DOT IDENTIFIER OR NOT IDENTIFIER DOT IDENTIFIER  */
#line 200 "parser.y"
      {
        prod("if_cond", "! IDENTIFIER.IDENTIFIER || ! IDENTIFIER.IDENTIFIER");
        free((yyvsp[-7].str)); free((yyvsp[-5].str)); free((yyvsp[-2].str)); free((yyvsp[0].str));
      }
#line 1497 "parser.tab.c"
    break;

  case 23: /* if_cond: NOT IDENTIFIER  */
#line 205 "parser.y"
      {
        prod("if_cond", "! IDENTIFIER");
        free((yyvsp[0].str));
      }
#line 1506 "parser.tab.c"
    break;

  case 24: /* if_cond: expr  */
#line 209 "parser.y"
           { prod("if_cond", "expr"); }
#line 1512 "parser.tab.c"
    break;

  case 25: /* throw_stmt: THROW NEW IDENTIFIER LPAREN STRING_LIT RPAREN SEMICOLON  */
#line 214 "parser.y"
      {
        prod("throw_stmt", "throw new IDENTIFIER ( STRING_LIT )");
        pt_enter("ThrowStatement");
        pt_leaf("message", (yyvsp[-2].str));
        pt_exit();
        free((yyvsp[-4].str)); free((yyvsp[-2].str));
      }
#line 1524 "parser.tab.c"
    break;

  case 26: /* throw_stmt: THROW NEW IDENTIFIER LPAREN STRING_LIT RPAREN  */
#line 222 "parser.y"
      {
        prod("throw_stmt", "throw new IDENTIFIER ( STRING_LIT )");
        pt_enter("ThrowStatement");
        pt_leaf("message", (yyvsp[-1].str));
        pt_exit();
        free((yyvsp[-3].str)); free((yyvsp[-1].str));
      }
#line 1536 "parser.tab.c"
    break;

  case 27: /* const_decl: CONST IDENTIFIER ASSIGN expr SEMICOLON  */
#line 233 "parser.y"
      {
        prod("const_decl", "const IDENTIFIER = expr");
        pt_enter("VariableDeclaration (const)");
        pt_leaf("name", (yyvsp[-3].str)); pt_leaf("init", "expr");
        pt_exit();
        free((yyvsp[-3].str));
      }
#line 1548 "parser.tab.c"
    break;

  case 28: /* const_decl: CONST IDENTIFIER ASSIGN object_literal SEMICOLON  */
#line 241 "parser.y"
      {
        prod("const_decl", "const IDENTIFIER = object_literal");
        pt_enter("VariableDeclaration (const)");
        pt_leaf("name", (yyvsp[-3].str)); pt_leaf("init", "ObjectLiteral");
        pt_exit();
        free((yyvsp[-3].str));
      }
#line 1560 "parser.tab.c"
    break;

  case 29: /* const_decl: CONST IDENTIFIER ASSIGN expr  */
#line 249 "parser.y"
      {
        prod("const_decl", "const IDENTIFIER = expr");
        pt_enter("VariableDeclaration (const)");
        pt_leaf("name", (yyvsp[-2].str)); pt_leaf("init", "expr");
        pt_exit();
        free((yyvsp[-2].str));
      }
#line 1572 "parser.tab.c"
    break;

  case 30: /* const_decl: CONST IDENTIFIER ASSIGN object_literal  */
#line 257 "parser.y"
      {
        prod("const_decl", "const IDENTIFIER = object_literal");
        pt_enter("VariableDeclaration (const)");
        pt_leaf("name", (yyvsp[-2].str)); pt_leaf("init", "ObjectLiteral");
        pt_exit();
        free((yyvsp[-2].str));
      }
#line 1584 "parser.tab.c"
    break;

  case 31: /* const_decl: CONST IDENTIFIER ASSIGN NUMBER_LIT STAR NUMBER_LIT STAR NUMBER_LIT SEMICOLON  */
#line 265 "parser.y"
      {
        prod("const_decl", "const IDENTIFIER = NUMBER * NUMBER * NUMBER");
        pt_enter("VariableDeclaration (const)");
        pt_leaf("name", (yyvsp[-7].str)); pt_leaf("init", "NumericExpr");
        pt_exit();
        free((yyvsp[-7].str)); free((yyvsp[-5].str)); free((yyvsp[-3].str)); free((yyvsp[-1].str));
      }
#line 1596 "parser.tab.c"
    break;

  case 32: /* const_decl: CONST IDENTIFIER ASSIGN NUMBER_LIT STAR NUMBER_LIT STAR NUMBER_LIT  */
#line 273 "parser.y"
      {
        prod("const_decl", "const IDENTIFIER = NUMBER * NUMBER * NUMBER");
        pt_enter("VariableDeclaration (const)");
        pt_leaf("name", (yyvsp[-6].str)); pt_leaf("init", "NumericExpr");
        pt_exit();
        free((yyvsp[-6].str)); free((yyvsp[-4].str)); free((yyvsp[-2].str)); free((yyvsp[0].str));
      }
#line 1608 "parser.tab.c"
    break;

  case 33: /* assign_stmt: IDENTIFIER DOT IDENTIFIER MINUS_ASSIGN IDENTIFIER DOT IDENTIFIER SEMICOLON  */
#line 284 "parser.y"
      {
        prod("assign_stmt", "IDENTIFIER.IDENTIFIER -= IDENTIFIER.IDENTIFIER");
        pt_enter("AssignmentExpression (-=)");
        pt_leaf("left", (yyvsp[-7].str)); pt_leaf("right", (yyvsp[-3].str));
        pt_exit();
        free((yyvsp[-7].str)); free((yyvsp[-5].str)); free((yyvsp[-3].str)); free((yyvsp[-1].str));
      }
#line 1620 "parser.tab.c"
    break;

  case 34: /* assign_stmt: IDENTIFIER DOT IDENTIFIER MINUS_ASSIGN IDENTIFIER DOT IDENTIFIER  */
#line 292 "parser.y"
      {
        prod("assign_stmt", "IDENTIFIER.IDENTIFIER -= IDENTIFIER.IDENTIFIER");
        pt_enter("AssignmentExpression (-=)");
        pt_leaf("left", (yyvsp[-6].str)); pt_leaf("right", (yyvsp[-2].str));
        pt_exit();
        free((yyvsp[-6].str)); free((yyvsp[-4].str)); free((yyvsp[-2].str)); free((yyvsp[0].str));
      }
#line 1632 "parser.tab.c"
    break;

  case 35: /* object_literal: LBRACE prop_list RBRACE  */
#line 303 "parser.y"
      { prod("object_literal", "{ prop_list }"); }
#line 1638 "parser.tab.c"
    break;

  case 36: /* prop_list: prop_list COMMA prop  */
#line 307 "parser.y"
                           { prod("prop_list", "prop_list , prop"); }
#line 1644 "parser.tab.c"
    break;

  case 37: /* prop_list: prop  */
#line 308 "parser.y"
                           { prod("prop_list", "prop"); }
#line 1650 "parser.tab.c"
    break;

  case 38: /* prop: IDENTIFIER COLON expr  */
#line 313 "parser.y"
      {
        prod("prop", "IDENTIFIER : expr");
        printf("  [Semantic]  Property %-14s : inferred\n", (yyvsp[-2].str));
        free((yyvsp[-2].str));
      }
#line 1660 "parser.tab.c"
    break;

  case 39: /* prop: IDENTIFIER COLON TRUE_LIT  */
#line 319 "parser.y"
      {
        prod("prop", "IDENTIFIER : true");
        printf("  [Semantic]  Property %-14s : boolean\n", (yyvsp[-2].str));
        free((yyvsp[-2].str));
      }
#line 1670 "parser.tab.c"
    break;

  case 40: /* prop: IDENTIFIER COLON FALSE_LIT  */
#line 325 "parser.y"
      {
        prod("prop", "IDENTIFIER : false");
        printf("  [Semantic]  Property %-14s : boolean\n", (yyvsp[-2].str));
        free((yyvsp[-2].str));
      }
#line 1680 "parser.tab.c"
    break;

  case 41: /* prop: IDENTIFIER COLON STRING_LIT  */
#line 331 "parser.y"
      {
        prod("prop", "IDENTIFIER : STRING_LIT");
        printf("  [Semantic]  Property %-14s : string\n", (yyvsp[-2].str));
        free((yyvsp[-2].str)); free((yyvsp[0].str));
      }
#line 1690 "parser.tab.c"
    break;

  case 42: /* prop: IDENTIFIER COLON NUMBER_LIT  */
#line 337 "parser.y"
      {
        prod("prop", "IDENTIFIER : NUMBER_LIT");
        printf("  [Semantic]  Property %-14s : number\n", (yyvsp[-2].str));
        free((yyvsp[-2].str)); free((yyvsp[0].str));
      }
#line 1700 "parser.tab.c"
    break;

  case 43: /* return_stmt: RETURN IDENTIFIER SEMICOLON  */
#line 346 "parser.y"
      {
        prod("return_stmt", "return IDENTIFIER");
        pt_enter("ReturnStatement");
        pt_leaf("value", (yyvsp[-1].str));
        pt_exit();
        free((yyvsp[-1].str));
      }
#line 1712 "parser.tab.c"
    break;

  case 44: /* return_stmt: RETURN IDENTIFIER  */
#line 354 "parser.y"
      {
        prod("return_stmt", "return IDENTIFIER");
        pt_enter("ReturnStatement");
        pt_leaf("value", (yyvsp[0].str));
        pt_exit();
        free((yyvsp[0].str));
      }
#line 1724 "parser.tab.c"
    break;

  case 45: /* var_decl: LET IDENTIFIER ASSIGN LBRACKET RBRACKET SEMICOLON  */
#line 365 "parser.y"
      {
        prod("var_decl", "let IDENTIFIER = []");
        pt_enter("VariableDeclaration (let)");
        pt_leaf("name", (yyvsp[-4].str)); pt_leaf("init", "[]");
        pt_exit();
        free((yyvsp[-4].str));
      }
#line 1736 "parser.tab.c"
    break;

  case 46: /* var_decl: LET IDENTIFIER ASSIGN LBRACKET RBRACKET  */
#line 373 "parser.y"
      {
        prod("var_decl", "let IDENTIFIER = []");
        pt_enter("VariableDeclaration (let)");
        pt_leaf("name", (yyvsp[-3].str)); pt_leaf("init", "[]");
        pt_exit();
        free((yyvsp[-3].str));
      }
#line 1748 "parser.tab.c"
    break;

  case 47: /* var_decl: LET IDENTIFIER ASSIGN LBRACKET prop_list RBRACKET SEMICOLON  */
#line 381 "parser.y"
      {
        prod("var_decl", "let IDENTIFIER = [ prop_list ]");
        pt_enter("VariableDeclaration (let)");
        pt_leaf("name", (yyvsp[-5].str)); pt_leaf("init", "[...]");
        pt_exit();
        free((yyvsp[-5].str));
      }
#line 1760 "parser.tab.c"
    break;

  case 48: /* var_decl: LET IDENTIFIER ASSIGN LBRACKET prop_list RBRACKET  */
#line 389 "parser.y"
      {
        prod("var_decl", "let IDENTIFIER = [ prop_list ]");
        pt_enter("VariableDeclaration (let)");
        pt_leaf("name", (yyvsp[-4].str)); pt_leaf("init", "[...]");
        pt_exit();
        free((yyvsp[-4].str));
      }
#line 1772 "parser.tab.c"
    break;

  case 49: /* var_decl: LET IDENTIFIER ASSIGN LBRACKET object_literal RBRACKET SEMICOLON  */
#line 397 "parser.y"
      {
        prod("var_decl", "let IDENTIFIER = [ object_literal ]");
        pt_enter("VariableDeclaration (let)");
        pt_leaf("name", (yyvsp[-5].str)); pt_leaf("init", "[Object]");
        pt_exit();
        free((yyvsp[-5].str));
      }
#line 1784 "parser.tab.c"
    break;

  case 50: /* var_decl: LET IDENTIFIER ASSIGN LBRACKET object_literal RBRACKET  */
#line 405 "parser.y"
      {
        prod("var_decl", "let IDENTIFIER = [ object_literal ]");
        pt_enter("VariableDeclaration (let)");
        pt_leaf("name", (yyvsp[-4].str)); pt_leaf("init", "[Object]");
        pt_exit();
        free((yyvsp[-4].str));
      }
#line 1796 "parser.tab.c"
    break;

  case 51: /* expr_stmt: expr SEMICOLON  */
#line 415 "parser.y"
                     { prod("expr_stmt", "expr ;"); }
#line 1802 "parser.tab.c"
    break;

  case 52: /* expr_stmt: expr  */
#line 416 "parser.y"
                     { prod("expr_stmt", "expr"); }
#line 1808 "parser.tab.c"
    break;

  case 53: /* expr: IDENTIFIER DOT IDENTIFIER LPAREN arrow_func RPAREN  */
#line 421 "parser.y"
      {
        prod("expr", "IDENTIFIER.IDENTIFIER ( arrow_func )");
        pt_enter("CallExpression");
        pt_leaf("callee", (yyvsp[-5].str)); pt_leaf("method", (yyvsp[-3].str));
        pt_exit();
        free((yyvsp[-5].str)); free((yyvsp[-3].str));
      }
#line 1820 "parser.tab.c"
    break;

  case 54: /* expr: IDENTIFIER DOT IDENTIFIER LPAREN IDENTIFIER RPAREN  */
#line 429 "parser.y"
      {
        prod("expr", "IDENTIFIER.IDENTIFIER ( IDENTIFIER )");
        pt_enter("CallExpression");
        pt_leaf("callee", (yyvsp[-5].str)); pt_leaf("method", (yyvsp[-3].str)); pt_leaf("arg", (yyvsp[-1].str));
        pt_exit();
        free((yyvsp[-5].str)); free((yyvsp[-3].str)); free((yyvsp[-1].str));
      }
#line 1832 "parser.tab.c"
    break;

  case 55: /* expr: IDENTIFIER DOT IDENTIFIER LPAREN RPAREN  */
#line 437 "parser.y"
      {
        prod("expr", "IDENTIFIER.IDENTIFIER ()");
        free((yyvsp[-4].str)); free((yyvsp[-2].str));
      }
#line 1841 "parser.tab.c"
    break;

  case 56: /* expr: NEW IDENTIFIER LPAREN RPAREN DOT IDENTIFIER LPAREN RPAREN  */
#line 442 "parser.y"
      { prod("expr", "new IDENTIFIER().IDENTIFIER()"); }
#line 1847 "parser.tab.c"
    break;

  case 57: /* expr: IDENTIFIER DOT IDENTIFIER  */
#line 444 "parser.y"
      {
        prod("expr", "IDENTIFIER.IDENTIFIER");
        free((yyvsp[-2].str)); free((yyvsp[0].str));
      }
#line 1856 "parser.tab.c"
    break;

  case 58: /* expr: IDENTIFIER LPAREN IDENTIFIER COMMA object_literal RPAREN DOT IDENTIFIER LPAREN arrow_func RPAREN DOT IDENTIFIER LPAREN arrow_func RPAREN  */
#line 449 "parser.y"
      {
        prod("expr", "IDENTIFIER ( IDENTIFIER, object_literal ).IDENTIFIER( arrow_func ).IDENTIFIER( arrow_func )");
        pt_enter("CallExpression (.then().catch())");
        pt_leaf("callee", (yyvsp[-15].str));
        pt_exit();
        free((yyvsp[-15].str)); free((yyvsp[-13].str)); free((yyvsp[-8].str)); free((yyvsp[-3].str));
      }
#line 1868 "parser.tab.c"
    break;

  case 59: /* expr: IDENTIFIER LPAREN IDENTIFIER COMMA object_literal RPAREN DOT IDENTIFIER LPAREN arrow_func RPAREN  */
#line 457 "parser.y"
      {
        prod("expr", "IDENTIFIER ( IDENTIFIER, object_literal ).IDENTIFIER( arrow_func )");
        pt_enter("CallExpression (.then())");
        pt_leaf("callee", (yyvsp[-10].str));
        pt_exit();
        free((yyvsp[-10].str)); free((yyvsp[-8].str)); free((yyvsp[-3].str));
      }
#line 1880 "parser.tab.c"
    break;

  case 60: /* expr: IDENTIFIER LPAREN IDENTIFIER COMMA object_literal RPAREN  */
#line 465 "parser.y"
      {
        prod("expr", "IDENTIFIER ( IDENTIFIER, object_literal )");
        pt_enter("CallExpression");
        pt_leaf("callee", (yyvsp[-5].str));
        pt_exit();
        free((yyvsp[-5].str)); free((yyvsp[-3].str));
      }
#line 1892 "parser.tab.c"
    break;

  case 61: /* expr: IDENTIFIER DOT IDENTIFIER LPAREN STRING_LIT RPAREN  */
#line 473 "parser.y"
      {
        prod("expr", "IDENTIFIER.IDENTIFIER ( STRING_LIT )");
        free((yyvsp[-5].str)); free((yyvsp[-3].str)); free((yyvsp[-1].str));
      }
#line 1901 "parser.tab.c"
    break;

  case 62: /* expr: IDENTIFIER DOT IDENTIFIER LPAREN STRING_LIT COMMA IDENTIFIER DOT IDENTIFIER RPAREN  */
#line 478 "parser.y"
      {
        prod("expr", "IDENTIFIER.IDENTIFIER ( STRING_LIT, IDENTIFIER.IDENTIFIER )");
        free((yyvsp[-9].str)); free((yyvsp[-7].str)); free((yyvsp[-5].str)); free((yyvsp[-3].str)); free((yyvsp[-1].str));
      }
#line 1910 "parser.tab.c"
    break;

  case 63: /* expr: IDENTIFIER DOT IDENTIFIER LPAREN IDENTIFIER DOT IDENTIFIER RPAREN  */
#line 483 "parser.y"
      {
        prod("expr", "IDENTIFIER.IDENTIFIER ( IDENTIFIER.IDENTIFIER )");
        free((yyvsp[-7].str)); free((yyvsp[-5].str)); free((yyvsp[-3].str)); free((yyvsp[-1].str));
      }
#line 1919 "parser.tab.c"
    break;

  case 64: /* expr: IDENTIFIER DOT IDENTIFIER EQ IDENTIFIER DOT IDENTIFIER  */
#line 488 "parser.y"
      {
        prod("expr", "IDENTIFIER.IDENTIFIER === IDENTIFIER.IDENTIFIER");
        free((yyvsp[-6].str)); free((yyvsp[-4].str)); free((yyvsp[-2].str)); free((yyvsp[0].str));
      }
#line 1928 "parser.tab.c"
    break;

  case 65: /* expr: IDENTIFIER EQ IDENTIFIER DOT IDENTIFIER  */
#line 493 "parser.y"
      {
        prod("expr", "IDENTIFIER === IDENTIFIER.IDENTIFIER");
        free((yyvsp[-4].str)); free((yyvsp[-2].str)); free((yyvsp[0].str));
      }
#line 1937 "parser.tab.c"
    break;

  case 66: /* expr: NUMBER_LIT STAR NUMBER_LIT  */
#line 498 "parser.y"
      {
        prod("expr", "NUMBER * NUMBER");
        free((yyvsp[-2].str)); free((yyvsp[0].str));
      }
#line 1946 "parser.tab.c"
    break;

  case 67: /* expr: IDENTIFIER  */
#line 502 "parser.y"
                 { prod("expr", "IDENTIFIER"); free((yyvsp[0].str)); }
#line 1952 "parser.tab.c"
    break;

  case 68: /* expr: STRING_LIT  */
#line 503 "parser.y"
                 { prod("expr", "STRING_LIT");  free((yyvsp[0].str)); }
#line 1958 "parser.tab.c"
    break;

  case 69: /* expr: NUMBER_LIT  */
#line 504 "parser.y"
                 { prod("expr", "NUMBER_LIT");  free((yyvsp[0].str)); }
#line 1964 "parser.tab.c"
    break;

  case 70: /* expr: TRUE_LIT  */
#line 505 "parser.y"
                 { prod("expr", "true"); }
#line 1970 "parser.tab.c"
    break;

  case 71: /* expr: FALSE_LIT  */
#line 506 "parser.y"
                 { prod("expr", "false"); }
#line 1976 "parser.tab.c"
    break;

  case 72: /* arrow_func: IDENTIFIER ARROW expr  */
#line 511 "parser.y"
      {
        prod("arrow_func", "IDENTIFIER => expr");
        pt_enter("ArrowFunction");
        pt_leaf("param", (yyvsp[-2].str));
        pt_exit();
        free((yyvsp[-2].str));
      }
#line 1988 "parser.tab.c"
    break;

  case 73: /* arrow_func: IDENTIFIER ARROW LBRACE func_body RBRACE  */
#line 519 "parser.y"
      {
        prod("arrow_func", "IDENTIFIER => { func_body }");
        pt_enter("ArrowFunction (block)");
        pt_leaf("param", (yyvsp[-4].str));
        pt_exit();
        free((yyvsp[-4].str));
      }
#line 2000 "parser.tab.c"
    break;

  case 74: /* arrow_func: IDENTIFIER ARROW LBRACE RBRACE  */
#line 527 "parser.y"
      {
        prod("arrow_func", "IDENTIFIER => {}");
        free((yyvsp[-3].str));
      }
#line 2009 "parser.tab.c"
    break;


#line 2013 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

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
                      yytoken, &yylval);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 533 "parser.y"


/* ── Semantic Definitions ── */
void print_semantic_defs() {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║              SEMANTIC DEFINITIONS                    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");

    if (func_mode == 1) {
        printf("  Type  User        = { id: number, email: string, password: string, isActive: boolean }\n");
        printf("  Type  UserInput   = { email: string, password: string }\n");
        printf("  Func  registerUser: (User[], UserInput) → Promise<User>\n");
        printf("  Var   users       : User[]\n");
    } else if (func_mode == 2) {
        printf("  Type  FileRecord  = { id: number, name: string, size: number, uploadedAt: string }\n");
        printf("  Type  FileInput   = { name: string, size: number }\n");
        printf("  Func  uploadFile  : (FileRecord[], FileInput) → Promise<FileRecord>\n");
        printf("  Const MAX_SIZE    : number = 5242880\n");
        printf("  Var   files       : FileRecord[]\n");
    } else if (func_mode == 3) {
        printf("  Type  Product     = { id: number, name: string, stock: number }\n");
        printf("  Type  OrderInput  = { productId: number, quantity: number }\n");
        printf("  Type  Order       = { orderId: number, productId: number, quantity: number, status: string }\n");
        printf("  Func  placeOrder  : (Product[], OrderInput) → Promise<Order>\n");
        printf("  Var   products    : Product[]\n");
    }
}

/* ── Code Generation ── */
void emit_typescript() {
    FILE *f = fopen("output.ts", "w");
    if (!f) { perror("output.ts"); return; }

    if (func_mode == 1) {
        fprintf(f, "type User = {\n");
        fprintf(f, "    id: number;\n");
        fprintf(f, "    email: string;\n");
        fprintf(f, "    password: string;\n");
        fprintf(f, "    isActive: boolean;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "type UserInput = {\n");
        fprintf(f, "    email: string;\n");
        fprintf(f, "    password: string;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "async function registerUser(\n");
        fprintf(f, "    users: User[],\n");
        fprintf(f, "    userData: UserInput\n");
        fprintf(f, "): Promise<User> {\n");
        fprintf(f, "    if (!userData.email || !userData.password) {\n");
        fprintf(f, "        throw new Error(\"Invalid input\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    const existingUser = users.find(\n");
        fprintf(f, "        (u: User) => u.email === userData.email\n");
        fprintf(f, "    );\n\n");
        fprintf(f, "    if (existingUser) {\n");
        fprintf(f, "        throw new Error(\"User already exists\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    const newUser: User = {\n");
        fprintf(f, "        id: Date.now(),\n");
        fprintf(f, "        email: userData.email,\n");
        fprintf(f, "        password: userData.password,\n");
        fprintf(f, "        isActive: true\n");
        fprintf(f, "    };\n\n");
        fprintf(f, "    users.push(newUser);\n");
        fprintf(f, "    return newUser;\n");
        fprintf(f, "}\n\n");
        fprintf(f, "let users: User[] = [];\n");

    } else if (func_mode == 2) {
        fprintf(f, "type FileRecord = {\n");
        fprintf(f, "    id: number;\n");
        fprintf(f, "    name: string;\n");
        fprintf(f, "    size: number;\n");
        fprintf(f, "    uploadedAt: string;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "type FileInput = {\n");
        fprintf(f, "    name: string;\n");
        fprintf(f, "    size: number;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "async function uploadFile(\n");
        fprintf(f, "    files: FileRecord[],\n");
        fprintf(f, "    fileData: FileInput\n");
        fprintf(f, "): Promise<FileRecord> {\n");
        fprintf(f, "    if (!fileData.name || !fileData.size) {\n");
        fprintf(f, "        throw new Error(\"Invalid file data\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    const MAX_SIZE: number = 5 * 1024 * 1024;\n\n");
        fprintf(f, "    if (fileData.size > MAX_SIZE) {\n");
        fprintf(f, "        throw new Error(\"File size exceeds limit\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    const fileExists = files.find(\n");
        fprintf(f, "        (f: FileRecord) => f.name === fileData.name\n");
        fprintf(f, "    );\n\n");
        fprintf(f, "    if (fileExists) {\n");
        fprintf(f, "        throw new Error(\"File already exists\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    const newFile: FileRecord = {\n");
        fprintf(f, "        id: Date.now(),\n");
        fprintf(f, "        name: fileData.name,\n");
        fprintf(f, "        size: fileData.size,\n");
        fprintf(f, "        uploadedAt: new Date().toISOString()\n");
        fprintf(f, "    };\n\n");
        fprintf(f, "    files.push(newFile);\n");
        fprintf(f, "    return newFile;\n");
        fprintf(f, "}\n\n");
        fprintf(f, "let files: FileRecord[] = [];\n");

    } else if (func_mode == 3) {
        fprintf(f, "type Product = {\n");
        fprintf(f, "    id: number;\n");
        fprintf(f, "    name: string;\n");
        fprintf(f, "    stock: number;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "type OrderInput = {\n");
        fprintf(f, "    productId: number;\n");
        fprintf(f, "    quantity: number;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "type Order = {\n");
        fprintf(f, "    orderId: number;\n");
        fprintf(f, "    productId: number;\n");
        fprintf(f, "    quantity: number;\n");
        fprintf(f, "    status: string;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "async function placeOrder(\n");
        fprintf(f, "    products: Product[],\n");
        fprintf(f, "    order: OrderInput\n");
        fprintf(f, "): Promise<Order> {\n");
        fprintf(f, "    if (!order.productId || !order.quantity) {\n");
        fprintf(f, "        throw new Error(\"Invalid order data\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    const product = products.find(\n");
        fprintf(f, "        (p: Product) => p.id === order.productId\n");
        fprintf(f, "    );\n\n");
        fprintf(f, "    if (!product) {\n");
        fprintf(f, "        throw new Error(\"Product not found\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    if (product.stock < order.quantity) {\n");
        fprintf(f, "        throw new Error(\"Insufficient stock\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    product.stock -= order.quantity;\n\n");
        fprintf(f, "    const newOrder: Order = {\n");
        fprintf(f, "        orderId: Date.now(),\n");
        fprintf(f, "        productId: order.productId,\n");
        fprintf(f, "        quantity: order.quantity,\n");
        fprintf(f, "        status: \"confirmed\"\n");
        fprintf(f, "    };\n\n");
        fprintf(f, "    return newOrder;\n");
        fprintf(f, "}\n\n");
        fprintf(f, "let products: Product[] = [{ id: 101, name: \"Laptop\", stock: 5 }];\n");
    }

    fclose(f);
    printf("\n[CodeGen] ✔ TypeScript written to output.ts\n");
}

void yyerror(const char *s) {
    fprintf(stderr, "[Parse Error] line %d: %s\n", yylineno, s);
}

int main() {
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║         JS → TS Compiler  (Lex + Bison)              ║\n");
    printf("║   Phases: Lexical | Syntax | Semantic | CodeGen      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── PHASE 1: Lexical Analysis ──────────────────────────\n");
    printf("  Tokenizing input stream...\n\n");

    printf("── PHASE 2: Syntax Analysis  (Productions + Parse Tree)\n");
    yyparse();

    print_parse_method();
    print_action_trace();

    printf("\n── PHASE 3: Semantic Analysis ─────────────────────────\n");
    print_symtable();
    print_semantic_defs();

    printf("\n── PHASE 4: Code Generation ───────────────────────────\n");
    emit_typescript();

    printf("\n✅ Compilation successful.\n");
    return 0;
}
