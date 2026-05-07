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
#include <stdarg.h>
#include "ast.h"

void yyerror(const char *s);
int  yylex(void);
extern int yylineno;

static Node *root = NULL;
static FILE *trace_f = NULL;  /* trace output file, NULL = no tracing */

static void trace(const char *phase, const char *rule) {
    if (!trace_f) return;
    fprintf(trace_f, "[%-8s]  %s\n", phase, rule);
}

/* ── Error tracking ── */
#define MAX_ERRORS 32
typedef struct { int line; char msg[128]; } CompileError;
static CompileError errors[MAX_ERRORS];
static int nerrors = 0;

static void sem_error(int line, const char *fmt, ...) {
    if (nerrors >= MAX_ERRORS) return;
    errors[nerrors].line = line;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(errors[nerrors].msg, 127, fmt, ap);
    va_end(ap);
    nerrors++;
}

static void print_errors(void) {
    if (nerrors == 0) return;
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║                  COMPILATION ERRORS                 ║\n");
    printf("╠══════════════════════════════════════════════════════╣\n");
    for (int i = 0; i < nerrors; i++)
        printf("║  line %-4d  %s\n", errors[i].line, errors[i].msg);
    printf("╚══════════════════════════════════════════════════════╝\n");
}

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


#line 220 "parser.tab.c"

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

#if 1

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
#endif /* 1 */

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
#define YYFINAL  72
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   313

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  30
/* YYNRULES -- Number of rules.  */
#define YYNRULES  88
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  161

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
       0,   184,   184,   188,   189,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   206,   214,   225,   226,   227,
     231,   232,   236,   238,   240,   242,   244,   249,   251,   256,
     257,   261,   262,   263,   267,   268,   273,   276,   278,   280,
     284,   286,   290,   292,   296,   298,   300,   304,   306,   308,
     310,   312,   316,   318,   322,   324,   328,   330,   334,   344,
     350,   356,   360,   361,   362,   363,   364,   365,   366,   368,
     369,   370,   371,   375,   377,   379,   386,   387,   391,   392,
     396,   398,   403,   404,   408,   409,   413,   414,   415
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
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

#define YYPACT_NINF (-100)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-19)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     210,   -10,    14,     7,     9,    17,    39,   219,   250,    27,
    -100,  -100,  -100,   250,   272,   114,   241,    51,  -100,  -100,
      68,    22,  -100,  -100,  -100,  -100,  -100,  -100,  -100,  -100,
      54,    66,    74,    69,    41,    45,    82,  -100,   -11,  -100,
    -100,  -100,  -100,  -100,  -100,  -100,    86,   107,   117,   110,
     250,  -100,    29,   113,  -100,   119,   130,  -100,   111,    36,
      16,   118,  -100,    -1,   142,   146,    -3,  -100,  -100,  -100,
      33,   281,  -100,  -100,  -100,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   137,   250,   250,   144,
     139,   250,   250,  -100,   150,  -100,   159,   142,  -100,  -100,
     250,   141,   171,  -100,   250,   250,  -100,   131,  -100,   250,
    -100,  -100,  -100,    74,    69,    41,    41,    45,    45,    45,
      45,    82,  -100,  -100,   169,  -100,    42,  -100,   139,  -100,
      48,   174,   175,   172,    58,  -100,   172,  -100,  -100,  -100,
    -100,   250,   250,  -100,    67,   172,  -100,  -100,   178,   189,
    -100,  -100,    76,  -100,   172,  -100,  -100,   172,  -100,  -100,
    -100
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      65,    66,    67,     0,    19,     0,     0,    62,    63,    64,
       0,     0,     4,     5,    11,     6,     7,     8,     9,    10,
      35,    39,    41,    43,    46,    51,    53,    55,    57,    61,
      71,    69,    70,    12,    14,    13,     0,     0,     0,     0,
       0,    33,     0,    32,    36,    30,     0,    56,    57,    62,
       0,     0,    21,    62,    63,     0,     0,    79,    83,    85,
       0,     0,     1,     3,    34,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    88,     0,     0,
      19,     0,     0,    26,     0,    77,     0,     0,    31,    29,
      88,     0,     0,    72,     0,     0,    20,     0,    76,     0,
      82,    74,    73,    40,    42,    44,    45,    47,    48,    49,
      50,    52,    54,    38,    59,    87,     0,    37,    19,    18,
       0,    23,    25,     0,     0,    17,     0,    80,    81,    78,
      84,    88,     0,    60,     0,     0,    22,    24,     0,    28,
      68,    75,     0,    86,     0,    16,    21,     0,    58,    15,
      27
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -100,  -100,   199,   -16,  -100,   -84,   -64,  -100,  -100,  -100,
    -100,  -100,    -5,     0,  -100,   125,   127,    88,    15,   124,
      -9,    30,  -100,  -100,  -100,  -100,    94,  -100,  -100,   -99
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,    20,    65,    22,    23,    60,    24,    25,    26,    27,
      28,    29,   125,    54,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    66,    67,    42,    70,   126
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      30,   134,    53,    55,    57,    73,   130,   111,    85,    61,
      43,    69,    86,    87,    71,    30,    44,    45,    46,   107,
     104,    30,    -2,     1,   108,     2,     3,     4,     5,     6,
      88,     7,     8,     9,    10,    11,    12,    47,   101,    48,
      13,   102,   152,    58,   144,    94,    14,    49,    15,    73,
      16,    71,    17,    18,    19,   109,    95,    56,   -18,    96,
      97,   -18,   110,    50,   142,    30,    71,   143,    72,   149,
     101,   112,   151,   145,    74,   122,    79,    80,    81,    82,
     142,   155,    75,   150,    83,   123,   131,   132,   127,   101,
     159,    76,   154,   160,   117,   118,   119,   120,   142,   137,
     138,   158,    77,    78,   140,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,     1,    89,     2,     3,     4,
       5,     6,    84,     7,     8,     9,    10,    11,    12,    92,
      93,    90,    13,    98,    86,    87,    91,   153,    14,    99,
      15,    62,    16,   103,    63,    64,    19,     1,    30,     2,
       3,     4,     5,     6,   100,     7,     8,     9,    10,    11,
      12,    96,    97,   105,    13,   115,   116,   124,   128,   129,
      14,   135,    15,   106,    16,   133,    17,    18,    19,     1,
     104,     2,     3,     4,     5,     6,   136,     7,     8,     9,
      10,    11,    12,   141,   146,   147,    13,   157,   148,    21,
     113,   139,    14,   114,    15,   156,    16,   121,    17,    18,
      19,     1,     0,     2,     3,     4,     5,     6,     0,     7,
       8,     9,    10,    11,    12,     0,     0,     0,    13,     0,
       9,    10,    11,    12,    14,     0,    15,    13,    16,    51,
      17,    18,    19,    14,     0,    52,     0,    16,     0,    17,
      18,    19,     9,    10,    11,    12,     0,     0,     0,    13,
       0,     9,    10,    11,    12,    14,     0,    52,    13,    16,
      68,    17,    18,    19,    14,     0,    52,     0,    16,     0,
      17,    18,    19,     9,    10,    11,    12,     0,     0,     0,
      13,     0,     9,    10,    11,    12,    14,     0,    52,    13,
      16,     0,    59,    18,    19,    14,     0,    15,     0,    16,
       0,    17,    18,    19
};

