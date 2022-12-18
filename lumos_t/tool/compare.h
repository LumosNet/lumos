#ifndef COMPARE_H
#define COMPARE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cJSON.h"
#include "cJSON_Utils.h"
#include "benchmark_json.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define PASS    0
#define ERROR   1

int compare_test(cJSON *cjson_benchmark, void **origin, char **bnames, int num);

int compare_float_array(float *a, float *b, int num);
int compare_int_array(int *a, int *b, int num);
int compare_string_array(char **a, char **b, int num);

#ifdef __cplusplus
}
#endif

#endif
