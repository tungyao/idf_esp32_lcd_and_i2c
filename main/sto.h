//
// Created by Administrator on 2024/7/15.
//

#ifndef STO_H
#define STO_H


#include "nvs_flash.h"

void sto_init();
void store_data(char *key,char *data);
char *read_data(char *key);



#endif //STO_H