static const yytype_int16 yycheck[] =
{
       0,   100,     7,     8,    13,    21,    90,    71,    19,    14,
      20,    16,    23,    24,    15,    15,    26,    27,     4,    22,
      21,    21,     0,     1,    27,     3,     4,     5,     6,     7,
      41,     9,    10,    11,    12,    13,    14,    30,    22,    30,
      18,    25,   141,    13,   128,    50,    24,    30,    26,    65,
      28,    15,    30,    31,    32,    22,    27,    30,    22,    30,
      31,    25,    29,    24,    22,    65,    15,    25,     0,   133,
      22,    71,   136,    25,    20,    84,    35,    36,    37,    38,
      22,   145,    16,    25,    39,    85,    91,    92,    88,    22,
     154,    17,    25,   157,    79,    80,    81,    82,    22,   104,
     105,    25,    33,    34,   109,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,     1,    30,     3,     4,     5,
       6,     7,    40,     9,    10,    11,    12,    13,    14,    19,
      20,    24,    18,    20,    23,    24,    19,   142,    24,    20,
      26,    27,    28,    25,    30,    31,    32,     1,   148,     3,
       4,     5,     6,     7,    24,     9,    10,    11,    12,    13,
      14,    30,    31,    21,    18,    77,    78,    30,    24,    30,
      24,    30,    26,    27,    28,    25,    30,    31,    32,     1,
      21,     3,     4,     5,     6,     7,    15,     9,    10,    11,
      12,    13,    14,    24,    20,    20,    18,     8,    26,     0,
      75,   107,    24,    76,    26,    27,    28,    83,    30,    31,
      32,     1,    -1,     3,     4,     5,     6,     7,    -1,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    18,    -1,
      11,    12,    13,    14,    24,    -1,    26,    18,    28,    20,
      30,    31,    32,    24,    -1,    26,    -1,    28,    -1,    30,
      31,    32,    11,    12,    13,    14,    -1,    -1,    -1,    18,
      -1,    11,    12,    13,    14,    24,    -1,    26,    18,    28,
      29,    30,    31,    32,    24,    -1,    26,    -1,    28,    -1,
      30,    31,    32,    11,    12,    13,    14,    -1,    -1,    -1,
      18,    -1,    11,    12,    13,    14,    24,    -1,    26,    18,
      28,    -1,    30,    31,    32,    24,    -1,    26,    -1,    28,
      -1,    30,    31,    32
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,     9,    10,    11,
      12,    13,    14,    18,    24,    26,    28,    30,    31,    32,
      44,    45,    46,    47,    49,    50,    51,    52,    53,    54,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    70,    20,    26,    27,     4,    30,    30,    30,
      24,    20,    26,    55,    56,    55,    30,    63,    64,    30,
      48,    55,    27,    30,    31,    45,    68,    69,    29,    55,
      71,    15,     0,    46,    20,    16,    17,    33,    34,    35,
      36,    37,    38,    39,    40,    19,    23,    24,    41,    30,
      24,    19,    19,    20,    55,    27,    30,    31,    20,    20,
      24,    22,    25,    25,    21,    21,    27,    22,    27,    22,
      29,    49,    56,    58,    59,    60,    60,    61,    61,    61,
      61,    62,    63,    56,    30,    55,    72,    56,    24,    30,
      48,    55,    55,    25,    72,    30,    15,    55,    55,    69,
      55,    24,    22,    25,    48,    25,    20,    20,    26,    49,
      25,    49,    72,    55,    25,    49,    27,     8,    25,    49,
      49
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    43,    44,    45,    45,    46,    46,    46,    46,    46,
      46,    46,    46,    46,    46,    47,    47,    48,    48,    48,
      49,    49,    50,    50,    50,    50,    50,    51,    51,    52,
      52,    53,    53,    53,    54,    54,    55,    56,    56,    56,
      57,    57,    58,    58,    59,    59,    59,    60,    60,    60,
      60,    60,    61,    61,    62,    62,    63,    63,    64,    64,
      64,    64,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    66,    66,    66,    67,    67,    68,    68,
      69,    69,    70,    70,    71,    71,    72,    72,    72
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     7,     6,     3,     1,     0,
       3,     2,     5,     4,     5,     4,     3,     7,     5,     3,
       2,     3,     2,     2,     2,     1,     1,     3,     3,     1,
       3,     1,     3,     1,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     1,     3,     1,     2,     1,     6,     3,
       4,     1,     1,     1,     1,     1,     1,     1,     5,     1,
       1,     1,     3,     3,     3,     5,     3,     2,     3,     1,
       3,     3,     3,     2,     3,     1,     3,     1,     0
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


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

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

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
#line 184 "parser.y"
                 { root = make_node(N_PROGRAM); root->list = (yyvsp[0].list); (yyval.node) = root; }
#line 1688 "parser.tab.c"
    break;

  case 3: /* stmt_list: stmt_list stmt  */
#line 188 "parser.y"
                      { (yyval.list) = nl_append((yyvsp[-1].list), (yyvsp[0].node)); }
#line 1694 "parser.tab.c"
    break;

  case 4: /* stmt_list: stmt  */
#line 189 "parser.y"
                      { (yyval.list) = nl_append(NULL, (yyvsp[0].node)); }
#line 1700 "parser.tab.c"
    break;

  case 5: /* stmt: func_decl  */
#line 193 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1706 "parser.tab.c"
    break;

  case 6: /* stmt: var_decl  */
#line 194 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1712 "parser.tab.c"
    break;

  case 7: /* stmt: if_stmt  */
#line 195 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1718 "parser.tab.c"
    break;

  case 8: /* stmt: throw_stmt  */
#line 196 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1724 "parser.tab.c"
    break;

  case 9: /* stmt: return_stmt  */
#line 197 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1730 "parser.tab.c"
    break;

  case 10: /* stmt: expr_stmt  */
#line 198 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1736 "parser.tab.c"
    break;

  case 11: /* stmt: block  */
#line 199 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1742 "parser.tab.c"
    break;

  case 12: /* stmt: error SEMICOLON  */
#line 200 "parser.y"
                      { (yyval.node) = NULL; yyerrok; }
#line 1748 "parser.tab.c"
    break;

  case 13: /* stmt: error RBRACE  */
#line 201 "parser.y"
                      { (yyval.node) = NULL; yyerrok; }
#line 1754 "parser.tab.c"
    break;

  case 14: /* stmt: error LBRACE  */
#line 202 "parser.y"
                      { (yyval.node) = NULL; yyerrok; }
#line 1760 "parser.tab.c"
    break;

  case 15: /* func_decl: ASYNC FUNCTION IDENTIFIER LPAREN param_list RPAREN block  */
#line 207 "parser.y"
      {
        (yyval.node) = make_node(N_FUNC);
        (yyval.node)->sval  = (yyvsp[-4].str);
        (yyval.node)->ival  = 1; /* async */
        (yyval.node)->left  = make_node(N_BLOCK); (yyval.node)->left->list = (yyvsp[-2].list); /* params as block */
        (yyval.node)->right = (yyvsp[0].node);
      }
#line 1772 "parser.tab.c"
    break;

  case 16: /* func_decl: FUNCTION IDENTIFIER LPAREN param_list RPAREN block  */
#line 215 "parser.y"
      {
        (yyval.node) = make_node(N_FUNC);
        (yyval.node)->sval  = (yyvsp[-4].str);
        (yyval.node)->ival  = 0;
        (yyval.node)->left  = make_node(N_BLOCK); (yyval.node)->left->list = (yyvsp[-2].list);
        (yyval.node)->right = (yyvsp[0].node);
      }
#line 1784 "parser.tab.c"
    break;

  case 17: /* param_list: param_list COMMA IDENTIFIER  */
#line 225 "parser.y"
                                  { (yyval.list) = nl_append((yyvsp[-2].list), make_leaf(N_IDENT, (yyvsp[0].str))); free((yyvsp[0].str)); }
#line 1790 "parser.tab.c"
    break;

  case 18: /* param_list: IDENTIFIER  */
#line 226 "parser.y"
                                  { (yyval.list) = nl_append(NULL, make_leaf(N_IDENT, (yyvsp[0].str))); free((yyvsp[0].str)); }
#line 1796 "parser.tab.c"
    break;

  case 19: /* param_list: %empty  */
#line 227 "parser.y"
                                  { (yyval.list) = NULL; }
#line 1802 "parser.tab.c"
    break;

  case 20: /* block: LBRACE stmt_list RBRACE  */
#line 231 "parser.y"
                              { (yyval.node) = make_node(N_BLOCK); (yyval.node)->list = (yyvsp[-1].list); }
#line 1808 "parser.tab.c"
    break;

  case 21: /* block: LBRACE RBRACE  */
#line 232 "parser.y"
                              { (yyval.node) = make_node(N_BLOCK); (yyval.node)->list = NULL; }
#line 1814 "parser.tab.c"
    break;

  case 22: /* var_decl: CONST IDENTIFIER ASSIGN expr SEMICOLON  */
#line 237 "parser.y"
      { (yyval.node) = make_node(N_VAR_DECL); (yyval.node)->ival=1; (yyval.node)->sval=(yyvsp[-3].str); (yyval.node)->left=(yyvsp[-1].node); }
#line 1820 "parser.tab.c"
    break;

  case 23: /* var_decl: CONST IDENTIFIER ASSIGN expr  */
#line 239 "parser.y"
      { (yyval.node) = make_node(N_VAR_DECL); (yyval.node)->ival=1; (yyval.node)->sval=(yyvsp[-2].str); (yyval.node)->left=(yyvsp[0].node); }
#line 1826 "parser.tab.c"
    break;

  case 24: /* var_decl: LET IDENTIFIER ASSIGN expr SEMICOLON  */
#line 241 "parser.y"
      { (yyval.node) = make_node(N_VAR_DECL); (yyval.node)->ival=0; (yyval.node)->sval=(yyvsp[-3].str); (yyval.node)->left=(yyvsp[-1].node); }
#line 1832 "parser.tab.c"
    break;

  case 25: /* var_decl: LET IDENTIFIER ASSIGN expr  */
#line 243 "parser.y"
      { (yyval.node) = make_node(N_VAR_DECL); (yyval.node)->ival=0; (yyval.node)->sval=(yyvsp[-2].str); (yyval.node)->left=(yyvsp[0].node); }
#line 1838 "parser.tab.c"
    break;

  case 26: /* var_decl: LET IDENTIFIER SEMICOLON  */
#line 245 "parser.y"
      { (yyval.node) = make_node(N_VAR_DECL); (yyval.node)->ival=0; (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=NULL; }
#line 1844 "parser.tab.c"
    break;

  case 27: /* if_stmt: IF LPAREN expr RPAREN block ELSE block  */
#line 250 "parser.y"
      { (yyval.node) = make_node(N_IF); (yyval.node)->left=(yyvsp[-4].node); (yyval.node)->right=(yyvsp[-2].node); (yyval.node)->extra=(yyvsp[0].node); }
#line 1850 "parser.tab.c"
    break;

  case 28: /* if_stmt: IF LPAREN expr RPAREN block  */
#line 252 "parser.y"
      { (yyval.node) = make_node(N_IF); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); (yyval.node)->extra=NULL; }
#line 1856 "parser.tab.c"
    break;

  case 29: /* throw_stmt: THROW expr SEMICOLON  */
#line 256 "parser.y"
                           { (yyval.node) = make_node(N_THROW); (yyval.node)->left=(yyvsp[-1].node); }
#line 1862 "parser.tab.c"
    break;

  case 30: /* throw_stmt: THROW expr  */
#line 257 "parser.y"
                           { (yyval.node) = make_node(N_THROW); (yyval.node)->left=(yyvsp[0].node); }
#line 1868 "parser.tab.c"
    break;

  case 31: /* return_stmt: RETURN expr SEMICOLON  */
#line 261 "parser.y"
                            { (yyval.node) = make_node(N_RETURN); (yyval.node)->left=(yyvsp[-1].node); }
#line 1874 "parser.tab.c"
    break;

  case 32: /* return_stmt: RETURN expr  */
#line 262 "parser.y"
                            { (yyval.node) = make_node(N_RETURN); (yyval.node)->left=(yyvsp[0].node); }
#line 1880 "parser.tab.c"
    break;

  case 33: /* return_stmt: RETURN SEMICOLON  */
#line 263 "parser.y"
                            { (yyval.node) = make_node(N_RETURN); (yyval.node)->left=NULL; }
#line 1886 "parser.tab.c"
    break;

  case 34: /* expr_stmt: assign_expr SEMICOLON  */
#line 267 "parser.y"
                            { (yyval.node) = make_node(N_EXPR_STMT); (yyval.node)->left=(yyvsp[-1].node); }
#line 1892 "parser.tab.c"
    break;

  case 35: /* expr_stmt: assign_expr  */
#line 268 "parser.y"
                            { (yyval.node) = make_node(N_EXPR_STMT); (yyval.node)->left=(yyvsp[0].node); }
#line 1898 "parser.tab.c"
    break;

  case 36: /* expr: assign_expr  */
#line 273 "parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 1904 "parser.tab.c"
    break;

  case 37: /* assign_expr: postfix_expr ASSIGN_OP assign_expr  */
#line 277 "parser.y"
      { (yyval.node) = make_node(N_ASSIGN); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1910 "parser.tab.c"
    break;

  case 38: /* assign_expr: postfix_expr ASSIGN assign_expr  */
#line 279 "parser.y"
      { (yyval.node) = make_node(N_ASSIGN); (yyval.node)->sval=strdup("="); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1916 "parser.tab.c"
    break;

  case 39: /* assign_expr: or_expr  */
#line 280 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1922 "parser.tab.c"
    break;

  case 40: /* or_expr: or_expr OR and_expr  */
#line 285 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=strdup("||"); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1928 "parser.tab.c"
    break;

  case 41: /* or_expr: and_expr  */
#line 286 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1934 "parser.tab.c"
    break;

  case 42: /* and_expr: and_expr AND eq_expr  */
#line 291 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=strdup("&&"); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1940 "parser.tab.c"
    break;

  case 43: /* and_expr: eq_expr  */
#line 292 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1946 "parser.tab.c"
    break;

  case 44: /* eq_expr: eq_expr EQ rel_expr  */
#line 297 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1952 "parser.tab.c"
    break;

  case 45: /* eq_expr: eq_expr NEQ rel_expr  */
#line 299 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1958 "parser.tab.c"
    break;

  case 46: /* eq_expr: rel_expr  */
#line 300 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1964 "parser.tab.c"
    break;

  case 47: /* rel_expr: rel_expr LT add_expr  */
#line 305 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1970 "parser.tab.c"
    break;

  case 48: /* rel_expr: rel_expr GT add_expr  */
#line 307 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1976 "parser.tab.c"
    break;

  case 49: /* rel_expr: rel_expr LE add_expr  */
#line 309 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1982 "parser.tab.c"
    break;

  case 50: /* rel_expr: rel_expr GE add_expr  */
#line 311 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 1988 "parser.tab.c"
    break;

  case 51: /* rel_expr: add_expr  */
#line 312 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1994 "parser.tab.c"
    break;

  case 52: /* add_expr: add_expr ADDOP mul_expr  */
#line 317 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 2000 "parser.tab.c"
    break;

  case 53: /* add_expr: mul_expr  */
#line 318 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 2006 "parser.tab.c"
    break;

  case 54: /* mul_expr: mul_expr MULOP unary_expr  */
#line 323 "parser.y"
      { (yyval.node) = make_node(N_BINARY); (yyval.node)->sval=(yyvsp[-1].str); (yyval.node)->left=(yyvsp[-2].node); (yyval.node)->right=(yyvsp[0].node); }
#line 2012 "parser.tab.c"
    break;

  case 55: /* mul_expr: unary_expr  */
#line 324 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 2018 "parser.tab.c"
    break;

  case 56: /* unary_expr: NOT unary_expr  */
#line 329 "parser.y"
      { (yyval.node) = make_node(N_UNARY); (yyval.node)->sval=strdup("!"); (yyval.node)->left=(yyvsp[0].node); }
#line 2024 "parser.tab.c"
    break;

  case 57: /* unary_expr: postfix_expr  */
#line 330 "parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 2030 "parser.tab.c"
    break;

  case 58: /* postfix_expr: postfix_expr DOT IDENTIFIER LPAREN arg_list RPAREN  */
#line 335 "parser.y"
      {
        /* method call: obj.method(args) */
        Node *mem = make_node(N_MEMBER);
        mem->left  = (yyvsp[-5].node);
        mem->right = make_leaf(N_IDENT, (yyvsp[-3].str)); free((yyvsp[-3].str));
        (yyval.node) = make_node(N_CALL);
        (yyval.node)->left = mem;
        (yyval.node)->list = (yyvsp[-1].list);
      }
#line 2044 "parser.tab.c"
    break;

  case 59: /* postfix_expr: postfix_expr DOT IDENTIFIER  */
#line 345 "parser.y"
      {
        (yyval.node) = make_node(N_MEMBER);
        (yyval.node)->left  = (yyvsp[-2].node);
        (yyval.node)->right = make_leaf(N_IDENT, (yyvsp[0].str)); free((yyvsp[0].str));
      }
#line 2054 "parser.tab.c"
    break;

  case 60: /* postfix_expr: postfix_expr LPAREN arg_list RPAREN  */
#line 351 "parser.y"
      {
        (yyval.node) = make_node(N_CALL);
        (yyval.node)->left = (yyvsp[-3].node);
        (yyval.node)->list = (yyvsp[-1].list);
      }
#line 2064 "parser.tab.c"
    break;

  case 61: /* postfix_expr: primary_expr  */
#line 356 "parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 2070 "parser.tab.c"
    break;

  case 62: /* primary_expr: IDENTIFIER  */
#line 360 "parser.y"
                      { (yyval.node) = make_leaf(N_IDENT,  (yyvsp[0].str)); free((yyvsp[0].str)); }
#line 2076 "parser.tab.c"
    break;

  case 63: /* primary_expr: STRING_LIT  */
#line 361 "parser.y"
                      { (yyval.node) = make_leaf(N_STR,    (yyvsp[0].str)); free((yyvsp[0].str)); }
#line 2082 "parser.tab.c"
    break;

  case 64: /* primary_expr: NUMBER_LIT  */
#line 362 "parser.y"
                      { (yyval.node) = make_leaf(N_NUM,    (yyvsp[0].str)); free((yyvsp[0].str)); }
#line 2088 "parser.tab.c"
    break;

  case 65: /* primary_expr: TRUE_LIT  */
#line 363 "parser.y"
                      { (yyval.node) = make_node(N_BOOL); (yyval.node)->ival=1; (yyval.node)->sval=strdup("true"); }
#line 2094 "parser.tab.c"
    break;

  case 66: /* primary_expr: FALSE_LIT  */
#line 364 "parser.y"
                      { (yyval.node) = make_node(N_BOOL); (yyval.node)->ival=0; (yyval.node)->sval=strdup("false"); }
#line 2100 "parser.tab.c"
    break;

  case 67: /* primary_expr: NULL_LIT  */
#line 365 "parser.y"
                      { (yyval.node) = make_node(N_NULL); (yyval.node)->sval=strdup("null"); }
#line 2106 "parser.tab.c"
    break;

  case 68: /* primary_expr: NEW IDENTIFIER LPAREN arg_list RPAREN  */
#line 367 "parser.y"
      { (yyval.node) = make_node(N_NEW); (yyval.node)->sval=(yyvsp[-3].str); (yyval.node)->list=(yyvsp[-1].list); }
#line 2112 "parser.tab.c"
    break;

  case 69: /* primary_expr: object_literal  */
#line 368 "parser.y"
                      { (yyval.node) = (yyvsp[0].node); }
#line 2118 "parser.tab.c"
    break;

  case 70: /* primary_expr: array_literal  */
#line 369 "parser.y"
                      { (yyval.node) = (yyvsp[0].node); }
#line 2124 "parser.tab.c"
    break;

  case 71: /* primary_expr: arrow_func  */
#line 370 "parser.y"
                      { (yyval.node) = (yyvsp[0].node); }
#line 2130 "parser.tab.c"
    break;

  case 72: /* primary_expr: LPAREN expr RPAREN  */
#line 371 "parser.y"
                         { (yyval.node) = (yyvsp[-1].node); }
#line 2136 "parser.tab.c"
    break;

  case 73: /* arrow_func: IDENTIFIER ARROW assign_expr  */
#line 376 "parser.y"
      { (yyval.node) = make_node(N_ARROW); (yyval.node)->left=make_leaf(N_IDENT,(yyvsp[-2].str)); free((yyvsp[-2].str)); (yyval.node)->right=(yyvsp[0].node); }
#line 2142 "parser.tab.c"
    break;

  case 74: /* arrow_func: IDENTIFIER ARROW block  */
#line 378 "parser.y"
      { (yyval.node) = make_node(N_ARROW); (yyval.node)->left=make_leaf(N_IDENT,(yyvsp[-2].str)); free((yyvsp[-2].str)); (yyval.node)->right=(yyvsp[0].node); }
#line 2148 "parser.tab.c"
    break;

  case 75: /* arrow_func: LPAREN param_list RPAREN ARROW block  */
#line 380 "parser.y"
      { (yyval.node) = make_node(N_ARROW);
        (yyval.node)->left = make_node(N_BLOCK); (yyval.node)->left->list=(yyvsp[-3].list);
        (yyval.node)->right=(yyvsp[0].node); }
#line 2156 "parser.tab.c"
    break;

  case 76: /* object_literal: LBRACE prop_list RBRACE  */
#line 386 "parser.y"
                              { (yyval.node) = make_node(N_OBJECT); (yyval.node)->list=(yyvsp[-1].list); }
#line 2162 "parser.tab.c"
    break;

  case 77: /* object_literal: LBRACE RBRACE  */
#line 387 "parser.y"
                              { (yyval.node) = make_node(N_OBJECT); (yyval.node)->list=NULL; }
#line 2168 "parser.tab.c"
    break;

  case 78: /* prop_list: prop_list COMMA prop  */
#line 391 "parser.y"
                           { (yyval.list) = nl_append((yyvsp[-2].list), (yyvsp[0].node)); }
#line 2174 "parser.tab.c"
    break;

  case 79: /* prop_list: prop  */
#line 392 "parser.y"
                           { (yyval.list) = nl_append(NULL, (yyvsp[0].node)); }
#line 2180 "parser.tab.c"
    break;

  case 80: /* prop: IDENTIFIER COLON expr  */
#line 397 "parser.y"
      { (yyval.node) = make_node(N_PROP); (yyval.node)->sval=(yyvsp[-2].str); (yyval.node)->left=(yyvsp[0].node); }
#line 2186 "parser.tab.c"
    break;

  case 81: /* prop: STRING_LIT COLON expr  */
#line 399 "parser.y"
      { (yyval.node) = make_node(N_PROP); (yyval.node)->sval=(yyvsp[-2].str); (yyval.node)->left=(yyvsp[0].node); }
#line 2192 "parser.tab.c"
    break;

  case 82: /* array_literal: LBRACKET elem_list RBRACKET  */
#line 403 "parser.y"
                                  { (yyval.node) = make_node(N_ARRAY); (yyval.node)->list=(yyvsp[-1].list); }
#line 2198 "parser.tab.c"
    break;

  case 83: /* array_literal: LBRACKET RBRACKET  */
#line 404 "parser.y"
                                  { (yyval.node) = make_node(N_ARRAY); (yyval.node)->list=NULL; }
#line 2204 "parser.tab.c"
    break;

  case 84: /* elem_list: elem_list COMMA expr  */
#line 408 "parser.y"
                           { (yyval.list) = nl_append((yyvsp[-2].list), (yyvsp[0].node)); }
#line 2210 "parser.tab.c"
    break;

  case 85: /* elem_list: expr  */
#line 409 "parser.y"
                           { (yyval.list) = nl_append(NULL, (yyvsp[0].node)); }
#line 2216 "parser.tab.c"
    break;

  case 86: /* arg_list: arg_list COMMA expr  */
#line 413 "parser.y"
                          { (yyval.list) = nl_append((yyvsp[-2].list), (yyvsp[0].node)); }
#line 2222 "parser.tab.c"
    break;

  case 87: /* arg_list: expr  */
#line 414 "parser.y"
                          { (yyval.list) = nl_append(NULL, (yyvsp[0].node)); }
#line 2228 "parser.tab.c"
    break;

  case 88: /* arg_list: %empty  */
#line 415 "parser.y"
                          { (yyval.list) = NULL; }
#line 2234 "parser.tab.c"
    break;


#line 2238 "parser.tab.c"

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
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
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
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 418 "parser.y"


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

            /* Semantic check: function must have params to infer types */
            if (!s->left || !s->left->list)
                sem_error(0, "function '%s': no parameters — cannot infer types", s->sval);

            /* Semantic check: return type must be resolvable */
            if (strcmp(ret, "Promise<unknown>") == 0)
                sem_error(0, "function '%s': return type unknown — no 'const new* = {...}' found in body", s->sval);

            snprintf(sig, 127, "async function(...): %s", ret);
            var_set(s->sval, sig, "global");
        }
        if (s->kind == N_VAR_DECL && s->left && s->left->kind == N_ARRAY) {
            var_set(s->sval, "__let_array__", "global");
            if (s->left->list && s->left->list->node &&
                s->left->list->node->kind == N_OBJECT) {
                char elem[64]; strncpy(elem, s->sval, 63);
                int elen = strlen(elem);
                if (elen>1 && elem[elen-1]=='s') elem[elen-1]='\0';
                pascal(elem, elem, 64);
                register_object_type(elem, s->left->list->node);
            }
        }
    }

    /* Semantic check: warn about unresolved property types */
    for (int i = 0; i < ntype; i++)
        for (int j = 0; j < types[i].nprops; j++)
            if (strcmp(types[i].props[j].type, "unknown") == 0)
                sem_error(0, "type '%s': property '%s' type could not be inferred",
                          types[i].name, types[i].props[j].key);
}

