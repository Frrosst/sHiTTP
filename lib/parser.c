#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include <string.h>
#include <ctype.h>


/* RFC 9110 tchar lookup table
 *
 * tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*"
 *       / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
 *       / DIGIT / ALPHA
 */

static const unsigned char is_tchar[256] = {
    /* 0x00–0x1F (control chars) */
    [0 ... 31] = 0,

    /* 0x20 ' ' */
    [' '] = 0,

    /* 0x21–0x7E */
    ['!'] = 1,
    ['#'] = 1,
    ['$'] = 1,
    ['%'] = 1,
    ['&'] = 1,
    ['\''] = 1,
    ['*'] = 1,
    ['+'] = 1,
    ['-'] = 1,
    ['.'] = 1,
    ['^'] = 1,
    ['_'] = 1,
    ['`'] = 1,
    ['|'] = 1,
    ['~'] = 1,

    /* Digits */
    ['0'] = 1, ['1'] = 1, ['2'] = 1, ['3'] = 1, ['4'] = 1,
    ['5'] = 1, ['6'] = 1, ['7'] = 1, ['8'] = 1, ['9'] = 1,

    /* Uppercase letters */
    ['A'] = 1, ['B'] = 1, ['C'] = 1, ['D'] = 1, ['E'] = 1,
    ['F'] = 1, ['G'] = 1, ['H'] = 1, ['I'] = 1, ['J'] = 1,
    ['K'] = 1, ['L'] = 1, ['M'] = 1, ['N'] = 1, ['O'] = 1,
    ['P'] = 1, ['Q'] = 1, ['R'] = 1, ['S'] = 1, ['T'] = 1,
    ['U'] = 1, ['V'] = 1, ['W'] = 1, ['X'] = 1, ['Y'] = 1,
    ['Z'] = 1,

    /* Lowercase letters */
    ['a'] = 1, ['b'] = 1, ['c'] = 1, ['d'] = 1, ['e'] = 1,
    ['f'] = 1, ['g'] = 1, ['h'] = 1, ['i'] = 1, ['j'] = 1,
    ['k'] = 1, ['l'] = 1, ['m'] = 1, ['n'] = 1, ['o'] = 1,
    ['p'] = 1, ['q'] = 1, ['r'] = 1, ['s'] = 1, ['t'] = 1,
    ['u'] = 1, ['v'] = 1, ['w'] = 1, ['x'] = 1, ['y'] = 1,
    ['z'] = 1,

    /* 0x7F DEL and 0x80–0xFF */
    [127 ... 255] = 0
};



static const unsigned char is_forbidden_target_char[256] = {
    [0 ... 31] = 1,   // CTLs
    [' '] = 1,
    ['#'] = 1,
    [127] = 1,
    [128 ... 255] = 1
};

enum req_line_status {
    ACCEPTED,
    MAL_METHOD,
    MAL_PATH, 
    MAL_VERSION
};


enum req_line_status parse_req_line(const char *line, Request_line *req_line) {

    const char *p = line;
    size_t i = 0;


    while (*p && *p != ' ') {
        if (i >= sizeof(req_line->method) - 1)
            return MAL_METHOD;
        req_line->method[i++] = *p++;
    }
    req_line->method[i] = '\0';


    if (*p != ' ') return MAL_METHOD;

    p++;
    if (*p == ' ') return MAL_METHOD;  

    i = 0;
    while (*p && *p != ' ') {
        if (i >= sizeof(req_line->path) - 1)
            return MAL_PATH;
        req_line->path[i++] = *p++;
    }
    req_line->path[i] = '\0';


    if (*p != ' ') return MAL_VERSION;  

    p++;
    if (*p == ' ') return MAL_VERSION;  

    i = 0;
    while (*p) {
        if (i >= sizeof(req_line->version) - 1)
            return MAL_VERSION;
        if (*p == ' ' || *p == '\t'){
            return MAL_VERSION;
        }
            
        req_line->version[i++] = *p++;
    }
    req_line->version[i] = '\0';



    // checking method
    if (strlen(req_line->method) >= sizeof(req_line->method)){
        return MAL_METHOD;
    }
    
