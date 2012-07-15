
/* Test cases from "http://publicsuffix.org/list/test.txt" */

typedef struct {
    char *full_domain;
    char *domain;
} test_t;


test_t tests[] = {
    /* Mixed case.*/
    {"COM", NULL},
    {"example.COM", "example.com"},
    {"WwW.example.COM", "example.com"},
    /* Leading dot. */
    {".com", NULL},
    {".example", NULL},
    {".example.com", NULL},
    {".example.example", NULL},
    /* Unlisted TLD. */
    {"example", NULL},
    {"example.example", NULL},
    {"b.example.example", NULL},
    {"a.b.example.example", NULL},
#if 0
    /* Listed, but non-Internet, TLD.*/
    {"local", NULL},
    {"example.local", NULL},
    {"b.example.local", NULL},
    {"a.b.example.local", NULL},
#endif
    /* TLD with only 1 rule. */
    {"biz", NULL},
    {"domain.biz", "domain.biz"},
    {"b.domain.biz", "domain.biz"},
    {"a.b.domain.biz", "domain.biz"},
    /* TLD with some 2-level rules. */
    {"com", NULL},
    {"example.com", "example.com"},
    {"b.example.com", "example.com"},
    {"a.b.example.com", "example.com"},
    {"uk.com", NULL},
    {"example.uk.com", "example.uk.com"},
    {"b.example.uk.com", "example.uk.com"},
    {"a.b.example.uk.com", "example.uk.com"},
    {"test.ac", "test.ac"},
    /* TLD with only 1 {wildcard} rule. */
    {"cy", NULL},
    {"c.cy", NULL},
    {"b.c.cy", "b.c.cy"},
    {"a.b.c.cy", "b.c.cy"},
    /* More complex TLD. */
    {"jp", NULL},
    {"test.jp", "test.jp"},
    {"www.test.jp", "test.jp"},
    {"ac.jp", NULL},
    {"test.ac.jp", "test.ac.jp"},
    {"www.test.ac.jp", "test.ac.jp"},
    {"kyoto.jp", NULL},
    {"c.kyoto.jp", NULL},
    {"b.c.kyoto.jp", "b.c.kyoto.jp"},
    {"a.b.c.kyoto.jp", "b.c.kyoto.jp"},
    {"pref.kyoto.jp", "pref.kyoto.jp"},	    /* Exception rule. */
    {"www.pref.kyoto.jp", "pref.kyoto.jp"},	/* Exception rule. */
    {"city.kyoto.jp", "city.kyoto.jp"},	    /* Exception rule. */
    {"www.city.kyoto.jp", "city.kyoto.jp"},	/* Exception rule. */
    /* TLD with a wildcard rule and exceptions. */
    {"om", NULL},
    {"test.om", NULL},
    {"b.test.om", "b.test.om"},
    {"a.b.test.om", "b.test.om"},
    {"songfest.om", "songfest.om"},
    {"www.songfest.om", "songfest.om"},
    /* US K12. */
    {"us", NULL},
    {"test.us", "test.us"},
    {"www.test.us", "test.us"},
    {"ak.us", NULL},
    {"test.ak.us", "test.ak.us"},
    {"www.test.ak.us", "test.ak.us"},
    {"k12.ak.us", NULL},
    {"test.k12.ak.us", "test.k12.ak.us"},
    {"www.test.k12.ak.us", "test.k12.ak.us"},
    {NULL, NULL}
};