/* ════════════════════════════════════════════════════════
   SYMBOL TABLE PRINT
   ════════════════════════════════════════════════════════ */

/* PARSE TRACE */
static void trace_node(Node *n, FILE *f, int depth) {
    if (!n) return;
    for (int i=0;i<depth;i++) fprintf(f,"  ");
    switch (n->kind) {
    case N_PROGRAM:
        fprintf(f,"[Program]\n");
        for (NodeList *l=n->list;l;l=l->next) trace_node(l->node,f,depth+1);
        break;
    case N_FUNC:
        fprintf(f,"[%sFunction] %s\n", n->ival?"Async ":"", n->sval);
        for (int i=0;i<depth+1;i++) fprintf(f,"  "); fprintf(f,"[Params]\n");
        if (n->left) for (NodeList *l=n->left->list;l;l=l->next) trace_node(l->node,f,depth+2);
        for (int i=0;i<depth+1;i++) fprintf(f,"  "); fprintf(f,"[Body]\n");
        if (n->right) for (NodeList *l=n->right->list;l;l=l->next) trace_node(l->node,f,depth+2);
        break;
    case N_VAR_DECL:
        fprintf(f,"[VarDecl] %s %s\n", n->ival?"const":"let", n->sval);
        trace_node(n->left,f,depth+1);
        break;
    case N_IF:
        fprintf(f,"[IfStmt]\n");
        for (int i=0;i<depth+1;i++) fprintf(f,"  "); fprintf(f,"[Condition]\n");
        trace_node(n->left,f,depth+2);
        for (int i=0;i<depth+1;i++) fprintf(f,"  "); fprintf(f,"[Then]\n");
        if (n->right) for (NodeList *l=n->right->list;l;l=l->next) trace_node(l->node,f,depth+2);
        if (n->extra) {
            for (int i=0;i<depth+1;i++) fprintf(f,"  "); fprintf(f,"[Else]\n");
            for (NodeList *l=n->extra->list;l;l=l->next) trace_node(l->node,f,depth+2);
        }
        break;
    case N_THROW:
        fprintf(f,"[ThrowStmt]\n"); trace_node(n->left,f,depth+1); break;
    case N_RETURN:
        fprintf(f,"[ReturnStmt]\n"); trace_node(n->left,f,depth+1); break;
    case N_EXPR_STMT:
        fprintf(f,"[ExprStmt]\n"); trace_node(n->left,f,depth+1); break;
    case N_CALL:
        fprintf(f,"[Call]\n");
        for (int i=0;i<depth+1;i++) fprintf(f,"  "); fprintf(f,"[Callee]\n");
        trace_node(n->left,f,depth+2);
        if (n->list) {
            for (int i=0;i<depth+1;i++) fprintf(f,"  "); fprintf(f,"[Args]\n");
            for (NodeList *l=n->list;l;l=l->next) trace_node(l->node,f,depth+2);
        }
        break;
    case N_MEMBER:
        fprintf(f,"[Member] ");
        if (n->left&&n->left->kind==N_IDENT) fprintf(f,"%s",n->left->sval);
        fprintf(f,".");
        if (n->right&&n->right->kind==N_IDENT) fprintf(f,"%s",n->right->sval);
        fprintf(f,"\n"); break;
    case N_BINARY:
        fprintf(f,"[Binary] %s\n",n->sval);
        trace_node(n->left,f,depth+1); trace_node(n->right,f,depth+1); break;
    case N_UNARY:
        fprintf(f,"[Unary] %s\n",n->sval); trace_node(n->left,f,depth+1); break;
    case N_ASSIGN:
        fprintf(f,"[Assign] %s\n",n->sval);
        trace_node(n->left,f,depth+1); trace_node(n->right,f,depth+1); break;
    case N_OBJECT:
        fprintf(f,"[ObjectLiteral]\n");
        for (NodeList *l=n->list;l;l=l->next) trace_node(l->node,f,depth+1); break;
    case N_PROP:
        fprintf(f,"[Prop] %s:\n",n->sval); trace_node(n->left,f,depth+1); break;
    case N_ARRAY:
        fprintf(f,"[ArrayLiteral]\n");
        for (NodeList *l=n->list;l;l=l->next) trace_node(l->node,f,depth+1); break;
    case N_ARROW:
        fprintf(f,"[ArrowFunc]\n");
        for (int i=0;i<depth+1;i++) fprintf(f,"  "); fprintf(f,"[Param]\n");
        trace_node(n->left,f,depth+2);
        for (int i=0;i<depth+1;i++) fprintf(f,"  "); fprintf(f,"[Body]\n");
        trace_node(n->right,f,depth+2); break;
    case N_NEW:  fprintf(f,"[New] %s\n",n->sval); break;
    case N_IDENT: fprintf(f,"[Ident] %s\n",n->sval); break;
    case N_STR:   fprintf(f,"[String] %s\n",n->sval); break;
    case N_NUM:   fprintf(f,"[Number] %s\n",n->sval); break;
    case N_BOOL:  fprintf(f,"[Bool] %s\n",n->sval); break;
    case N_NULL:  fprintf(f,"[Null]\n"); break;
    default: break;
    }
}

