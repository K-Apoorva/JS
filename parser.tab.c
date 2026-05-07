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
#include "ast.h"

void yyerror(const char *s);
int  yylex(void);
extern int yylineno;

static Node *root = NULL;

/* ── Type inference structures ── */
#define MAX_PROPS 32
#define MAX_TYPES 16
#define MAX_VARS  64

typedef struct { char key[64]; char type[32]; } Prop;
typedef struct { char name[64]; Prop props[MAX_PROPS]; int nprops; } TypeDef;
typedef struct { char name[64]; char type[64]; char scope[32]; } VarEntry;

static TypeDef  types[MAX_TYPES]; static int ntype = 0;
static VarEntry vars[MAX_VARS];   static int nvar  = 0;

/* forward declarations */
static void   infer_program(Node *n);
static void   emit_program(Node *n, FILE *f);
static char  *infer_expr_type(Node *n);
static void   emit_expr(Node *n, FILE *f);
static void   emit_stmt(Node *n, FILE *f, int ind);
static void   emit_block_stmts(NodeList *l, FILE *f, int ind);
static void   indent_f(FILE *f, int d);

static void var_set(const char *name, const char *type, const char *scope) {
    for (int i = 0; i < nvar; i++)
        if (strcmp(vars[i].name, name) == 0 &&
            strcmp(vars[i].scope, scope) == 0) {
            strncpy(vars[i].type, type, 63); return;
        }
    strncpy(vars[nvar].name,  name,  63);
    strncpy(vars[nvar].type,  type,  63);
    strncpy(vars[nvar].scope, scope, 31);
    nvar++;
}

static const char *var_type_in(const char *name, const char *scope) {
    for (int i = 0; i < nvar; i++)
        if (strcmp(vars[i].name, name) == 0 &&
            strcmp(vars[i].scope, scope) == 0) return vars[i].type;
    return "unknown";
}

static const char *var_type(const char *name) {
    for (int i = 0; i < nvar; i++)
        if (strcmp(vars[i].name, name) == 0) return vars[i].type;
    return "unknown";
}

static TypeDef *type_find(const char *name) {
    for (int i = 0; i < ntype; i++)
        if (strcmp(types[i].name, name) == 0) return &types[i];
    return NULL;
}

static TypeDef *type_new(const char *name) {
    if (type_find(name)) return type_find(name);
    strncpy(types[ntype].name, name, 63);
    types[ntype].nprops = 0;
    return &types[ntype++];
}

static void type_add_prop(TypeDef *t, const char *key, const char *ptype) {
    for (int i = 0; i < t->nprops; i++)
        if (strcmp(t->props[i].key, key) == 0) return;
    strncpy(t->props[t->nprops].key,  key,   63);
    strncpy(t->props[t->nprops].type, ptype, 31);
    t->nprops++;
}

/* infer type of a single property value node */
static const char *prop_val_type(Node *v) {
    if (!v) return "unknown";
    if (v->kind == N_BOOL)   return "boolean";
    if (v->kind == N_STR)    return "string";
    if (v->kind == N_NUM)    return "number";
    if (v->kind == N_NULL)   return "null";
    /* Date.now() → number */
    if (v->kind == N_CALL && v->left && v->left->kind == N_MEMBER) {
        Node *m = v->left;
        /* new Date().toISOString() → string */
        if (m->left && m->left->kind == N_NEW) return "string";
        if (m->left && m->left->kind == N_IDENT &&
            strcmp(m->left->sval,"Date")==0 &&
            m->right && strcmp(m->right->sval,"now")==0)
            return "number";
        /* any .toString(), .toISOString(), etc. → string */
        if (m->right && m->right->kind == N_IDENT) {
            const char *mn = m->right->sval;
            if (strstr(mn,"String")!=NULL || strstr(mn,"string")!=NULL ||
                strstr(mn,"ISO")!=NULL || strstr(mn,"Format")!=NULL)
                return "string";
        }
    }
    /* arithmetic → number */
    if (v->kind == N_BINARY) return "number";
    /* member access: param.prop — look up param type */
    if (v->kind == N_MEMBER && v->left && v->left->kind == N_IDENT) {
        const char *pt = var_type(v->left->sval);
        if (strcmp(pt,"unknown") != 0) return pt;
    }
    if (v->kind == N_IDENT) return var_type(v->sval);
    return "unknown";
}


