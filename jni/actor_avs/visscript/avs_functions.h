#ifndef _AVS_FUNCTIONS_H
#define _AVS_FUNCTIONS_H 1

enum _AvsBuiltinFunctionType;
typedef enum _AvsBuiltinFunctionType AvsBuiltinFunctionType;
struct _AvsBuiltinFunctionToken;
typedef struct _AvsBuiltinFunctionToken AvsBuiltinFunctionToken;

enum _AvsBuiltinFunctionType {
	AVS_BUILTIN_FUNCTION_ABS,
	AVS_BUILTIN_FUNCTION_SIN,
	AVS_BUILTIN_FUNCTION_COS,
	AVS_BUILTIN_FUNCTION_TAN,
	AVS_BUILTIN_FUNCTION_ASIN,
	AVS_BUILTIN_FUNCTION_ACOS,
	AVS_BUILTIN_FUNCTION_ATAN,
	AVS_BUILTIN_FUNCTION_ATAN2,
	AVS_BUILTIN_FUNCTION_SQR,
	AVS_BUILTIN_FUNCTION_SQRT,
	AVS_BUILTIN_FUNCTION_INVSQRT,
	AVS_BUILTIN_FUNCTION_POW,
	AVS_BUILTIN_FUNCTION_EXP,
	AVS_BUILTIN_FUNCTION_LOG,
	AVS_BUILTIN_FUNCTION_LOG10,
	AVS_BUILTIN_FUNCTION_FLOOR,
	AVS_BUILTIN_FUNCTION_CEIL,
	AVS_BUILTIN_FUNCTION_SIGN,
	AVS_BUILTIN_FUNCTION_MIN,
	AVS_BUILTIN_FUNCTION_MAX,
	AVS_BUILTIN_FUNCTION_SIGMOID,
	AVS_BUILTIN_FUNCTION_RAND,
	AVS_BUILTIN_FUNCTION_BAND,
	AVS_BUILTIN_FUNCTION_BOR,
	AVS_BUILTIN_FUNCTION_BNOT,
	AVS_BUILTIN_FUNCTION_EQUAL,
	AVS_BUILTIN_FUNCTION_ABOVE,
	AVS_BUILTIN_FUNCTION_BELOW,
    AVS_BUILTIN_FUNCTION_GETOSC,
    AVS_BUILTIN_FUNCTION_GETSPEC,
    AVS_BUILTIN_FUNCTION_GETTIME,
    AVS_BUILTIN_FUNCTION_GETKBMOUSE,
	AVS_BUILTIN_FUNCTION_IF,
	AVS_BUILTIN_FUNCTION_ASSIGN,
	AVS_BUILTIN_FUNCTION_EXEC2,
	AVS_BUILTIN_FUNCTION_EXEC3,
	AVS_BUILTIN_FUNCTION_LOOP,
};

struct _AvsBuiltinFunctionToken {
	char			*name;
	AvsBuiltinFunctionType	lookup;
};


/* prototypes */
AvsBuiltinFunctionType avs_builtin_function_type(char *name);
AvsRunnableFunction *avs_builtin_function_lookup(AvsBuiltinFunctionType lookup);
AvsRunnableFunction *avs_builtin_function_find(char *name);

#endif /* !_AVS_FUNCTIONS_H */