static void write_trace(FILE *f, const char *input_name) {
    fprintf(f,"JS -> TS Compiler  Parse Trace\n");
    fprintf(f,"Input : %s\n", input_name);
    fprintf(f,"======================================================\n\n");
    fprintf(f,"-- PHASE 1: Lexical Analysis --------------------------\n");
    fprintf(f,"   Tokens recognised by flex rules in lexer.l\n\n");
    fprintf(f,"-- PHASE 2: Syntax Analysis  (AST) --------------------\n");
    fprintf(f,"   LALR(1) parser reduces token stream to AST nodes.\n\n");
    trace_node(root, f, 2);
    fprintf(f,"\n-- PHASE 3: Semantic Analysis -------------------------\n");
    fprintf(f,"   %-16s  %-30s  %s\n","Identifier","Inferred Type","Scope");
    fprintf(f,"   %-16s  %-30s  %s\n","----------------","------------------------------","------");
    for (int i=0;i<nvar;i++)
        fprintf(f,"   %-16s  %-30s  %s\n",vars[i].name,vars[i].type,vars[i].scope);
    fprintf(f,"\n-- PHASE 4: Code Generation ---------------------------\n");
    fprintf(f,"   TypeScript emitted by walking AST with inferred types.\n");
    fprintf(f,"   Output written to output.ts\n");
}

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
    if (nerrors < MAX_ERRORS) {
        errors[nerrors].line = yylineno;
        snprintf(errors[nerrors].msg, 127, "%s", s);
        nerrors++;
    }
    fprintf(stderr, "  ❌ line %d: %s\n", yylineno, s);
}


