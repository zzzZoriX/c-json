#ifndef _CJSONH_
#define _CJSONH_

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef char*   string;
typedef char*   key_t;

typedef enum {
    CJ_NULL,
    CJ_BOOL,
    CJ_NUMBER,
    CJ_STRING,
    CJ_ARRAY,
    CJ_OBJECT
} CJsonType;

typedef struct CJsonArray CJsonArray;
typedef struct CJsonObject CJsonObject;

typedef struct CJsonValue {
    CJsonType type;

    union {
        double          cj_int_value;
        bool            cj_bool_value;
        string          cj_string_value;
        CJsonArray*     cj_array_value;
        CJsonObject*    cj_objectt_value;
    };
} CJsonValue;

typedef struct CJsonArray {
    size_t  size,
            capacity;

    CJsonValue* data;
} CJsonArray;

typedef struct {
    key_t ket;
    CJsonValue value;
} CJsonPair;

typedef struct CJsonObject {
    size_t  size,
            capacity;

    CJsonPair* pairs;
} CJsonObject;

#ifdef __cplusplus
}
#endif

#endif // _CJSONH_