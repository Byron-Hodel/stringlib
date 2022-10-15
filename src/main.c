/************************************
 * NOTE: This file is for testing the
 * functions in str.h and should be
 * deleted once str.h is complete
************************************/

#define STR_IMPLIMENTATION
#include "str.h"

#include <immintrin.h>
#include <stdio.h>

int main(void) {
	string_t str = string_create("cats are cute!", NULL);
	string_t cat = string_get("!!!!");
	if(str.capacity < str.len + cat.len) string_resize(&str, str.len + cat.len, NULL);
	string_cat(&str, cat);

	printf("%s\n", str.c_str);

	string_destroy(str, NULL);
	return 0;
}