int main(int argc, char *argv[]) {
    /* optional: ./js2ts tracefile.txt < input.js */
    const char *trace_name = (argc > 1) ? argv[1] : NULL;
    const char *input_label = (argc > 2) ? argv[2] : "stdin";

    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║         JS → TS Compiler  (Lex + Bison)              ║\n");
    printf("║   Phases: Lexical | Syntax | Semantic | CodeGen      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── PHASE 1 & 2: Lexical + Syntax Analysis ─────────────\n");
    yyparse();

    if (nerrors > 0) {
        printf("\n╔══════════════════════════════════════════════════════╗\n");
        printf("║                  SYNTAX ERRORS FOUND                ║\n");
        printf("╠══════╦═══════════════════════════════════════════════╣\n");
        printf("║ Line ║ Error                                         ║\n");
        printf("╠══════╬═══════════════════════════════════════════════╣\n");
        for (int i = 0; i < nerrors; i++)
            printf("║ %-4d ║ %-45s ║\n", errors[i].line, errors[i].msg);
        printf("╚══════╩═══════════════════════════════════════════════╝\n");
        printf("\n❌ Compilation aborted — %d error(s) found. No output produced.\n", nerrors);
        return 1;
    }

    printf("  ✔ Parse successful.\n");

    printf("\n── PHASE 3: Semantic Analysis ─────────────────────────\n");
    infer_program(root);
    print_symtable();
    print_errors();

    printf("\n── PHASE 4: Code Generation ───────────────────────────\n");
    FILE *out = fopen("output.ts", "w");
    if (!out) { perror("output.ts"); return 1; }
    emit_program(root, out);
    fclose(out);
    printf("  ✔ TypeScript written to output.ts\n");

    if (trace_name) {
        FILE *tf = fopen(trace_name, "w");
        if (tf) {
            write_trace(tf, input_label);
            fclose(tf);
            printf("  ✔ Parse trace written to %s\n", trace_name);
        } else {
            perror(trace_name);
        }
    }

    ast_free(root);
    printf("\n✅ Compilation successful.\n");
    return 0;
}
