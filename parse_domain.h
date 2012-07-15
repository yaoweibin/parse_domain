
#ifndef _PARSE_HTTP_H_
#define _PARSE_HTTP_H_


#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>


typedef struct {
    char                 *data;
    size_t                len;
} string_t;


struct tldnode_el {
	string_t              dom;
	char                  attr;

	int                   num_children;
	struct tldnode_el   **subnodes;
};

typedef struct tldnode_el tldnode;

/* like the ruby public suffix service packet */
typedef struct {
    string_t              domain;
    string_t              subdomain;
    string_t              tld;
    string_t              sld;
    string_t              trd;
} public_suffix_t;

typedef struct {
    string_t              scheme;
    string_t              domain;
    public_suffix_t       complex_domain;
    string_t              uri;
    string_t              arg;
    string_t              input;  /* lower case input */
} http_result_t;


#define tolower(c)      (char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)

#define MAX_LENGTH      2048


tldnode * init_tld_tree(char *tlds);
void print_tld_tree(tldnode *node, const char *spacer);
void free_tld_tree(tldnode *node);

int parse_domain(tldnode *tree, http_result_t *r, string_t *domain);

#endif /* _PARSE_HTTP_H_ */