#line 187 "parser.tab.c"

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
  YYSYMBOL_ELSE = 8,                       /* ELSE  */
  YYSYMBOL_RETURN = 9,                     /* RETURN  */
  YYSYMBOL_THROW = 10,                     /* THROW  */
  YYSYMBOL_NEW = 11,                       /* NEW  */
  YYSYMBOL_TRUE_LIT = 12,                  /* TRUE_LIT  */
  YYSYMBOL_FALSE_LIT = 13,                 /* FALSE_LIT  */
  YYSYMBOL_NULL_LIT = 14,                  /* NULL_LIT  */
  YYSYMBOL_ARROW = 15,                     /* ARROW  */
  YYSYMBOL_OR = 16,                        /* OR  */
  YYSYMBOL_AND = 17,                       /* AND  */
  YYSYMBOL_NOT = 18,                       /* NOT  */
  YYSYMBOL_ASSIGN = 19,                    /* ASSIGN  */
  YYSYMBOL_SEMICOLON = 20,                 /* SEMICOLON  */
  YYSYMBOL_COLON = 21,                     /* COLON  */
  YYSYMBOL_COMMA = 22,                     /* COMMA  */
  YYSYMBOL_DOT = 23,                       /* DOT  */
  YYSYMBOL_LPAREN = 24,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 25,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 26,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 27,                    /* RBRACE  */
  YYSYMBOL_LBRACKET = 28,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 29,                  /* RBRACKET  */
  YYSYMBOL_IDENTIFIER = 30,                /* IDENTIFIER  */
  YYSYMBOL_STRING_LIT = 31,                /* STRING_LIT  */
  YYSYMBOL_NUMBER_LIT = 32,                /* NUMBER_LIT  */
  YYSYMBOL_EQ = 33,                        /* EQ  */
  YYSYMBOL_NEQ = 34,                       /* NEQ  */
  YYSYMBOL_LT = 35,                        /* LT  */
  YYSYMBOL_GT = 36,                        /* GT  */
  YYSYMBOL_LE = 37,                        /* LE  */
  YYSYMBOL_GE = 38,                        /* GE  */
  YYSYMBOL_ADDOP = 39,                     /* ADDOP  */
  YYSYMBOL_MULOP = 40,                     /* MULOP  */
  YYSYMBOL_ASSIGN_OP = 41,                 /* ASSIGN_OP  */
  YYSYMBOL_LOWER_THAN_ELSE = 42,           /* LOWER_THAN_ELSE  */
  YYSYMBOL_YYACCEPT = 43,                  /* $accept  */
  YYSYMBOL_program = 44,                   /* program  */
  YYSYMBOL_stmt_list = 45,                 /* stmt_list  */
  YYSYMBOL_stmt = 46,                      /* stmt  */
  YYSYMBOL_func_decl = 47,                 /* func_decl  */
  YYSYMBOL_param_list = 48,                /* param_list  */
  YYSYMBOL_block = 49,                     /* block  */
  YYSYMBOL_var_decl = 50,                  /* var_decl  */
  YYSYMBOL_if_stmt = 51,                   /* if_stmt  */
  YYSYMBOL_throw_stmt = 52,                /* throw_stmt  */
  YYSYMBOL_return_stmt = 53,               /* return_stmt  */
  YYSYMBOL_expr_stmt = 54,                 /* expr_stmt  */
  YYSYMBOL_expr = 55,                      /* expr  */
  YYSYMBOL_assign_expr = 56,               /* assign_expr  */
  YYSYMBOL_or_expr = 57,                   /* or_expr  */
  YYSYMBOL_and_expr = 58,                  /* and_expr  */
  YYSYMBOL_eq_expr = 59,                   /* eq_expr  */
  YYSYMBOL_rel_expr = 60,                  /* rel_expr  */
  YYSYMBOL_add_expr = 61,                  /* add_expr  */
  YYSYMBOL_mul_expr = 62,                  /* mul_expr  */
  YYSYMBOL_unary_expr = 63,                /* unary_expr  */
  YYSYMBOL_postfix_expr = 64,              /* postfix_expr  */
  YYSYMBOL_primary_expr = 65,              /* primary_expr  */
  YYSYMBOL_arrow_func = 66,                /* arrow_func  */
  YYSYMBOL_object_literal = 67,            /* object_literal  */
  YYSYMBOL_prop_list = 68,                 /* prop_list  */
  YYSYMBOL_prop = 69,                      /* prop  */
  YYSYMBOL_array_literal = 70,             /* array_literal  */
  YYSYMBOL_elem_list = 71,                 /* elem_list  */
  YYSYMBOL_arg_list = 72                   /* arg_list  */
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
#define YYFINAL  68
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   275

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  30
/* YYNRULES -- Number of rules.  */
#define YYNRULES  85
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  157

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   297


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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   150,   150,   154,   155,   159,   160,   161,   162,   163,
     164,   165,   169,   177,   188,   189,   190,   194,   195,   199,
     201,   203,   205,   207,   212,   214,   219,   220,   224,   225,
     226,   230,   231,   236,   239,   241,   243,   247,   249,   253,
     255,   259,   261,   263,   267,   269,   271,   273,   275,   279,
     281,   285,   287,   291,   293,   297,   307,   313,   319,   323,
     324,   325,   326,   327,   328,   329,   331,   332,   333,   334,
     338,   340,   342,   349,   350,   354,   355,   359,   361,   366,
     367,   371,   372,   376,   377,   378
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
  "CONST", "LET", "IF", "ELSE", "RETURN", "THROW", "NEW", "TRUE_LIT",
  "FALSE_LIT", "NULL_LIT", "ARROW", "OR", "AND", "NOT", "ASSIGN",
  "SEMICOLON", "COLON", "COMMA", "DOT", "LPAREN", "RPAREN", "LBRACE",
  "RBRACE", "LBRACKET", "RBRACKET", "IDENTIFIER", "STRING_LIT",
  "NUMBER_LIT", "EQ", "NEQ", "LT", "GT", "LE", "GE", "ADDOP", "MULOP",
  "ASSIGN_OP", "LOWER_THAN_ELSE", "$accept", "program", "stmt_list",
  "stmt", "func_decl", "param_list", "block", "var_decl", "if_stmt",
  "throw_stmt", "return_stmt", "expr_stmt", "expr", "assign_expr",
  "or_expr", "and_expr", "eq_expr", "rel_expr", "add_expr", "mul_expr",
  "unary_expr", "postfix_expr", "primary_expr", "arrow_func",
  "object_literal", "prop_list", "prop", "array_literal", "elem_list",
  "arg_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-91)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-16)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     172,     9,    -9,    14,    18,     1,   181,   212,    20,   -91,
     -91,   -91,   212,   234,    25,   203,    43,   -91,   -91,    62,
     172,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,    60,
      71,    99,    37,    38,    84,    85,   -91,    -1,   -91,   -91,
     -91,   -91,   107,   117,   123,    59,   212,   -91,    33,   130,
     -91,   137,   119,   -91,    75,    -6,   -10,   133,   -91,    -4,
     138,   108,    32,   -91,   -91,   -91,     4,   243,   -91,   -91,
     -91,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   131,   212,   212,   139,   132,   212,   212,   -91,
     140,   -91,   143,   138,   -91,   -91,   212,   141,   152,   -91,
     212,   212,   -91,    98,   -91,   212,   -91,   -91,   -91,    99,
      37,    38,    38,    84,    84,    84,    84,    85,   -91,   -91,
     156,   -91,     2,   -91,   132,   -91,    44,   167,   168,   163,
      63,   -91,   163,   -91,   -91,   -91,   -91,   212,   212,   -91,
      72,   163,   -91,   -91,   142,   183,   -91,   -91,   102,   -91,
     163,   -91,   -91,   163,   -91,   -91,   -91
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,    62,
      63,    64,     0,    16,     0,     0,    59,    60,    61,     0,
       2,     4,     5,    11,     6,     7,     8,     9,    10,    32,
      36,    38,    40,    43,    48,    50,    52,    54,    58,    68,
      66,    67,     0,     0,     0,     0,     0,    30,     0,    29,
      33,    27,     0,    53,    54,    59,     0,     0,    18,    59,
      60,     0,     0,    76,    80,    82,     0,     0,     1,     3,
      31,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    85,     0,     0,    16,     0,     0,    23,
       0,    74,     0,     0,    28,    26,    85,     0,     0,    69,
       0,     0,    17,     0,    73,     0,    79,    71,    70,    37,
      39,    41,    42,    44,    45,    46,    47,    49,    51,    35,
      56,    84,     0,    34,    16,    15,     0,    20,    22,     0,
       0,    14,     0,    77,    78,    75,    81,    85,     0,    57,
       0,     0,    19,    21,     0,    25,    65,    72,     0,    83,
       0,    13,    18,     0,    55,    12,    24
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -91,   -91,   197,   -15,   -91,   -79,   -64,   -91,   -91,   -91,
     -91,   -91,    -5,     0,   -91,   135,   136,    57,    15,   149,
      -8,    30,   -91,   -91,   -91,   -91,   115,   -91,   -91,   -90
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,    19,    61,    21,    22,    56,    23,    24,    25,    26,
      27,    28,   121,    50,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    62,    63,    41,    66,   122
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      29,    49,    51,   107,    53,    69,   130,   126,    57,    67,
      65,    67,    97,    42,    29,    98,   -15,   100,    81,   -15,
      29,    43,    82,    83,   138,    46,   105,   139,     1,     2,
       3,     4,     5,   106,     6,     7,     8,     9,    10,    11,
      84,    90,    54,    12,    44,   140,    69,   148,    45,    13,
      52,    14,    58,    15,   103,    59,    60,    18,    67,   104,
      91,    29,    68,    92,    93,   145,    97,   108,   147,   141,
      73,    74,   118,    75,    76,    77,    78,   151,    88,    89,
      70,   119,   127,   128,   123,   138,   155,    71,   146,   156,
     113,   114,   115,   116,    97,   133,   134,   150,    82,    83,
     136,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      54,     1,     2,     3,     4,     5,    72,     6,     7,     8,
       9,    10,    11,    79,   138,    80,    12,   154,    92,    93,
     111,   112,    13,   149,    14,   102,    15,    85,    16,    17,
      18,    86,    87,    96,    29,     1,     2,     3,     4,     5,
      94,     6,     7,     8,     9,    10,    11,    95,    99,   101,
      12,   120,   125,   124,   100,   129,    13,   132,    14,   152,
      15,   131,    16,    17,    18,     1,     2,     3,     4,     5,
     137,     6,     7,     8,     9,    10,    11,   142,   143,   144,
      12,   153,     8,     9,    10,    11,    13,    20,    14,    12,
      15,    47,    16,    17,    18,    13,   109,    48,   110,    15,
       0,    16,    17,    18,     8,     9,    10,    11,   135,     0,
       0,    12,     0,     8,     9,    10,    11,    13,   117,    48,
      12,    15,    64,    16,    17,    18,    13,     0,    48,     0,
      15,     0,    16,    17,    18,     8,     9,    10,    11,     0,
       0,     0,    12,     0,     8,     9,    10,    11,    13,     0,
      48,    12,    15,     0,    55,    17,    18,    13,     0,    14,
       0,    15,     0,    16,    17,    18
};

