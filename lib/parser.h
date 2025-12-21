#ifndef PARSER_H
#define PARSER_H



// Struct definitions
typedef struct {
    char method[16];
    char path[256];
    char version[16];
} Request_line;

typedef struct {
    Request_line request_line;
} Request;

typedef struct {
    char *start;   // pointer to the beginning of the header line in raw_req
    size_t length; // length of the line excluding CRLF
} Headers;

// Function declarations
void parse_req(const char *raw_req, Request *req);
enum req_line_status parse_req_line(const char *line, Request_line *req_line);
void parse_req_headers();

#endif
