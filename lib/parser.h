#ifndef PARSER_H
#define PARSER_H



// Struct definitions
typedef struct {
    char method[16];
    char path[256];
    char version[16];
} Request_line;


typedef struct {
    char *field_name;
    char *value;
} Headers;

typedef struct {
    char *start;
    size_t length; 
} Headers_info;

typedef struct {
    Request_line request_line;
    Headers *headers;
    int header_number;
} Request;

// Function declarations
void parse_req(const char *raw_req, Request *req);
enum req_line_status parse_req_line(const char *line, Request_line *req_line);
void parse_req_headers(Headers_info *headers_info, Headers *headers, int count);

#endif
