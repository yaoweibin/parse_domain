
#include "parse_domain.h"

/* This file contains the tld_string */
#include "data/tld_tree.c"

/* This file contains the test cases */
#include "data/tests.c"


static int
test_domain_case(tldnode *tree)
{
    char              line[MAX_LENGTH];
    int               i, len, count;
    char             *p, *last;
    test_t           *test;
    string_t         *domain;
    http_result_t     result;
    public_suffix_t  *ps;

    domain = &result.domain;
    test = tests;
    count = 0;

    while (test->full_domain) {

        strncpy(line, test->full_domain, MAX_LENGTH);
        memset(&result, 0, sizeof(http_result_t));

        len = strlen(line);
        if (len == 0) {
            break;
        }

        i = len;
        p = line;
        while (i) {
            /* lower the domain letter */
            *p = tolower(*p);

            p++;
            i--;
        }

        /* chmop the useless charset in the end */
        last = line + len - 1;
        while (len) {

            if (*last == '\r' || *last == '\n') {
                last--;
                len--;
                continue;
            }

            break;
        }

        if (len <= 0) {
            goto next;
        }

        domain->data = line;
        domain->len = len;

        if (parse_domain(tree, &result, domain) != 0) {

            if (test->domain != NULL) {
                goto fail;
            }

            goto next;
        }

        ps = &result.complex_domain;

        if (ps->domain.len == 0 && test->domain == NULL) {
            goto next;
        }

        if (test->domain == NULL) {
            goto fail;
        }

        if (strncmp(ps->domain.data, test->domain, ps->domain.len)) {
            goto fail;
        }

next:
        count++;
        test++;
        continue;
fail:
        printf("wanted:\"%s\"\n", test->domain);
        printf("got:\"%.*s\"\n", (int)ps->domain.len, ps->domain.data);

        return -1;
    }

    printf("test successfully with %d cases.\n", count);

    return 0;
}


int
main(int argc, char **argv)
{
    char              line[MAX_LENGTH];
    int               i, len;
    char             *p, *last;
    tldnode          *tree;
    string_t         *domain;
    http_result_t     result;
    public_suffix_t  *ps;

    tree = init_tld_tree(tld_string);
    if (tree == NULL) {
        fprintf(stderr, "can't init the TLD tree.\n");
        return -1;
    }

    /*print_tld_tree(tree, " ");*/

    if (argc > 1) {
        test_domain_case(tree);
        goto end;
    }
    
    domain = &result.domain;

    printf("Input the domain line by line from STDIN\n");

    while (fgets(line, MAX_LENGTH, stdin) != NULL) {

        memset(&result, 0, sizeof(http_result_t));

        len = strlen(line);
        if (len == 0) {
            break;
        }

        i = len;
        p = line;
        while (i) {
            /* lower the domain letter */
            *p = tolower(*p);

            p++;
            i--;
        }

        /* chmop the useless charset in the end */
        last = line + len - 1;
        while (len) {

            if (*last == '\r' || *last == '\n') {
                last--;
                len--;
                continue;
            }

            break;
        }

        if (len <= 0) {
            continue;
        }

        domain->data = line;
        domain->len = len;

        if (parse_domain(tree, &result, domain) != 0) {
            fprintf(stderr, "can't parse the domain: \"%.*s\"\n",
                    (int)domain->len, domain->data);
            continue;
        }

        ps = &result.complex_domain;

        printf("tld:\"%.*s\", ", (int)ps->tld.len, ps->tld.data);
        printf("sld:\"%.*s\", ", (int)ps->sld.len, ps->sld.data);
        printf("trd:\"%.*s\", ", (int)ps->trd.len, ps->trd.data);
        printf("domain:\"%.*s\", ", (int)ps->domain.len, ps->domain.data);
        printf("subdomain:\"%.*s\"\n", (int)ps->subdomain.len, ps->subdomain.data);
    }
    
end:

    free_tld_tree(tree);

    return 0;
}
