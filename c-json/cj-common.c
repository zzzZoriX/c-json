
#include "c-json.h"

CJsonError cjo_realloc(CJsonObject* cjo){
    if(cjo->size == cjo->capacity)
        cjo->capacity *= _CJ_CAP_FACTOR_;

    cjo->pairs = (CJsonPair*)realloc(cjo->pairs, sizeof(CJsonPair) * cjo->size);
    if(!cjo->pairs)
        return CJE_ERR_OOM;

    return CJE_OK;
}

CJsonError cjo_create(CJsonObject* cjo){
    cjo = (CJsonObject*)malloc(sizeof(CJsonObject));
    if(!cjo)
        return CJE_ERR_OOM;

    cjo->pairs = (CJsonPair*)calloc(_CJ_DEFAULT_OBJ_CAP_VALUE_, sizeof(CJsonPair));
    if(!cjo->pairs)
            return CJE_ERR_OOM;

    cjo->size = 0;
    cjo->capacity = _CJ_DEFAULT_OBJ_CAP_VALUE_;

    return CJE_OK;
}

CJsonError cjv_create(CJsonValue* cjv){
    cjv = (CJsonValue*)malloc(sizeof(CJsonValue));
    if(!cjv)
        return CJE_ERR_OOM;

    cjv->value_type = CJ_NULL;
    
    cjv->cj_bool_value = false;
    cjv->cj_number_value = 0;
    cjv->cj_string_value = NULL;
    cjv->cj_object_value = NULL;
    cjv->cj_array_value = NULL;

    return CJE_OK;
}

CJsonError cja_create(CJsonArray* cja){
    cja = (CJsonArray*)malloc(sizeof(CJsonArray));
    if(!cja)
        return CJE_ERR_OOM;

    cja->size = 0;
    cja->capacity = _CJ_DEFAULT_ARR_CAP_VALUE_;

    cja->data = (CJsonValue*)calloc(_CJ_DEFAULT_ARR_CAP_VALUE_, sizeof(CJsonValue));
    if(!cja->data)
        return CJE_ERR_OOM;

    memset(cja->data, 0, sizeof(CJsonValue) * _CJ_DEFAULT_ARR_CAP_VALUE_);
    
    return CJE_OK;
}

CJsonError cja_realloc(CJsonArray* cja){
    if(cja->size == cja->capacity)
        cja->capacity *= _CJ_CAP_FACTOR_;

    cja->data = (CJsonValue*)realloc(cja->data, sizeof(CJsonValue) * cja->capacity);
    if(!cja->data)
        return CJE_ERR_OOM;

    return CJE_OK;
}