    if (req_line->method[0] == '\0')
        return MAL_METHOD;

    i = 0;
    while (req_line->method[i] != '\0'){
        if (!is_tchar[(unsigned char)req_line->method[i]])
            return MAL_METHOD;
        
        i++;
    }
    


    // checking path 
    
    size_t path_len = strlen(req_line->path);
    if ( (long unsigned int)path_len >= sizeof(req_line->path)){
        return MAL_PATH;
    }
   

    if (req_line->path[0] == '*'){
        if (req_line->path[1] != '\0'){
            return MAL_PATH;
        }
        goto jump_version;
    } else if(req_line->path[0] != '/' || req_line->path[1] == '/'){
        return MAL_PATH;
    } 

    i = 0;
    int qmark_check = 0;

   
    while (i < path_len) {
        unsigned char c = req_line->path[i];

        if (is_forbidden_target_char[c])
            return MAL_PATH;

        if (c == '?') {
            if (qmark_check)
                return MAL_PATH; 
            qmark_check = 1;
        }

        if (c == '%') {
            if (i + 2 >= path_len)
                return MAL_PATH; 
            if (!isxdigit(req_line->path[i+1]) || !isxdigit(req_line->path[i+2]))
                return MAL_PATH; 
            i += 3; 
            continue;
        }

        i++;
    }



jump_version:
    // Check version

    int version_len = strlen(req_line->version);
    if (version_len != 8){
        return MAL_VERSION; 
    }
    const char *v = req_line->version;

    if (v[0] != 'H' || v[1] != 'T' || v[2] != 'T' ||
        v[3] != 'P' || v[4] != '/' ||
        !isdigit((unsigned char)v[5]) ||
        v[6] != '.' ||
        !isdigit((unsigned char)v[7]))
    {
        
        return MAL_VERSION; 
    }    
    
    
    return ACCEPTED;   
    
}
  

void parse_req_headers(){
    
}


void parse_req(const char *raw_req, Request *req){
    
    char *p =  (char *)raw_req;
    size_t len  = strlen(raw_req);
    char *end_p = p + len;
    char line[512];
    size_t i = 0;
    


    if (*p == '\r' || *p == '\n') return;

    //request line
    while (p != end_p){
        if (*p =='\r'){
            if (p + 1 < end_p &&*(p+1) != '\n'){
                return; //malformed
            }
            break;
        }
    
        if (i >= sizeof(line) - 1){
            return; //overflow
        }
        line[i++] = *p++;
    
    }
    
    if (p + 2 >= end_p) return; //overflow
    
    line[i] = '\0';
    
    p += 2;
    i = 0;

    // headers
    int count = 0;
    char *header_start = p;
    while (p != end_p){

        if (*p == '\r') {
            if (p + 1 < end_p && *(p+1) == '\n'){
                
                count++; 
                
                if (p + 2 < end_p && *(p+2) == '\r' && p + 3 < end_p && *(p+3) == '\n'){
                    p += 4;
                    break;
                }
                p += 2;
                continue;
        
            } else{
                return; //malformed
            }

        }
        p++;
    }
    

    Headers *headers = (Headers*)calloc(count, sizeof(Headers));
    if (!headers) return;
    
    fprintf(stderr, "num of header: %u\n", count);

    p = header_start;
    int char_cnt = 0;
    while (p != end_p){

        if (*p == '\r') {
            headers[i].length = char_cnt;
            char_cnt = 0;
            
            if (p + 1 < end_p && *(p+1) == '\n'){
                
                if (p + 2 < end_p && *(p+2) == '\r' && p + 3 < end_p && *(p+3) == '\n'){
                    p += 4;
                    break;
                }
                p += 2;
                continue;
        
            } else{
                return; //malformed
            }

            i++;

        }
        
        if (char_cnt == 0){
            headers[i].start = p;
        }
        
        char_cnt++;
        p++;
    }

    free(headers);

    
    enum req_line_status result = parse_req_line(line, &req->request_line);
    if (result == MAL_VERSION){ 
        fprintf(stderr, "malformed version"); 
    }

}

