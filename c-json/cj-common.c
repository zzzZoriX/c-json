
#include "c-json.h"

CJsonError cjo_realloc(CJsonObject* cjo){
    cjo->capacity *= _CJ_CAP_FACTOR_;

    cjo->pairs = (CJsonPair*)realloc(cjo->pairs, sizeof(CJsonPair) * cjo->capacity);
    if(!cjo->pairs)
        return CJE_ERR_OOM;

    return CJE_OK;
}

CJsonError cjo_create(CJsonObject** cjo){
    *cjo = (CJsonObject*)malloc(sizeof(CJsonObject));
    if(!*cjo)
        return CJE_ERR_OOM;

    (*cjo)->pairs = (CJsonPair*)calloc(_CJ_DEFAULT_OBJ_CAP_VALUE_, sizeof(CJsonPair));
    if(!(*cjo)->pairs)
            return CJE_ERR_OOM;

    (*cjo)->size = 0;
    (*cjo)->capacity = _CJ_DEFAULT_OBJ_CAP_VALUE_;

    return CJE_OK;
}

CJsonError cjv_create(CJsonValue** cjv){
    *cjv = (CJsonValue*)malloc(sizeof(CJsonValue));
    if(!*cjv)
        return CJE_ERR_OOM;

    (*cjv)->value_type = CJ_NULL;
    
    (*cjv)->cj_bool_value = false;
    (*cjv)->cj_number_value = 0;
    (*cjv)->cj_string_value = NULL;
    (*cjv)->cj_object_value = NULL;
    (*cjv)->cj_array_value = NULL;

    return CJE_OK;
}

CJsonError cja_create(CJsonArray** cja){
    *cja = (CJsonArray*)malloc(sizeof(CJsonArray));
    if(!*cja)
        return CJE_ERR_OOM;

    (*cja)->size = 0;
    (*cja)->capacity = _CJ_DEFAULT_ARR_CAP_VALUE_;

    (*cja)->data = (CJsonValue*)calloc(_CJ_DEFAULT_ARR_CAP_VALUE_, sizeof(CJsonValue));
    if(!(*cja)->data)
        return CJE_ERR_OOM;

    memset((*cja)->data, 0, sizeof(CJsonValue) * _CJ_DEFAULT_ARR_CAP_VALUE_);
    
    return CJE_OK;
}

CJsonError cja_realloc(CJsonArray* cja){
    cja->capacity *= _CJ_CAP_FACTOR_;

    cja->data = (CJsonValue*)realloc(cja->data, sizeof(CJsonValue) * cja->capacity);
    if(!cja->data)
        return CJE_ERR_OOM;

    return CJE_OK;
}

CJsonType cj_def_jsonv_type(const string s){
    if(s[0] == '"')     return CJ_STRING;
    if(isdigit(s[0]))   return CJ_NUMBER;
    if(!strcmp(s, "true") || !strcmp(s, "false"))   return CJ_BOOL;
    if(s[0] == '{')     return CJ_OBJECT;
    if(s[0] == '[')     return CJ_ARRAY;

    return CJ_NULL;
}

bool cj_is_valid_strv(const string s, CJsonError* e){
    return s[0] == '"' && s[strlen(s) - 1] == '"';
}

bool cj_is_valid_numv(const string s, CJsonError* e){
    if((s[0] != '-' && !isdigit(s[0])) && s[0] == '.')
        return false;

    bool has_dot = false;
    
    for(int i = 1; i < strlen(s); ++i){
        if((s[i] == '.' && has_dot) || !isdigit(s[i])){
            *e = CJE_ERR_NUMBER_FORMAT;
            return false;
        }
        else if(s[i] == '.')
            has_dot = true;
    }

    return true;
}

bool cj_is_valid_boolv(const string s, CJsonError* e){
    if(strcmp(s, "true") != 0 && strcmp(s, "false") != 0){
        *e = CJE_ERR_UNEXPECTED_TOKEN;
        return false;
    }

    return true;
}

string concat(const string s1, const string s2){
    string res = (string)malloc(sizeof(s1) + sizeof(s2) + 1);

    strcpy(res, s1);
    strcat(res, s2);

    return res;
}

string concat_c(const string s, const char c){
    string res = (string)malloc(sizeof(s) + 2);

    strcpy(res, s);

    res[sizeof(res) - 2] = c;
    res[sizeof(res) - 1] = '\0';

    return res;
}