//
// Created by Administrator on 2024/7/15.
//

#ifndef STO_H
#define STO_H


#include "nvs_flash.h"

void sto_init();
void store_data(char *key,char *data);
uint8_t read_data( const char* key, char* out_value, size_t* length);



#endif //STO_H
