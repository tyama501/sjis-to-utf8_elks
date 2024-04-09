#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include "table_sjis.h"
#include "table_utf8.h"

void println(const char* str, ...) {
    va_list args;
    va_start(args, str);
  //    vprintf(str, args);
    vfprintf(stdout, str, args);
  //    printf("\n");
    va_end(args);
}

FILE* file_open_read(char* filename) {
    FILE* file_ptr = fopen(filename, "r");
    if (file_ptr == NULL) {
        println("Failed to open file\n");
        exit(0);
    }
    return file_ptr;
}

void file_seek_end(FILE* file_ptr) {
    bool is_error = fseek(file_ptr, 0, SEEK_END);
    if (is_error) {
        println("SEEK_END failed\n");
        exit(0);
    }
}

void file_seek_set(FILE* file_ptr) {
    bool is_error = fseek(file_ptr, 0L, SEEK_SET);
    if (is_error) {
        println("SEEK_SET failed\n");
        exit(0);
    }
}

int32_t get_file_length(FILE* file_ptr) {
    int32_t length = 0;
    file_seek_end(file_ptr);
    length = ftell(file_ptr);
    file_seek_set(file_ptr);
    return length;
}

uint8_t __far * memory_alloc(int32_t size) {
  //    uint8_t* memory = (unsigned char*)malloc(size);
    uint8_t __far * memory = (unsigned char __far *)fmemalloc(size);
    if (memory == NULL) {
        println("Failed to allocate\n");
        exit(0);
    }
    return memory;
}

void file_read(FILE* file_ptr, uint8_t __far * data, int32_t size) {
  //    bool is_error = fread(data, 1, size, file_ptr) < size;
  //    if (is_error) {
  //        println("Failed to read file");
  //        exit(0);
  //    }
    int datget;
    while ((datget = getc(file_ptr)) != EOF) {
        *data = datget;
        if (((long)data & 0xFFFF) == 0xFFFF) {
            data += 0x10000001;  // increment segment
        } else {
            data++;
        }
    }
}

uint8_t __far * get_file_raw_data(char* filename, int32_t* size) {
    FILE *file_ptr;
    uint8_t __far * raw_data;
    file_ptr = file_open_read(filename);
    *size = get_file_length(file_ptr);
    raw_data = memory_alloc(*size);
    file_read(file_ptr, raw_data, *size);
    fclose(file_ptr);
    return raw_data;
}

// utf8 1byte to char
void utf8_1byte_to_char(char* str,uint32_t utf8_code) {
    uint32_t tmp_utf8 = utf8_code;
    str[1] = '\0';
    str[0] = (uint8_t)tmp_utf8;
}

// utf8 2byte to char
void utf8_2byte_to_char(char* str, uint32_t utf8_code) {
    uint32_t tmp_utf8 = utf8_code;
    str[2] = '\0';
    str[1] = (uint8_t)tmp_utf8;
    tmp_utf8 = tmp_utf8 >> 8;
    str[0] = (uint8_t)tmp_utf8;
}

// utf8 3byte to char
void utf8_3byte_to_char(char* str, uint32_t utf8_code) {
    uint32_t tmp_utf8 = utf8_code;
    str[3] = '\0';
    str[2] = (uint8_t)tmp_utf8;
    tmp_utf8 = tmp_utf8 >> 8;
    str[1] = (uint8_t)tmp_utf8;
    tmp_utf8 = tmp_utf8 >> 8;
    str[0] = (uint8_t)tmp_utf8;
}

void utf8_to_char(char* str, uint32_t utf8_code) {
    if (utf8_code < 0x80) {
        utf8_1byte_to_char(str, utf8_code);
    } else if (utf8_code <= 0xF1A0) {
        utf8_2byte_to_char(str, utf8_code);
    } else {
        utf8_3byte_to_char(str, utf8_code);
    }
}

int search_sjis_index(uint32_t* table, uint32_t sjis_code) { //Return position that matches SJIS code
    int table_len = sizeof(table_sjis) / sizeof(uint32_t);
    for (int i = 0; i < table_len; i++) {
        if (sjis_code == table_sjis[i]) {
            return i;
        }
    }
    //printf("\n%d\n",sjis_code);
    return 0;
}

void print_utf8_from_sjis(uint32_t* table, uint32_t sjis_code) {
    int index = search_sjis_index(table, sjis_code);
    if (index) {
        char str[4] = "";
        utf8_to_char(str, table_utf8[index]);
        //printf("%s", str);
        println(str);
    } else { // No convert
        char str[4] = "";
        utf8_to_char(str, sjis_code);
        //printf("%s", str);
        println(str);
    }
}

uint32_t get_2byte_from_raw_data(uint8_t __far * data, int offset) {
    uint32_t code = 0;
    //   code += data[offset + 0]; // get 2byte
    //   code = code << 8;
    //   code += data[offset + 1];
    code += *data; // get 2byte
    code = code << 8;
    if (((long)data & 0xFFFF) == 0xFFFF) {
        data += 0x10000001;  // increment segment
    } else {
        data++;
    }
    code += *data;
    return code;
}

void print_sjis_data(uint8_t __far * data, int32_t size) {
    for(int32_t offset = 0; offset < size; ) {
        if ((*data < 0x80) ||
            (*data >= 0xA1 && *data <= 0xDF)) { // If first byte is less than 0x80 then 1byte char
            uint32_t sjis_code = 0;
            sjis_code += *data;
            if (((long)data & 0xFFFF) == 0xFFFF) {
                data += 0x10000001;  // increment segment
            } else {
                data++;
            }
            offset += 1;
            print_utf8_from_sjis(table_sjis, sjis_code);
        } else { // If first byte is greater or equal 0x80 then 2byte char
            uint32_t sjis_code = 0;
            sjis_code = get_2byte_from_raw_data(data, offset);
            if (((long)data & 0xFFFE) == 0xFFFE) {
                data += 0x10000002;  // increment segment
            } else {
                data += 2;
            }
            offset += 2;
            print_utf8_from_sjis(table_sjis, sjis_code);
        }
    }
}

int main(int argc, char **argv) {
    uint8_t __far * data;
    int32_t size;

    if (argc > 1) {
        data = get_file_raw_data(argv[1], &size);
    } else {
        println("Usage : sjisutf8 textfile\n");
        exit(0);
    }

    print_sjis_data(data, size);
  //    free(data);
    return 0;
}
