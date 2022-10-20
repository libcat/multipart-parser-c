#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "multipart_parser.h"

#define BOUNDARY_STR "hello world"

// all the callbacks is paused
// data callbacks
long on_header_field(multipart_parser *p, const char *at, size_t length) {
    printf("[HEADER_FIELD] \"%.*s\"\n", (int)length, at);
    return MPPE_OK;
}

long on_header_value(multipart_parser *p, const char *at, size_t length) {
    printf("[HEADER_VALUE] \"%.*s\"\n", (int)length, at);
    return MPPE_OK;
}

long on_part_data(multipart_parser *p, const char *at, size_t length) {
    printf("[PART_DATA] \"%.*s\"\n", (int)length, at);
    return MPPE_OK;
}

// notify callbacks
long on_part_data_begin(multipart_parser *p) {
    printf("[PART_DATA_BEGIN]\n");
    return MPPE_OK;
}

long on_headers_complete(multipart_parser *p) {
    printf("[HEADERS_COMPLETE]\n");
    return MPPE_OK;
}

long on_part_data_end(multipart_parser *p) {
    printf("[PART_DATA_END]\n");
    return MPPE_OK;
}

long on_body_end(multipart_parser *p) {
    printf("[BODY_END]\n");
    return MPPE_OK;
}

int main(int argc, char **argv) {
    const char *boundary = BOUNDARY_STR;
    if (argc > 1) {
        boundary = argv[1];
    }
    size_t boundary_len = strnlen(boundary, 70);

    multipart_parser_settings settings;
    settings.on_header_field = (multipart_data_cb)on_header_field;
    settings.on_header_value = (multipart_data_cb)on_header_value;
    settings.on_part_data = (multipart_data_cb)on_part_data;
    settings.on_part_data_begin = (multipart_notify_cb)on_part_data_begin;
    settings.on_headers_complete = (multipart_notify_cb)on_headers_complete;
    settings.on_part_data_end = (multipart_notify_cb)on_part_data_end;
    settings.on_body_end = (multipart_notify_cb)on_body_end;

    multipart_parser parser;

    multipart_parser_init((multipart_parser *)&parser, boundary, boundary_len, &settings);

    char buffer[8192] = { '\0' };
    size_t buffer_len = 0;
    size_t buffer_parsed;
    while ((buffer_len = read(0, buffer, sizeof(buffer))) > 0) {
        buffer_parsed = 0;
        size_t parsed;
        parsed = multipart_parser_execute((multipart_parser *)&parser, &buffer[buffer_parsed], buffer_len - buffer_parsed);
        if (parsed == MPPE_ERROR) {
            // parse failed
            multipart_parser_error_msg((multipart_parser *)&parser, buffer, sizeof(buffer) - 1);
            fprintf(stderr, "failed parse %s\n", buffer);
            return 1;
        }

        buffer_parsed += parsed;

        // move remaining buffer to head
        memmove(buffer, &buffer[buffer_parsed], buffer_len - buffer_parsed);
        memset(&buffer[buffer_len - buffer_parsed], 0, buffer_parsed);
    }

    return 0;
}