static const yytype_int16 yycheck[] =
{
       0,     6,     7,    67,    12,    20,    96,    86,    13,    15,
      15,    15,    22,     4,    14,    25,    22,    21,    19,    25,
      20,    30,    23,    24,    22,    24,    22,    25,     3,     4,
       5,     6,     7,    29,     9,    10,    11,    12,    13,    14,
      41,    46,    12,    18,    30,   124,    61,   137,    30,    24,
      30,    26,    27,    28,    22,    30,    31,    32,    15,    27,
      27,    61,     0,    30,    31,   129,    22,    67,   132,    25,
      33,    34,    80,    35,    36,    37,    38,   141,    19,    20,
      20,    81,    87,    88,    84,    22,   150,    16,    25,   153,
      75,    76,    77,    78,    22,   100,   101,    25,    23,    24,
     105,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,     3,     4,     5,     6,     7,    17,     9,    10,    11,
      12,    13,    14,    39,    22,    40,    18,    25,    30,    31,
      73,    74,    24,   138,    26,    27,    28,    30,    30,    31,
      32,    24,    19,    24,   144,     3,     4,     5,     6,     7,
      20,     9,    10,    11,    12,    13,    14,    20,    25,    21,
      18,    30,    30,    24,    21,    25,    24,    15,    26,    27,
      28,    30,    30,    31,    32,     3,     4,     5,     6,     7,
      24,     9,    10,    11,    12,    13,    14,    20,    20,    26,
      18,     8,    11,    12,    13,    14,    24,     0,    26,    18,
      28,    20,    30,    31,    32,    24,    71,    26,    72,    28,
      -1,    30,    31,    32,    11,    12,    13,    14,   103,    -1,
      -1,    18,    -1,    11,    12,    13,    14,    24,    79,    26,
      18,    28,    29,    30,    31,    32,    24,    -1,    26,    -1,
      28,    -1,    30,    31,    32,    11,    12,    13,    14,    -1,
      -1,    -1,    18,    -1,    11,    12,    13,    14,    24,    -1,
      26,    18,    28,    -1,    30,    31,    32,    24,    -1,    26,
      -1,    28,    -1,    30,    31,    32
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     9,    10,    11,    12,
      13,    14,    18,    24,    26,    28,    30,    31,    32,    44,
      45,    46,    47,    49,    50,    51,    52,    53,    54,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    70,     4,    30,    30,    30,    24,    20,    26,    55,
      56,    55,    30,    63,    64,    30,    48,    55,    27,    30,
      31,    45,    68,    69,    29,    55,    71,    15,     0,    46,
      20,    16,    17,    33,    34,    35,    36,    37,    38,    39,
      40,    19,    23,    24,    41,    30,    24,    19,    19,    20,
      55,    27,    30,    31,    20,    20,    24,    22,    25,    25,
      21,    21,    27,    22,    27,    22,    29,    49,    56,    58,
      59,    60,    60,    61,    61,    61,    61,    62,    63,    56,
      30,    55,    72,    56,    24,    30,    48,    55,    55,    25,
      72,    30,    15,    55,    55,    69,    55,    24,    22,    25,
      48,    25,    20,    20,    26,    49,    25,    49,    72,    55,
      25,    49,    27,     8,    25,    49,    49
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    43,    44,    45,    45,    46,    46,    46,    46,    46,
      46,    46,    47,    47,    48,    48,    48,    49,    49,    50,
      50,    50,    50,    50,    51,    51,    52,    52,    53,    53,
      53,    54,    54,    55,    56,    56,    56,    57,    57,    58,
      58,    59,    59,    59,    60,    60,    60,    60,    60,    61,
      61,    62,    62,    63,    63,    64,    64,    64,    64,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      66,    66,    66,    67,    67,    68,    68,    69,    69,    70,
      70,    71,    71,    72,    72,    72
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     7,     6,     3,     1,     0,     3,     2,     5,
       4,     5,     4,     3,     7,     5,     3,     2,     3,     2,
       2,     2,     1,     1,     3,     3,     1,     3,     1,     3,
       1,     3,     3,     1,     3,     3,     3,     3,     1,     3,
       1,     3,     1,     2,     1,     6,     3,     4,     1,     1,
       1,     1,     1,     1,     1,     5,     1,     1,     1,     3,
       3,     3,     5,     3,     2,     3,     1,     3,     3,     3,
       2,     3,     1,     3,     1,     0
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
#line 150 "parser.y"
                 { root = make_node(N_PROGRAM); root->list = (yyvsp[0].list); (yyval.node) = root; }
#line 1374 "parser.tab.c"
    break;

  case 3: /* stmt_list: stmt_list stmt  */
#line 154 "parser.y"
                      { (yyval.list) = nl_append((yyvsp[-1].list), (yyvsp[0].node)); }
#line 1380 "parser.tab.c"
    break;

  case 4: /* stmt_list: stmt  */
#line 155 "parser.y"
                      { (yyval.list) = nl_append(NULL, (yyvsp[0].node)); }
#line 1386 "parser.tab.c"
    break;

  case 5: /* stmt: func_decl  */
#line 159 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1392 "parser.tab.c"
    break;

  case 6: /* stmt: var_decl  */
#line 160 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1398 "parser.tab.c"
    break;

  case 7: /* stmt: if_stmt  */
#line 161 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1404 "parser.tab.c"
    break;

  case 8: /* stmt: throw_stmt  */
#line 162 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1410 "parser.tab.c"
    break;

  case 9: /* stmt: return_stmt  */
#line 163 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1416 "parser.tab.c"
    break;

  case 10: /* stmt: expr_stmt  */
#line 164 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1422 "parser.tab.c"
    break;

  case 11: /* stmt: block  */
#line 165 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1428 "parser.tab.c"
    break;

  case 12: /* func_decl: ASYNC FUNCTION IDENTIFIER LPAREN param_list RPAREN block  */
#line 170 "parser.y"
      {
        (yyval.node) = make_node(N_FUNC);
        (yyval.node)->sval  = (yyvsp[-4].str);
        (yyval.node)->ival  = 1; /* async */
        (yyval.node)->left  = make_node(N_BLOCK); (yyval.node)->left->list = (yyvsp[-2].list); /* params as block */
        (yyval.node)->right = (yyvsp[0].node);
      }
#line 1440 "parser.tab.c"
    break;

  case 13: /* func_decl: FUNCTION IDENTIFIER LPAREN param_list RPAREN block  */
#line 178 "parser.y"
      {
        (yyval.node) = make_node(N_FUNC);
        (yyval.node)->sval  = (yyvsp[-4].str);
        (yyval.node)->ival  = 0;
        (yyval.node)->left  = make_node(N_BLOCK); (yyval.node)->left->list = (yyvsp[-2].list);
        (yyval.node)->right = (yyvsp[0].node);
      }
#line 1452 "parser.tab.c"
    break;

  case 14: /* param_list: param_list COMMA IDENTIFIER  */
#line 188 "parser.y"
                                  { (yyval.list) = nl_append((yyvsp[-2].list), make_leaf(N_IDENT, (yyvsp[0].str))); free((yyvsp[0].str)); }
#line 1458 "parser.tab.c"
    break;

  case 15: /* param_list: IDENTIFIER  */
#line 189 "parser.y"
                                  { (yyval.list) = nl_append(NULL, make_leaf(N_IDENT, (yyvsp[0].str))); free((yyvsp[0].str)); }
#line 1464 "parser.tab.c"
    break;

  case 16: /* param_list: %empty  */
#line 190 "parser.y"
                                  { (yyval.list) = NULL; }
#line 1470 "parser.tab.c"
    break;

  case 17: /* block: LBRACE stmt_list RBRACE  */
#line 194 "parser.y"
                              { (yyval.node) = make_node(N_BLOCK); (yyval.node)->list = (yyvsp[-1].list); }
#line 1476 "parser.tab.c"
    break;

  case 18: /* block: LBRACE RBRACE  */
#line 195 "parser.y"
                              { (yyval.node) = make_node(N_BLOCK); (yyval.node)->list = NULL; }
#line 1482 "parser.tab.c"
    break;

  case 19: /* var_decl: CONST IDENTIFIER ASSIGN expr SEMICOLON  */
#line 200 "parser.y"
      { (yyval.node) = make_node(N_VAR_DECL); (yyval.node)->ival=1; (yyval.node)->sval=(yyvsp[-3].str); (yyval.node)->left=(yyvsp[-1].node); }
#line 1488 "parser.tab.c"
    break;

  case 20: /* var_decl: CONST IDENTIFIER ASSIGN expr  */
#line 202 "parser.y"
      { (yyval.node) = make_node(N_VAR_DECL); (yyval.node)->ival=1; (yyval.node)->sval=(yyvsp[-2].str); (yyval.node)->left=(yyvsp[0].node); }
#line 1494 "parser.tab.c"
    break;

  case 21: /* var_decl: LET IDENTIFIER ASSIGN expr SEMICOLON  */
#line 204 "parser.y"
      { (yyval.node) = make_node(N_VAR_DECL); (yyval.node)->ival=0; (yyval.node)->sval=(yyvsp[-3].str); (yyval.node)->left=(yyvsp[-1].node); }
#line 1500 "parser.tab.c"
    break;

  case 22: /* var_decl: LET IDENTIFIER ASSIGN expr  */
#line 206 "parser.y"
      { (yyval.node) = make_node(N_VAR_DECL); (yyval.node)->ival=0; (yyval.node)->sval=(yyvsp[-2].str); (yyval.node)->left=(yyvsp[0].node); }
#line 1506 "parser.tab.c"
    break;

  case 23: /* var_decl: LET IDENTIFIER SEMICOLON  */
#line 208 "parser.y"
      { (yyval.node) = make_node(N_VAR_DECL); (yyval.node)->ival=0; (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=NULL; }
#line 1512 "parser.tab.c"
    break;

  case 24: /* if_stmt: IF LPAREN expr RPAREN block ELSE block  */
#line 213 "parser.y"
      { (yyval.node) = make_node(N_IF); (yyval.node)->left=(yyvsp[-4].node); (yyval.node)->right=(yyvsp[-2].node); (yyval.node)->extra=(yyvsp[0].node); }
#line 1518 "parser.tab.c"
    break;

  case 25: /* if_stmt: IF LPAREN expr RPAREN block  */
#line 215 "parser.y"
      { (yyval.node) = make_node(N_IF); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); (yyval.node)->extra=NULL; }
#line 1524 "parser.tab.c"
    break;

  case 26: /* throw_stmt: THROW expr SEMICOLON  */
#line 219 "parser.y"
                           { (yyval.node) = make_node(N_THROW); (yyval.node)->left=(yyvsp[-1].node); }
#line 1530 "parser.tab.c"
    break;

  case 27: /* throw_stmt: THROW expr  */
#line 220 "parser.y"
                           { (yyval.node) = make_node(N_THROW); (yyval.node)->left=(yyvsp[0].node); }
#line 1536 "parser.tab.c"
    break;

  case 28: /* return_stmt: RETURN expr SEMICOLON  */
#line 224 "parser.y"
                            { (yyval.node) = make_node(N_RETURN); (yyval.node)->left=(yyvsp[-1].node); }
#line 1542 "parser.tab.c"
    break;

  case 29: /* return_stmt: RETURN expr  */
#line 225 "parser.y"
                            { (yyval.node) = make_node(N_RETURN); (yyval.node)->left=(yyvsp[0].node); }
#line 1548 "parser.tab.c"
    break;

  case 30: /* return_stmt: RETURN SEMICOLON  */
#line 226 "parser.y"
                            { (yyval.node) = make_node(N_RETURN); (yyval.node)->left=NULL; }
#line 1554 "parser.tab.c"
    break;

  case 31: /* expr_stmt: assign_expr SEMICOLON  */
#line 230 "parser.y"
                            { (yyval.node) = make_node(N_EXPR_STMT); (yyval.node)->left=(yyvsp[-1].node); }
#line 1560 "parser.tab.c"
    break;

  case 32: /* expr_stmt: assign_expr  */
#line 231 "parser.y"
                            { (yyval.node) = make_node(N_EXPR_STMT); (yyval.node)->left=(yyvsp[0].node); }
#line 1566 "parser.tab.c"
    break;

  case 33: /* expr: assign_expr  */
#line 236 "parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 1572 "parser.tab.c"
    break;

  case 34: /* assign_expr: postfix_expr ASSIGN_OP assign_expr  */
#line 240 "parser.y"
      { (yyval.node) = make_node(N_ASSIGN); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1578 "parser.tab.c"
    break;

  case 35: /* assign_expr: postfix_expr ASSIGN assign_expr  */
#line 242 "parser.y"
      { (yyval.node) = make_node(N_ASSIGN); (yyval.node)->sval=strdup("="); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1584 "parser.tab.c"
    break;

  case 36: /* assign_expr: or_expr  */
#line 243 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1590 "parser.tab.c"
    break;

  case 37: /* or_expr: or_expr OR and_expr  */
#line 248 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=strdup("||"); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1596 "parser.tab.c"
    break;

  case 38: /* or_expr: and_expr  */
#line 249 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1602 "parser.tab.c"
    break;

  case 39: /* and_expr: and_expr AND eq_expr  */
#line 254 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=strdup("&&"); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1608 "parser.tab.c"
    break;

  case 40: /* and_expr: eq_expr  */
#line 255 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1614 "parser.tab.c"
    break;

  case 41: /* eq_expr: eq_expr EQ rel_expr  */
#line 260 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1620 "parser.tab.c"
    break;

  case 42: /* eq_expr: eq_expr NEQ rel_expr  */
#line 262 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1626 "parser.tab.c"
    break;

  case 43: /* eq_expr: rel_expr  */
#line 263 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1632 "parser.tab.c"
    break;

  case 44: /* rel_expr: rel_expr LT add_expr  */
#line 268 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1638 "parser.tab.c"
    break;

  case 45: /* rel_expr: rel_expr GT add_expr  */
#line 270 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1644 "parser.tab.c"
    break;

  case 46: /* rel_expr: rel_expr LE add_expr  */
#line 272 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1650 "parser.tab.c"
    break;

  case 47: /* rel_expr: rel_expr GE add_expr  */
#line 274 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1656 "parser.tab.c"
    break;

  case 48: /* rel_expr: add_expr  */
#line 275 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1662 "parser.tab.c"
    break;

  case 49: /* add_expr: add_expr ADDOP mul_expr  */
#line 280 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1668 "parser.tab.c"
    break;

  case 50: /* add_expr: mul_expr  */
#line 281 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1674 "parser.tab.c"
    break;

  case 51: /* mul_expr: mul_expr MULOP unary_expr  */
#line 286 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1680 "parser.tab.c"
    break;

  case 52: /* mul_expr: unary_expr  */
#line 287 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 1686 "parser.tab.c"
    break;

  case 53: /* unary_expr: NOT unary_expr  */
#line 292 "parser.y"
      { (yyval.node) = make_node(N_UNARY); (yyval.node)->sval=strdup("!"); (yyval.node)->left=(yyvsp[0].node); }
#line 1692 "parser.tab.c"
    break;

  case 54: /* unary_expr: postfix_expr  */
#line 293 "parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 1698 "parser.tab.c"
    break;

  case 55: /* postfix_expr: postfix_expr DOT IDENTIFIER LPAREN arg_list RPAREN  */
#line 298 "parser.y"
      {
        /* method call: obj.method(args) */
        Node *mem = make_node(N_MEMBER);
        mem->left  = (yyvsp[-5].node);
        mem->right = make_leaf(N_IDENT, (yyvsp[-3].str)); free((yyvsp[-3].str));
        (yyval.node) = make_node(N_CALL);
        (yyval.node)->left = mem;
        (yyval.node)->list = (yyvsp[-1].list);
      }
#line 1712 "parser.tab.c"
    break;

  case 56: /* postfix_expr: postfix_expr DOT IDENTIFIER  */
#line 308 "parser.y"
      {
        (yyval.node) = make_node(N_MEMBER);
        (yyval.node)->left  = (yyvsp[-2].node);
        (yyval.node)->right = make_leaf(N_IDENT, (yyvsp[0].str)); free((yyvsp[0].str));
      }
#line 1722 "parser.tab.c"
    break;

  case 57: /* postfix_expr: postfix_expr LPAREN arg_list RPAREN  */
#line 314 "parser.y"
      {
        (yyval.node) = make_node(N_CALL);
        (yyval.node)->left = (yyvsp[-3].node);
        (yyval.node)->list = (yyvsp[-1].list);
      }
#line 1732 "parser.tab.c"
    break;

  case 58: /* postfix_expr: primary_expr  */
#line 319 "parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 1738 "parser.tab.c"
    break;

  case 59: /* primary_expr: IDENTIFIER  */
#line 323 "parser.y"
                      { (yyval.node) = make_leaf(N_IDENT,  (yyvsp[0].str)); free((yyvsp[0].str)); }
#line 1744 "parser.tab.c"
    break;

  case 60: /* primary_expr: STRING_LIT  */
#line 324 "parser.y"
                      { (yyval.node) = make_leaf(N_STR,    (yyvsp[0].str)); free((yyvsp[0].str)); }
#line 1750 "parser.tab.c"
    break;

  case 61: /* primary_expr: NUMBER_LIT  */
#line 325 "parser.y"
                      { (yyval.node) = make_leaf(N_NUM,    (yyvsp[0].str)); free((yyvsp[0].str)); }
#line 1756 "parser.tab.c"
    break;

  case 62: /* primary_expr: TRUE_LIT  */
#line 326 "parser.y"
                      { (yyval.node) = make_node(N_BOOL); (yyval.node)->ival=1; (yyval.node)->sval=strdup("true"); }
#line 1762 "parser.tab.c"
    break;

  case 63: /* primary_expr: FALSE_LIT  */
#line 327 "parser.y"
                      { (yyval.node) = make_node(N_BOOL); (yyval.node)->ival=0; (yyval.node)->sval=strdup("false"); }
#line 1768 "parser.tab.c"
    break;

  case 64: /* primary_expr: NULL_LIT  */
#line 328 "parser.y"
                      { (yyval.node) = make_node(N_NULL); (yyval.node)->sval=strdup("null"); }
#line 1774 "parser.tab.c"
    break;

  case 65: /* primary_expr: NEW IDENTIFIER LPAREN arg_list RPAREN  */
#line 330 "parser.y"
      { (yyval.node) = make_node(N_NEW); (yyval.node)->sval=(yyvsp[-3].str); (yyval.node)->list=(yyvsp[-1].list); }
#line 1780 "parser.tab.c"
    break;

  case 66: /* primary_expr: object_literal  */
#line 331 "parser.y"
                      { (yyval.node) = (yyvsp[0].node); }
#line 1786 "parser.tab.c"
    break;

  case 67: /* primary_expr: array_literal  */
#line 332 "parser.y"
                      { (yyval.node) = (yyvsp[0].node); }
#line 1792 "parser.tab.c"
    break;

  case 68: /* primary_expr: arrow_func  */
#line 333 "parser.y"
                      { (yyval.node) = (yyvsp[0].node); }
#line 1798 "parser.tab.c"
    break;

  case 69: /* primary_expr: LPAREN expr RPAREN  */
#line 334 "parser.y"
                         { (yyval.node) = (yyvsp[-1].node); }
#line 1804 "parser.tab.c"
    break;

  case 70: /* arrow_func: IDENTIFIER ARROW assign_expr  */
#line 339 "parser.y"
      { (yyval.node) = make_node(N_ARROW); (yyval.node)->left=make_leaf(N_IDENT,(yyvsp[-2].str)); free((yyvsp[-2].str)); (yyval.node)->right=(yyvsp[0].node); }
#line 1810 "parser.tab.c"
    break;

  case 71: /* arrow_func: IDENTIFIER ARROW block  */
#line 341 "parser.y"
      { (yyval.node) = make_node(N_ARROW); (yyval.node)->left=make_leaf(N_IDENT,(yyvsp[-2].str)); free((yyvsp[-2].str)); (yyval.node)->right=(yyvsp[0].node); }
#line 1816 "parser.tab.c"
    break;

  case 72: /* arrow_func: LPAREN param_list RPAREN ARROW block  */
#line 343 "parser.y"
      { (yyval.node) = make_node(N_ARROW);
        (yyval.node)->left = make_node(N_BLOCK); (yyval.node)->left->list=(yyvsp[-3].list);
        (yyval.node)->right=(yyvsp[0].node); }
#line 1824 "parser.tab.c"
    break;

  case 73: /* object_literal: LBRACE prop_list RBRACE  */
#line 349 "parser.y"
                              { (yyval.node) = make_node(N_OBJECT); (yyval.node)->list=(yyvsp[-1].list); }
#line 1830 "parser.tab.c"
    break;

  case 74: /* object_literal: LBRACE RBRACE  */
#line 350 "parser.y"
                              { (yyval.node) = make_node(N_OBJECT); (yyval.node)->list=NULL; }
#line 1836 "parser.tab.c"
    break;

  case 75: /* prop_list: prop_list COMMA prop  */
#line 354 "parser.y"
                           { (yyval.list) = nl_append((yyvsp[-2].list), (yyvsp[0].node)); }
#line 1842 "parser.tab.c"
    break;

  case 76: /* prop_list: prop  */
#line 355 "parser.y"
                           { (yyval.list) = nl_append(NULL, (yyvsp[0].node)); }
#line 1848 "parser.tab.c"
    break;

  case 77: /* prop: IDENTIFIER COLON expr  */
#line 360 "parser.y"
      { (yyval.node) = make_node(N_PROP); (yyval.node)->sval=(yyvsp[-2].str); (yyval.node)->left=(yyvsp[0].node); }
#line 1854 "parser.tab.c"
    break;

  case 78: /* prop: STRING_LIT COLON expr  */
#line 362 "parser.y"
      { (yyval.node) = make_node(N_PROP); (yyval.node)->sval=(yyvsp[-2].str); (yyval.node)->left=(yyvsp[0].node); }
#line 1860 "parser.tab.c"
    break;

  case 79: /* array_literal: LBRACKET elem_list RBRACKET  */
#line 366 "parser.y"
                                  { (yyval.node) = make_node(N_ARRAY); (yyval.node)->list=(yyvsp[-1].list); }
#line 1866 "parser.tab.c"
    break;

  case 80: /* array_literal: LBRACKET RBRACKET  */
#line 367 "parser.y"
                                  { (yyval.node) = make_node(N_ARRAY); (yyval.node)->list=NULL; }
#line 1872 "parser.tab.c"
    break;

  case 81: /* elem_list: elem_list COMMA expr  */
#line 371 "parser.y"
                           { (yyval.list) = nl_append((yyvsp[-2].list), (yyvsp[0].node)); }
#line 1878 "parser.tab.c"
    break;

  case 82: /* elem_list: expr  */
#line 372 "parser.y"
                           { (yyval.list) = nl_append(NULL, (yyvsp[0].node)); }
#line 1884 "parser.tab.c"
    break;

  case 83: /* arg_list: arg_list COMMA expr  */
#line 376 "parser.y"
                          { (yyval.list) = nl_append((yyvsp[-2].list), (yyvsp[0].node)); }
#line 1890 "parser.tab.c"
    break;

  case 84: /* arg_list: expr  */
#line 377 "parser.y"
                          { (yyval.list) = nl_append(NULL, (yyvsp[0].node)); }
#line 1896 "parser.tab.c"
    break;

  case 85: /* arg_list: %empty  */
#line 378 "parser.y"
                          { (yyval.list) = NULL; }
#line 1902 "parser.tab.c"
    break;


#line 1906 "parser.tab.c"

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

#line 381 "parser.y"


/* ════════════════════════════════════════════════════════
   SEMANTIC ANALYSIS
   ════════════════════════════════════════════════════════ */

/* PascalCase the first letter of a word */
static void pascal(char *dst, const char *src, int maxlen) {
    strncpy(dst, src, maxlen-1); dst[maxlen-1]='\0';
    if (dst[0]>='a'&&dst[0]<='z') dst[0]-=32;
}

/* Derive a type name from a variable name.
   "newUser" → "User",  "newFile" → "FileRecord" (last capital word),
   "userData" → "UserData" → we strip trailing "Data" and add "Input" */
static char *derive_type_name(const char *vname) {
    static char buf[64];
    /* strip leading "new" */
    const char *p = vname;
    if (strncmp(p,"new",3)==0 && p[3]>='A'&&p[3]<='Z') p+=3;
    pascal(buf, p, 64);
    /* avoid collision with JS/TS built-in types */
    static const char *builtins[] = {
        "File","Error","Event","URL","Map","Set","Date","Array","Object",
        "Promise","Function","Symbol","Number","String","Boolean", NULL
    };
    for (int i=0; builtins[i]; i++)
        if (strcmp(buf, builtins[i])==0) { strncat(buf,"Record",63-strlen(buf)); break; }
    return buf;
}

/* Derive an Input type name from a param name.
   "userData" → "UserInput",  "fileData" → "FileInput",
   "order"    → "OrderInput" */
static char *derive_input_type(const char *pname) {
    static char buf[64];
    /* strip trailing "Data" if present */
    char tmp[64]; strncpy(tmp, pname, 63);
    int len = strlen(tmp);
    if (len>4 && strcmp(tmp+len-4,"Data")==0) tmp[len-4]='\0';
    pascal(buf, tmp, 60);
    strncat(buf, "Input", 63-strlen(buf));
    return buf;
}

/* Walk an object literal and register its shape as a TypeDef.
   For member-access values (param.field), we record type as "string" by default
   since we can't resolve them at this point — they'll be overridden if we see
   a string literal at the call site. */
static void register_object_type(const char *tname, Node *obj) {
    if (!obj || obj->kind != N_OBJECT) return;
    TypeDef *td = type_new(tname);
    for (NodeList *l = obj->list; l; l = l->next) {
        Node *p = l->node;
        if (!p || p->kind != N_PROP) continue;
        const char *pt = prop_val_type(p->left);
        /* member access like userData.email — infer from field name */
        if (strcmp(pt,"unknown")==0 && p->left && p->left->kind==N_MEMBER) {
            Node *field = p->left->right;
            if (field && field->kind==N_IDENT) {
                /* common field name heuristics */
                const char *fn = field->sval;
                if (strstr(fn,"At")!=NULL || strstr(fn,"Date")!=NULL ||
                    strstr(fn,"Name")!=NULL || strstr(fn,"name")!=NULL ||
                    strstr(fn,"email")!=NULL || strstr(fn,"password")!=NULL ||
                    strstr(fn,"status")!=NULL || strstr(fn,"url")!=NULL)
                    pt = "string";
                else if (strstr(fn,"id")!=NULL || strstr(fn,"Id")!=NULL ||
                         strstr(fn,"size")!=NULL || strstr(fn,"count")!=NULL ||
                         strstr(fn,"quantity")!=NULL || strstr(fn,"stock")!=NULL)
                    pt = "number";
                else
                    pt = "string"; /* safe default */
            }
        }
        type_add_prop(td, p->sval, pt);
    }
}

/* Infer return type: find `const newX = { ... }` in body → Promise<X> */
static char *infer_return_type(Node *func) {
    static char buf[64];
    Node *body = func->right;
    if (!body) return "void";
    char last_obj_var[64] = "";
    for (NodeList *l = body->list; l; l = l->next) {
        Node *s = l->node;
        if (s && s->kind == N_VAR_DECL && s->left && s->left->kind == N_OBJECT)
            strncpy(last_obj_var, s->sval, 63);
    }
    if (last_obj_var[0]) {
        snprintf(buf, 63, "Promise<%s>", derive_type_name(last_obj_var));
        return buf;
    }
    return "Promise<unknown>";
}

/* Scan function body and infer param types from usage */
static void infer_param_types(Node *func) {
    NodeList *params = func->left ? func->left->list : NULL;
    Node     *body   = func->right;
    if (!params || !body) return;

    char pnames[8][64]; int np = 0;
    for (NodeList *l = params; l && np < 8; l = l->next)
        if (l->node) strncpy(pnames[np++], l->node->sval, 63);

    /* track which params are arrays */
    int is_array[8] = {0};

    for (NodeList *sl = body->list; sl; sl = sl->next) {
        Node *s = sl->node;
        if (!s) continue;

        /* expr_stmt: param.push/find/filter/map → array */
        if (s->kind == N_EXPR_STMT && s->left) {
            Node *e = s->left;
            if (e->kind == N_CALL && e->left && e->left->kind == N_MEMBER) {
                Node *obj = e->left->left, *mth = e->left->right;
                if (obj && obj->kind == N_IDENT && mth)
                    for (int i=0;i<np;i++)
                        if (strcmp(obj->sval,pnames[i])==0 &&
                            (strcmp(mth->sval,"push")==0||strcmp(mth->sval,"find")==0||
                             strcmp(mth->sval,"filter")==0||strcmp(mth->sval,"map")==0))
                            is_array[i]=1;
            }
        }

        /* const x = param.find/filter → param is array */
        if (s->kind == N_VAR_DECL && s->left) {
            Node *e = s->left;
            if (e->kind == N_CALL && e->left && e->left->kind == N_MEMBER) {
                Node *obj = e->left->left, *mth = e->left->right;
                if (obj && obj->kind == N_IDENT && mth)
                    for (int i=0;i<np;i++)
                        if (strcmp(obj->sval,pnames[i])==0 &&
                            (strcmp(mth->sval,"find")==0||strcmp(mth->sval,"filter")==0)) {
                            is_array[i]=1;
                        }
            }
            /* const newX = { ... } → register object type */
            if (s->left->kind == N_OBJECT) {
                char *tn = derive_type_name(s->sval);
                register_object_type(tn, s->left);
                var_set(s->sval, tn, func->sval);
            }
        }
    }

    /* Now assign types to params.
       Array params get ElemType[] where ElemType = the object type found in body.
       Non-array params get InputType derived from param name. */
    char elem_type[64] = "";
    /* find the object type registered for this function */
    for (int i=0;i<nvar;i++)
        if (strcmp(vars[i].scope,func->sval)==0 &&
            strncmp(vars[i].type,"__",2)!=0 &&
            strcmp(vars[i].type,"unknown")!=0 &&
            strcmp(vars[i].type,"string")!=0 &&
            strcmp(vars[i].type,"number")!=0 &&
            strcmp(vars[i].type,"boolean")!=0) {
            strncpy(elem_type, vars[i].type, 63); break;
        }

    for (int i=0;i<np;i++) {
        if (is_array[i]) {
            char arr[72];
            /* Derive element type from param name: "products" → "Product",
               "users" → "User", "files" → "FileRecord" (builtin check) */
            char elem[64];
            /* strip trailing 's' for plurals */
            strncpy(elem, pnames[i], 63);
            int elen = strlen(elem);
            if (elen > 1 && elem[elen-1]=='s') elem[elen-1]='\0';
            pascal(elem, elem, 64);
            /* builtin collision check */
            static const char *builtins2[] = {
                "File","Error","Event","URL","Map","Set","Date","Array","Object",
                "Promise","Function","Symbol","Number","String","Boolean", NULL
            };
            for (int j=0; builtins2[j]; j++)
                if (strcmp(elem, builtins2[j])==0) { strncat(elem,"Record",63-strlen(elem)); break; }
            snprintf(arr,71,"%s[]",elem);
            var_set(pnames[i], arr, func->sval);
        } else if (strcmp(var_type_in(pnames[i],func->sval),"unknown")==0 ||
                   var_type_in(pnames[i],func->sval)[0]=='\0') {
            var_set(pnames[i], derive_input_type(pnames[i]), func->sval);
        }
    }
}

/* Main semantic pass */
static void infer_program(Node *n) {
    if (!n) return;
    for (NodeList *l = n->list; l; l = l->next) {
        Node *s = l->node;
        if (!s) continue;
        if (s->kind == N_FUNC) {
            infer_param_types(s);
            char sig[128], ret[64];
            strncpy(ret, infer_return_type(s), 63);
            snprintf(sig, 127, "async function(...): %s", ret);
            var_set(s->sval, sig, "global");
        }
        if (s->kind == N_VAR_DECL && s->left && s->left->kind == N_ARRAY) {
            var_set(s->sval, "__let_array__", "global");
            /* if array has object literal elements, register their type */
            if (s->left->list && s->left->list->node &&
                s->left->list->node->kind == N_OBJECT) {
                /* derive type name from var name (strip 's') */
                char elem[64]; strncpy(elem, s->sval, 63);
                int elen = strlen(elem);
                if (elen>1 && elem[elen-1]=='s') elem[elen-1]='\0';
                pascal(elem, elem, 64);
                register_object_type(elem, s->left->list->node);
            }
        }
    }
}

/* ════════════════════════════════════════════════════════
   SYMBOL TABLE PRINT
   ════════════════════════════════════════════════════════ */
static void print_symtable(void) {
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║              SYMBOL TABLE (Semantic Analysis)                ║\n");
    printf("╠════════════════╦══════════════════════════════╦══════════════╣\n");
    printf("║ %-14s ║ %-28s ║ %-12s ║\n", "Identifier", "Type", "Scope");
    printf("╠════════════════╬══════════════════════════════╬══════════════╣\n");
    for (int i = 0; i < nvar; i++)
        printf("║ %-14s ║ %-28s ║ %-12s ║\n",
               vars[i].name, vars[i].type, vars[i].scope);
    printf("╚════════════════╩══════════════════════════════╩══════════════╝\n");
}

/* ════════════════════════════════════════════════════════
   CODE GENERATION
   ════════════════════════════════════════════════════════ */

static void indent_f(FILE *f, int d) {
    for (int i = 0; i < d; i++) fprintf(f, "    ");
}

/* Resolve the TypeScript type for a param variable */
static const char *ts_param_type(const char *pname, const char *fname) {
    return var_type_in(pname, fname);
}

/* Emit a TypeScript type for a variable name used in an expression */
static const char *ts_type_of(const char *name) {
    return var_type(name);
}

static void emit_expr(Node *n, FILE *f) {
    if (!n) return;
    switch (n->kind) {
    case N_IDENT:  fprintf(f, "%s", n->sval); break;
    case N_STR:    fprintf(f, "%s", n->sval); break;
    case N_NUM:    fprintf(f, "%s", n->sval); break;
    case N_BOOL:   fprintf(f, "%s", n->sval); break;
    case N_NULL:   fprintf(f, "null"); break;
    case N_UNARY:
        fprintf(f, "%s", n->sval);
        emit_expr(n->left, f);
        break;
    case N_BINARY:
        emit_expr(n->left, f);
        fprintf(f, " %s ", n->sval);
        emit_expr(n->right, f);
        break;
    case N_ASSIGN:
        emit_expr(n->left, f);
        fprintf(f, " %s ", n->sval);
        emit_expr(n->right, f);
        break;
    case N_MEMBER:
        emit_expr(n->left, f);
        fprintf(f, ".");
        emit_expr(n->right, f);
        break;
    case N_CALL: {
        /* special: array.find(arrow) — annotate arrow param with element type */
        int is_find = 0;
        const char *elem_type = NULL;
        if (n->left && n->left->kind == N_MEMBER) {
            Node *mth = n->left->right;
            Node *obj = n->left->left;
            if (mth && (strcmp(mth->sval,"find")==0 ||
                        strcmp(mth->sval,"filter")==0 ||
                        strcmp(mth->sval,"map")==0)) {
                is_find = 1;
                if (obj && obj->kind == N_IDENT) {
                    const char *at = ts_type_of(obj->sval);
                    /* strip [] */
                    static char et[64];
                    strncpy(et, at, 63);
                    int len = strlen(et);
                    if (len > 2 && et[len-2]=='[' && et[len-1]==']')
                        et[len-2] = '\0';
                    elem_type = et;
                }
            }
        }
        emit_expr(n->left, f);
        fprintf(f, "(");
        int first = 1;
        for (NodeList *l = n->list; l; l = l->next) {
            if (!first) fprintf(f, ", ");
            first = 0;
            Node *arg = l->node;
            /* annotate arrow param if this is a find/filter/map call */
            if (is_find && arg && arg->kind == N_ARROW &&
                arg->left && arg->left->kind == N_IDENT && elem_type) {
                fprintf(f, "(%s: %s) => ", arg->left->sval, elem_type);
                emit_expr(arg->right, f);
            } else {
                emit_expr(arg, f);
            }
        }
        fprintf(f, ")");
        break;
    }
    case N_NEW:
        fprintf(f, "new %s(", n->sval);
        { int first=1;
          for (NodeList *l=n->list;l;l=l->next){
              if(!first)fprintf(f,", ");first=0;
              emit_expr(l->node,f);
          }
        }
        fprintf(f, ")");
        break;
    case N_ARROW:
        if (n->left && n->left->kind == N_IDENT)
            fprintf(f, "%s => ", n->left->sval);
        emit_expr(n->right, f);
        break;
    case N_OBJECT: {
        fprintf(f, "{\n");
        int first = 1;
        for (NodeList *l = n->list; l; l = l->next) {
            Node *p = l->node;
            if (!p || p->kind != N_PROP) continue;
            if (!first) fprintf(f, ",\n");
            first = 0;
            fprintf(f, "        %s: ", p->sval);
            emit_expr(p->left, f);
        }
        fprintf(f, "\n    }");
        break;
    }
    case N_ARRAY:
        fprintf(f, "[");
        { int first=1;
          for (NodeList *l=n->list;l;l=l->next){
              if(!first)fprintf(f,", ");first=0;
              emit_expr(l->node,f);
          }
        }
        fprintf(f, "]");
        break;
    default: break;
    }
}

static void emit_stmt(Node *n, FILE *f, int ind) {
    if (!n) return;
    switch (n->kind) {
    case N_EXPR_STMT:
        indent_f(f, ind);
        emit_expr(n->left, f);
        fprintf(f, ";\n");
        break;
    case N_VAR_DECL: {
        indent_f(f, ind);
        const char *kw = n->ival ? "const" : "let";
        /* determine TS type annotation */
        char ts_type[128] = "";
        if (n->left) {
            if (n->left->kind == N_OBJECT) {
                char *tn = derive_type_name(n->sval);
                snprintf(ts_type, 127, ": %s", tn);
            } else if (n->left->kind == N_ARRAY) {
                /* let x = [] — look up inferred type */
                const char *vt = var_type(n->sval);
                if (strcmp(vt,"__let_array__")==0 || strcmp(vt,"unknown")==0) {
                    /* try to find element type from function context */
                    snprintf(ts_type, 127, ": unknown[]");
                } else {
                    snprintf(ts_type, 127, ": %s", vt);
                }
            } else if (n->left->kind == N_NUM ||
                       (n->left->kind == N_BINARY)) {
                snprintf(ts_type, 127, ": number");
            }
        }
        fprintf(f, "%s %s%s", kw, n->sval, ts_type);
        if (n->left) {
            fprintf(f, " = ");
            emit_expr(n->left, f);
        }
        fprintf(f, ";\n");
        break;
    }
    case N_IF:
        indent_f(f, ind);
        fprintf(f, "if (");
        emit_expr(n->left, f);
        fprintf(f, ") {\n");
        emit_block_stmts(n->right ? n->right->list : NULL, f, ind+1);
        indent_f(f, ind);
        fprintf(f, "}");
        if (n->extra) {
            fprintf(f, " else {\n");
            emit_block_stmts(n->extra->list, f, ind+1);
            indent_f(f, ind);
            fprintf(f, "}");
        }
        fprintf(f, "\n");
        break;
    case N_THROW:
        indent_f(f, ind);
        fprintf(f, "throw ");
        emit_expr(n->left, f);
        fprintf(f, ";\n");
        break;
    case N_RETURN:
        indent_f(f, ind);
        fprintf(f, "return");
        if (n->left) { fprintf(f, " "); emit_expr(n->left, f); }
        fprintf(f, ";\n");
        break;
    case N_BLOCK:
        emit_block_stmts(n->list, f, ind);
        break;
    default: break;
    }
}

static void emit_block_stmts(NodeList *l, FILE *f, int ind) {
    for (; l; l = l->next) emit_stmt(l->node, f, ind);
}

/* Emit type alias definitions discovered during semantic analysis */
static void emit_type_aliases(FILE *f) {
    for (int i = 0; i < ntype; i++) {
        TypeDef *td = &types[i];
        fprintf(f, "type %s = {\n", td->name);
        for (int j = 0; j < td->nprops; j++)
            fprintf(f, "    %s: %s;\n", td->props[j].key, td->props[j].type);
        fprintf(f, "};\n\n");
    }
}

/* Emit a function with typed params and return type */
static void emit_func(Node *n, FILE *f) {
    if (!n || n->kind != N_FUNC) return;

    /* collect param names */
    NodeList *params = n->left ? n->left->list : NULL;

    /* infer return type */
    char ret_type[64];
    strncpy(ret_type, infer_return_type(n), 63);

    if (n->ival) fprintf(f, "async ");
    fprintf(f, "function %s(\n", n->sval);

    for (NodeList *l = params; l; l = l->next) {
        Node *p = l->node;
        if (!p) continue;
        const char *pt = ts_param_type(p->sval, n->sval);
        fprintf(f, "    %s: %s", p->sval, pt);
        if (l->next) fprintf(f, ",");
        fprintf(f, "\n");
    }

    if (n->ival)
        fprintf(f, "): %s {\n", ret_type);
    else
        fprintf(f, ") {\n");

    /* emit body */
    if (n->right) emit_block_stmts(n->right->list, f, 1);

    fprintf(f, "}\n\n");
}

/* Emit a top-level let declaration with proper array type */
static void emit_top_let(Node *n, FILE *f) {
    if (!n || n->kind != N_VAR_DECL) return;
    /* find any param typed as X[] across all function scopes */
    char arr_type[64] = "unknown[]";
    for (int i = 0; i < nvar; i++) {
        int len = strlen(vars[i].type);
        if (len > 2 && vars[i].type[len-2]=='[' && vars[i].type[len-1]==']' &&
            strcmp(vars[i].scope,"global")!=0) {
            strncpy(arr_type, vars[i].type, 63);
            break;
        }
    }
    fprintf(f, "let %s: %s = ", n->sval, arr_type);
    if (n->left) emit_expr(n->left, f);
    else fprintf(f, "[]");
    fprintf(f, ";\n");
}

static void emit_program(Node *n, FILE *f) {
    if (!n) return;
    emit_type_aliases(f);
    for (NodeList *l = n->list; l; l = l->next) {
        Node *s = l->node;
        if (!s) continue;
        if (s->kind == N_FUNC) {
            emit_func(s, f);
        } else if (s->kind == N_VAR_DECL && !s->ival) {
            /* top-level let */
            emit_top_let(s, f);
        } else if (s->kind == N_EXPR_STMT) {
            /* top-level call like registerUser(...).then(...) — skip or emit */
            /* skip .then/.catch chains at top level — they're test harness code */
        }
    }
}

/* ════════════════════════════════════════════════════════
   MAIN
   ════════════════════════════════════════════════════════ */

void yyerror(const char *s) {
    fprintf(stderr, "[Parse Error] line %d: %s\n", yylineno, s);
}

int main(void) {
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║         JS → TS Compiler  (Lex + Bison)              ║\n");
    printf("║   Phases: Lexical | Syntax | Semantic | CodeGen      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── PHASE 1 & 2: Lexical + Syntax Analysis ─────────────\n");
    if (yyparse() != 0) {
        fprintf(stderr, "Parsing failed.\n");
        return 1;
    }
    printf("  ✔ Parse successful.\n");

    printf("\n── PHASE 3: Semantic Analysis ─────────────────────────\n");
    infer_program(root);
    print_symtable();

    printf("\n── PHASE 4: Code Generation ───────────────────────────\n");
    FILE *out = fopen("output.ts", "w");
    if (!out) { perror("output.ts"); return 1; }
    emit_program(root, out);
    fclose(out);
    printf("  ✔ TypeScript written to output.ts\n");

    ast_free(root);
    printf("\n✅ Compilation successful.\n");
    return 0;
}
