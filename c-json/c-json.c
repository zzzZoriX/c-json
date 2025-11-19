#include "c-json.h"
#include <stddef.h>

extern CJsonError cjo_create(CJsonObject* cjo);
extern CJsonError cjo_realloc(CJsonObject* cjo);
extern CJsonError cjv_create(CJsonValue* cjv);
extern CJsonError cja_create(CJsonArray* cja);
extern CJsonError cja_realloc(CJsonArray* cja);

extern CJsonValue* cj_parse_value(const string* tokens, size_t tc, size_t* cti, CJsonError* cje);

static CJsonError cjson_tokenize(pos* pep, string data, size_t lod, string* tokens, size_t* tc){
    for(size_t i = 0; i < lod; ++i){
        ++*pep;

        if(data[i] == ' ' || data[i] == '\n')
            continue;

        else if(
            data[i] == ':' || 
            data[i] == '{' || 
            data[i] == ',' || 
            data[i] == '[' ||
            data[i] == ']' ||
            data[i] == '}' 
        ){
            char str[2] = {data[i], '\0'};

            tokens[++*tc] = str;

            tokens = (string*)realloc(tokens, sizeof(string) * (*tc + 1));
            if(!tokens)
                return CJE_ERR_OOM;
        }

        else if(data[i] == '"'){
            string str = (string)calloc(3, sizeof(char));
            if(!str)
                return CJE_ERR_OOM;

            str[0] = data[i];
            str[2] = '\0';

            size_t str_i = 1; // because 0 is "

            while(++i != lod && data[i] != '"'){
                str[str_i++] = data[i];
                
                // why str_i + 2?
                // because str_i is '\0' pos - 1 and for increase the string we need jump over '\0'
                // and add 1 that equal "+ 2"
                str = (string)realloc(str, sizeof(char) * (str_i + 2));
                if(!str)
                    return CJE_ERR_OOM;

                str[str_i + 1] = '\0';
            }

            if(data[i] == '"')
                str[str_i] = data[i];
            
            tokens[++*tc] = str;

            tokens = (string*)realloc(tokens, sizeof(string) * (*tc + 1));
            if(!tokens)
                return CJE_ERR_OOM;
        }

        else if(data[i] == '-' || isdigit(data[i])){
            if(data[i] == '-' && !isdigit(data[i + 1]))
                return CJE_ERR_NUMBER_FORMAT;

            string number = (string)calloc(2, sizeof(char));
            if(!number)
                return CJE_ERR_OOM;

            number[0] = data[i];
            data[1] = '\0';

            size_t number_i = 1; // because next digit will be placed on index 1

            while((isdigit(data[i]) || data[i] == '.') && i != lod){
                // why str_i + 2?
                // because str_i is '\0' pos - 1 and for increase the string we need jump over '\0'
                // and add 1 that equal "+ 2"
                number = (string)realloc(number, sizeof(char) * (number_i + 2));
                if(!number)
                    return CJE_ERR_OOM;
                
                number[number_i++] = data[i];
                number[number_i] = '\0';
            }

            if(number[number_i - 1] == '.'){
                free(number);

                return CJE_ERR_NUMBER_FORMAT;
            }

            tokens[++*tc] = number;

            tokens = (string*)realloc(tokens, sizeof(string) * (*tc + 1));
            if(!tokens)
                return CJE_ERR_OOM;
        }
    }
    
    return CJE_OK;
}

CJsonValue* cjson_parse(
    const string data,
    const size_t length_of_data,
    CJsonError* error,
    pos* error_position
) {
    assert(error != NULL);

    *error = CJE_OK;
    *error_position = 0;
    pos possible_err_pos = 0;

    CJsonValue* cjson_main_obj;

    string* tokens = (string*)malloc(sizeof(string));
    size_t  tokens_count = 0,
            current_token = 0;

    if(!tokens){
        *error = CJE_ERR_OOM;
        
        free(cjson_main_obj);

        return NULL;
    }

    if((*error = cjson_tokenize(
        &possible_err_pos, 
        data, 
        length_of_data, 
        tokens, 
        &tokens_count)) != CJE_OK){
        free(cjson_main_obj);
        free(tokens);
        
        return NULL;
    }

    cjson_main_obj = cj_parse_value(tokens, tokens_count, &current_token, error);
    if(*error != CJE_OK){
        free(tokens);

        return NULL;
    }

    return cjson_main_obj;
}