#include "c-json.h"

extern CJsonError cjo_create(CJsonObject* cjo);
extern CJsonError cjo_realloc(CJsonObject* cjo);
extern CJsonError cjv_create(CJsonValue* cjv);
extern CJsonError cja_create(CJsonArray* cja);
extern CJsonError cja_realloc(CJsonArray* cja);

bool cj_is_valid_boolv(const string s, CJsonError* e);
bool cj_is_valid_numv(const string s, CJsonError* e);
bool cj_is_valid_strv(const string s, CJsonError* e);
CJsonType cj_def_jsonv_type(const string s);

CJsonValue* cj_parse_value(const string* tokens, size_t tc, size_t* cti, CJsonError* cje);

/**
 * @param tc    tokens count
 * @param cti   current token index
 * @param cje   c-json error
 */
CJsonObject* cj_parse_object(const string* tokens, size_t tc, size_t* cti, CJsonError* cje){
    ++*cti; // skip {

    CJsonObject* cjo;
    if((*cje = cjo_create(cjo)) != CJE_OK)
        return NULL;

    while(tokens[*cti][0] != '}' && *cti <= tc){
        if(cj_def_jsonv_type(tokens[*cti]) != CJ_STRING){
            *cje = CJE_ERR_UNEXPECTED_TOKEN;

            free(cjo->pairs);
            free(cjo);

            return NULL;
        }

        // key
        string key = _strdup(tokens[*cti]);
        if(!key){
            *cje = CJE_ERR_OOM;

            free(cjo->pairs);
            free(cjo);

            return NULL;
        }
        
        if(tokens[*cti + 1][0] != ':'){
            *cje = CJE_ERR_UNEXPECTED_TOKEN;

            free(cjo->pairs);
            free(cjo);

            return NULL;
        }

        *cti += 2; // jump over :
        if(*cti == tc){
            *cje = CJE_ERR_EOF;
        
            free(cjo->pairs);
            free(cjo);
        
            return NULL;
        }

        // value
        CJsonValue* cjov = cj_parse_value(tokens, tc, cti, cje);
        if(*cje != CJE_OK){
            free(cjo->pairs);
            free(cjo);

            return NULL;
        }

        // make the pair
        if(cjo->size == cjo->capacity)
            if((*cje = cjo_realloc(cjo)) != CJE_OK)
                return NULL;

        cjo->pairs[cjo->size].key = _strdup(key);
        
        if(!cjo->pairs[cjo->size].key){
            *cje = CJE_ERR_OOM;

            free(cjo->pairs);
            free(cjo);

            return NULL;
        }

        cjo->pairs[cjo->size].value = cjov;

        if(tokens[*cti][0] != ',' && tokens[*cti][0] != '}'){
            *cje = CJE_ERR_UNEXPECTED_TOKEN;

            free(cjo->pairs);
            free(cjo);

            return NULL;
        }
    }

    ++*cti;

    if(*cti > tc){
        *cje = CJE_ERR_EOF;
        return NULL;
    }

    *cje = CJE_OK;

    return cjo;
}

/**
 * @param tc    tokens count
 * @param cti   current token index
 * @param cje   c-json error
 */
CJsonArray* cj_parse_array(const string* tokens, size_t tc, size_t* cti, CJsonError* cje){
    ++*cti; // skip [

    CJsonArray* cja;
    if((*cje = cja_create(cja)) != CJE_OK)
        return NULL;

    while(tokens[*cti][0] != ']' && *cti <= tc){
        if(tokens[*cti][0] == ',' || tokens[*cti][0] == ':'){
            *cje = CJE_ERR_UNEXPECTED_TOKEN;

            free(cja->data);
            free(cja);

            return NULL;
        }

        if(cja->size == cja->capacity)
            if((*cje = cja_realloc(cja)) != CJE_OK)
                return NULL;

        cja->data[cja->size++] = *cj_parse_value(tokens, tc, cti, cje);

        if(tokens[*cti + 1][0] != ',' && tokens[*cti + 1][0] != ']'){
            *cje = CJE_ERR_UNEXPECTED_TOKEN;

            free(cja->data);
            free(cja);

            return NULL;
        }

        *cti += tokens[*cti + 1][0] == ']' ? 1 : 2;
    }

    ++*cti;

    if(*cti > tc){
        *cje = CJE_ERR_EOF;

        free(cja->data);
        free(cja);

        return NULL;
    }

    *cje = CJE_OK;

    return cja;
}

/**
 * @param tc    tokens count
 * @param cti   current token index
 * @param cje   c-json error
 */
CJsonValue* cj_parse_value(const string* tokens, size_t tc, size_t* cti, CJsonError* cje){
    CJsonValue* cjv;
    if((*cje = cjv_create(cjv)) != CJE_OK)
        return NULL;

    cjv->value_type = cj_def_jsonv_type(tokens[*cti]);
    if(cjv->value_type == CJ_NULL){
        *cje = CJE_ERR_UNEXPECTED_TOKEN;
        
        free(cjv);

        return NULL;
    }

    if(cjv->value_type == CJ_STRING){
        if(!cj_is_valid_strv(tokens[*cti], cje)){
    
            free(cjv);
    
            return NULL;
        }
    
        cjv->cj_string_value = _strdup(tokens[*cti]);
        if(!cjv->cj_string_value){
            *cje = CJE_ERR_OOM;
            
            free(cjv);
    
            return NULL;
        }
    }
    else if(cjv->value_type == CJ_NUMBER){
        if(!cj_is_valid_numv(tokens[*cti], cje)){
    
            free(cjv);
    
            return NULL;
        }
    
        string endptr; // for strtod function
    
        cjv->cj_number_value = strtod(tokens[*cti], &endptr);
    
        if(*endptr != '\0'){
            *cje = CJE_ERR_NUMBER_FORMAT;
    
            free(cjv);
    
            return NULL;
        }
    }
    else if(cjv->value_type == CJ_BOOL){
        if(!cj_is_valid_boolv(tokens[*cti], cje)){
    
            free(cjv);
    
            return NULL;
        }
        cjv->cj_bool_value = !strcmp(tokens[*cti], "true") ? true : false;
    }
    else if(cjv->value_type == CJ_ARRAY)
        cjv->cj_array_value = cj_parse_array(tokens, tc, cti, cje);

    else if(cjv->value_type == CJ_OBJECT)
        cjv->cj_object_value = cj_parse_object(tokens, tc, cti, cje);


    ++*cti;

    if(*cti == tc){
        *cje = CJE_ERR_EOF;
     
        free(cjv);

        return NULL;
    }

    *cje = CJE_OK;

    return cjv;
}