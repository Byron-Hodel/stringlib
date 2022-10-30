#ifndef STR_H
#define STR_H

#include <stdint.h>

typedef struct {
	char* c_str;
	int64_t len;
	int64_t capacity;
} string_t;

typedef struct {
	void* (*alloc)(void* user_data, uint64_t size, uint64_t alignment);
	void (*free)(void* user_data, void* addr);
	void* (*realloc)(void* user_data, void* addr, uint64_t size);
	void* user_data;
} string_allocation_callbacks_t;

// allocate a new string
// if c_str is set, the value will be copied into the allocated string
// if length is zero, no allocation will be done. It's kind of pointless though
// if allocator is not set, then the default allocator will be used (malloc.h)
string_t string_create(char* c_str, string_allocation_callbacks_t* allocator);
int8_t string_destroy(string_t string, string_allocation_callbacks_t* allocator);
// copy the contents of a string into a newly allocated string
string_t string_copy(const string_t string, string_allocation_callbacks_t* allocator);
// copy the c_str from one string into the c_str of another string, the dst c_str must have a sufficient length for the copy to work
int8_t string_copy_into(string_t* dst, const string_t src);

// concat a string into the dst string
int8_t string_cat(string_t* dst, const string_t src);

// will resize the string to the desired size
int8_t string_resize(string_t* string, const uint64_t new_size, string_allocation_callbacks_t* allocator);

// returns a string given a c_string. Does not allocate memory, instaid, it copies the pointer into the string type
inline string_t string_get(char* c_str);
// returns a substring of the passed string starting at index with a length of len
inline string_t string_get_substring(string_t string, int64_t index, int64_t len);
// Note: does not allocate a new string, instaid, it points to the already existing string containing the substring,
// to get a newly allocated string, use string_copy()
int64_t string_find_substring(const string_t string, const int64_t start_index, const string_t target);

// returns the index of the first occurence of the target character in str
int64_t string_find_char(const string_t str, const int64_t start_index, const char target);

// returns 1 if str1 is greater than str2
// returns -1 if str1 is less than str2
// returns 0 if both strings are equal
int8_t string_compare(const string_t str1, const string_t str2);





#ifdef STR_IMPLIMENTATION

#include <string.h>
#include <malloc.h>
#include <immintrin.h>


string_t string_create(char* c_str, string_allocation_callbacks_t* allocator) {
	string_t str_tmp = string_get(c_str);
	string_t str = {0};
	if(allocator != NULL) {
		if(allocator->alloc != NULL) {
			str.c_str = (char*)allocator->alloc(allocator->user_data, str_tmp.len+1, 4);
			if(str.c_str == NULL) return str;
		}
	}
	else {
		str.c_str = (char*)aligned_alloc(4, str_tmp.len + 1);
		if(str.c_str == NULL) return str;
	}
	str.len = str_tmp.len;
	str.capacity = str.len;
	memcpy(str.c_str, str_tmp.c_str, str.len);
	return str;
}


int8_t string_destroy(string_t string, string_allocation_callbacks_t* allocator) {
	if(string.c_str == NULL || string.len == 0) return 0;
	if(allocator != NULL) {
		if(allocator->free == NULL) return 0;
		allocator->free(allocator->user_data, string.c_str);
		return 1;
	}
	else {
		free(string.c_str);
		return 1;
	}
}


string_t string_copy(const string_t string, string_allocation_callbacks_t* allocator) {
	string_t str = {0};
	if(allocator != NULL) {
		if(allocator->alloc != NULL) {
			str.c_str = (char*)allocator->alloc(allocator->user_data, string.len+1, 16);
			if(str.c_str == NULL) return str;
		}
	}
	else {
		str.c_str = (char*)calloc(1, string.len + 1);
		if(str.c_str == NULL) return str;
	}
	str.len = string.len;
	str.capacity = str.len;
	memcpy(str.c_str, string.c_str, str.len);
	return str;
}


int8_t string_copy_into(string_t* dst, const string_t src) {
	if(dst->capacity < src.len) return 0;
	memcpy(dst->c_str, src.c_str, src.len+1);
	return 1;
}

int8_t string_cat(string_t* dst, const string_t src) {
	if(dst->capacity - dst->len < src.len) return 0;
	memcpy(dst->c_str + dst->len, src.c_str, src.len);
	dst->len += src.len;
	dst->c_str[dst->len] = '\0';
	return 1;
}

int8_t string_resize(string_t* string, const uint64_t new_size, string_allocation_callbacks_t* allocator) {
	if(allocator != NULL) {
		if(allocator->realloc != NULL) {
			char* c_str = allocator->realloc(allocator->user_data, string->c_str, new_size+1);
			if(c_str == NULL) return 0;
			string->c_str = c_str;
		}
		else if(allocator->alloc != NULL && allocator->free != NULL) {
			char* c_str = allocator->alloc(allocator->user_data, new_size+1, 16);
			uint64_t min_size = new_size < string->len ? new_size : string->len;
			memcpy(c_str, string->c_str, min_size);
			allocator->free(allocator->user_data, string->c_str);
			string->c_str = c_str;
		}
		string->capacity = new_size;
	}
	else {
		char* c_str = realloc(string->c_str, new_size+1);
		if(string->c_str == NULL) return 0;
		string->c_str = c_str;
		string->capacity = new_size;
	}
	return 1;
}

