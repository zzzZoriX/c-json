
#ifndef _CJSONH_
#define _CJSONH_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#define _CJ_DEFAULT_OBJ_CAP_VALUE_  1
#define _CJ_DEFAULT_ARR_CAP_VALUE_  2

#define _CJ_CAP_FACTOR_             2

#ifdef __cpluplus
extern "C" {
#endif

typedef char*   string;
typedef double  number;
typedef char*   key_t;
typedef size_t  pos;

typedef enum {
    CJ_NULL,
    CJ_STRING,
    CJ_NUMBER,
    CJ_BOOL,
    CJ_ARRAY,
    CJ_OBJECT
} CJsonType;

typedef enum {
    CJE_OK,
    CJE_ERR_SYNTAX,
    CJE_ERR_UNEXPECTED_TOKEN,
    CJE_ERR_OOM,
    CJE_ERR_INVALID_ESCAPE,
    CJE_ERR_NUMBER_FORMAT,
    CJE_ERR_EOF,
    CJE_ERR_DEPTH_LIM,
    CJE_ERR_UTF8
} CJsonError;

typedef struct CJsonArray CJsonArray;
typedef struct CJsonObject CJsonObject;

typedef struct CJsonValue {
    CJsonType value_type;

    union {
        bool cj_bool_value;
        number cj_number_value;
        string cj_string_value;
        CJsonArray* cj_array_value;
        CJsonObject* cj_object_value;
    };
} CJsonValue;

typedef struct {
    key_t key;
    CJsonValue* value;
} CJsonPair;

typedef struct CJsonArray {
    size_t  size,
            capacity;

    CJsonValue* data;
} CJsonArray;

typedef struct CJsonObject {
    size_t  size,
            capacity;

    CJsonPair* pairs;
} CJsonObject;

CJsonValue* cjson_parse(
        const string,   // json data
        const size_t,   // length of json data string
        CJsonError*,    // error code ref
        pos*            // error position
    );

CJsonValue* cjson_get(
        CJsonValue*,    // parsed json data
        key_t           // by that key the function will search an object  
    );
string cjson_get_string(
        CJsonValue*,    // parsed json data
        key_t           // by that key the function will search an object  
    );
number cjson_get_number(
        CJsonValue*,    // parsed json data
        key_t           // by that key the function will search an object  
    );
bool cjson_get_bool(
        CJsonValue*,    // parsed json data
        key_t           // by that key the function will search an object  
    );

size_t cjson_array_get_size(
        CJsonValue*     // object from which will be taked an array
    );
CJsonValue* cjson_array_get(
        CJsonValue*,    // object from which will be taked an array
        size_t          // index of taken object
    );


#ifdef __cpluplus
}
#endif

#endif // _CJSONH_