string_t string_get(char* c_str) {
	string_t str;
	str.c_str = c_str;
	str.len = strlen(c_str);
	str.capacity = 0;
	return str;
}


string_t string_get_substring(string_t string, int64_t index, int64_t len) {
	string_t str = {0};
	if(index >= string.len || len > string.len || index + len > string.len || len == 0) return str;
	str.c_str = string.c_str + index;
	str.len = len;
	str.capacity = 0;
	return str;
}


int64_t string_find_substring(const string_t string, const int64_t start_index, const string_t target) {
	return 0;
}


int64_t string_find_char(const string_t str, const int64_t start_index, const char target) {

#if __AVX__ && !defined(STR_NO_AVX)

	const __m256i TARGET = _mm256_set1_epi8(target);
	int64_t offset = 0;
	uint64_t aligned_addr = (uint64_t)(str.c_str + start_index) & 0xFFFFFFFFFFFFFFE0;
	uint64_t diff = (uint64_t)(str.c_str + start_index) - aligned_addr;
	if(diff > 0) {
		__m256i data = (__m256i)_mm256_load_ps((const float*)aligned_addr);
		data = _mm256_cmpeq_epi8(data, TARGET);

		uint32_t mov_mask = _mm256_movemask_epi8(data);
		mov_mask >>= diff;
		if(mov_mask > 0) {
			return (int64_t)__builtin_ctz(mov_mask) + start_index;
		}
		offset = 32;
	}

	uint32_t mov_mask = 0;
	while(mov_mask == 0 && offset - diff < str.len) {
		__m256i data = (__m256i)_mm256_load_ps((const float*)(aligned_addr + offset));
		data = _mm256_cmpeq_epi8(data, TARGET);

		mov_mask = _mm256_movemask_epi8(data);
		offset += 32;
	}
	offset -= 32;
	if(mov_mask > 0) return (int64_t)__builtin_ctz(mov_mask) + offset - diff + start_index;
	else return -1;

#elif __SSE2__

	const __m128i TARGET = _mm_set1_epi8(target);
	int64_t offset = 0;
	uint64_t aligned_addr = (uint64_t)(str.c_str + start_index) & 0xFFFFFFFFFFFFFFE0;
	uint64_t diff = (uint64_t)(str.c_str + start_index) - aligned_addr;
	if(diff > 0) {
		__m128i data = (__m128i)_mm_load_ps((const float*)aligned_addr);
		data = _mm_cmpeq_epi8(data, TARGET);

		uint32_t mov_mask = _mm_movemask_epi8(data);
		mov_mask >>= diff;
		if(mov_mask > 0) {
			return (int64_t)__builtin_ctz(mov_mask) + start_index;
		}
		offset = 16;
	}

	uint32_t mov_mask = 0;
	while(mov_mask == 0 && offset - diff < str.len) {
		__m128i data = (__m128i)_mm_load_ps((const float*)(aligned_addr + offset));
		data = _mm_cmpeq_epi8(data, TARGET);

		mov_mask = _mm_movemask_epi8(data);
		offset += 16;
	}
	offset -= 16;
	if(mov_mask > 0) return (int64_t)__builtin_ctz(mov_mask) + offset - diff + start_index;
	else return -1;

#else

	for(int64_t i = start_index; i < str.len; i++) {
		if(str.c_str[i] == target) return i + start_index;
	}
	return -1;

#endif // __AVX__
}


int8_t string_compare(const string_t str1, const string_t str2) {
	if(str1.c_str[0] != str2.c_str[0]) return str1.c_str[0] - str2.c_str[0];
	int64_t min_len = str1.len < str2.len ? str1.len : str2.len;
	int64_t i = 0;
#if __AVX__ && !defined(STR_NO_AVX)


	for(; i < min_len - 30; i += 32) {
		__m256i a = _mm256_loadu_si256((void*)(str1.c_str + i));
		__m256i b = _mm256_loadu_si256((void*)(str2.c_str + i));
		__m256i cmp = _mm256_cmpeq_epi8(a, b);
		uint32_t mov_mask = ~_mm256_movemask_epi8(cmp);
		if(mov_mask > 0) {
			int64_t index = (int64_t)__builtin_ctz(mov_mask) + i;
			return str1.c_str[index] - str2.c_str[index];
		}
	}


#endif // __AVX__
#if __SSE2__


	for(; i < min_len - 14; i += 16) {
		__m128i a = _mm_loadu_si128((void*)(str1.c_str + i));
		__m128i b = _mm_loadu_si128((void*)(str2.c_str + i));
		__m128i cmp = _mm_cmpeq_epi8(a, b);
		uint32_t mov_mask = ~_mm_movemask_epi8(cmp);
		if(mov_mask > 0) {
			int64_t index = (int64_t)__builtin_ctz(mov_mask) + i;
			return str1.c_str[index] - str2.c_str[index];
		}
	}


#endif // __SSE2__

	for(; i < min_len+1; i++) {
		if(str1.c_str[i] != str2.c_str[i]) return str1.c_str[i] - str2.c_str[i];
	}

	return 0;
}

#endif // STR_IMPLIMENTATION
#endif // STR_H