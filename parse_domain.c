
#include "parse_domain.h"


/*
 * Note from Weibin Yao. I use the method from dkim-regdom. And do many
 * modifications and fix some bugs.
 *
 * Calculate the effective registered domain of a fully qualified domain name.
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to you under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Florian Sager, 03.01.2009, sager@agitos.de
 * Christian Heindl, 29.12.2008, heindlc@agitos.de
 * Ward van Wanrooij, 04.04.2010, ward@ward.nu
 *
 */

struct dlist_el {
    string_t              val;
	struct dlist_el      *next;
};

typedef struct dlist_el dlist;


static int read_tld_string(tldnode *node, char *s, size_t len, int pos);
static char * find_registered_domain(tldnode *subtree, dlist *dom);

#define  ALL  '*'
#define  THIS '!'


tldnode *
init_tld_tree(char *tlds) 
{
    size_t   len;
    tldnode *root;

    len = strlen(tlds);
    if (len == 0) {
        return NULL;
    }

	root = (tldnode *) malloc(sizeof(tldnode));
    if (root == NULL) {
        return NULL;
    }

	if (read_tld_string(root, tlds, len, 0) < 0) {
        return NULL;
    }

	return root;
}


static int
tld_atoi(char *line, size_t n)
{
    int  value;

    if (n == 0) {
        return -1;
    }

    for (value = 0; n--; line++) {
        if (*line < '0' || *line > '9') {
            return -1;
        }

        value = value * 10 + (*line - '0');
    }

    if (value < 0) {
        return -1;

    } else {
        return value;
    }
}


static int
read_tld_string(tldnode *node, char *s, size_t len, int pos)
{
	size_t start = pos;
	int    state = 0;

    int  lenc, i;
    char c;

	memset(node, 0, sizeof(tldnode));

	do {
		c = *(s + pos);

		switch(state) {

        case 0: 
            /* general read */
            if ((c == ',') || (c == ')') || (c=='(')) {
                /* add last domain */
                lenc = (node->attr == THIS) ? (pos - start - 1) : (pos - start);

                node->dom.data = s + start;
                node->dom.len = lenc;

                if (c == '(') {
                    /* read number of children */
                    start = pos;
                    state = 1;
                } else if (c == ')' || c == ',') {
                    /* return to parent domains */
                    return pos;
                }

            } else if ( c == '!') {
                node->attr = THIS;
            }

            break;

        case 1:  

            /* reading number of elements (<number>: */

            if (c == ':') {
                node->num_children = tld_atoi(s + start + 1, pos - start - 1);
                if (node->num_children < 0) {
                    return -1;
                }

                /* allocate space for children */
                node->subnodes = malloc(node->num_children * sizeof(tldnode *));
                if (node->subnodes == NULL) {
                    return -1;
                }

                for (i = 0; i < node->num_children; i++) {
                    node->subnodes[i] = (tldnode *) malloc(sizeof(tldnode));
                    pos = read_tld_string(node->subnodes[i], s, len, pos + 1);
                    if (pos < 0) {
                        return -1;
                    }
                }

                return pos + 1;
            }

            break;
        }

        pos++;

    } while ((size_t) pos < len);

    return pos;
}


void
print_tld_tree(tldnode *node, const char *spacer)
{
    int  i;
    char dest[100];

	if (node->num_children != 0) {
        /* has children */
		printf("%s%.*s:\n", spacer, (int) node->dom.len, node->dom.data);

		for(i = 0; i < node->num_children; i++) {
			snprintf(dest, 100, "  %s", spacer);
			print_tld_tree(node->subnodes[i], dest);
		}
	} else {
        /* no children */
		printf("%s%.*s: %c\n", spacer, (int) node->dom.len,
               node->dom.data, node->attr);
	}
}


void
free_tld_tree(tldnode *node)
{
    int i;

	if (node->num_children > 0) {

		for(i = 0; i < node->num_children; i++) {
			free_tld_tree(node->subnodes[i]);
		}

        free(node->subnodes);
	}

	free(node);
}


/* linear search for domain (and * if available) */
static tldnode *
find_tld_node(tldnode *parent, string_t *subdom)
{
	int        i;
    string_t  *dom;
	tldnode   *all_node = NULL;

	for (i = 0; i < parent->num_children; i++) {

        dom = &parent->subnodes[i]->dom;

		if ((dom->len == subdom->len)
             && strncmp(dom->data, subdom->data, dom->len) == 0) {
			return parent->subnodes[i];
		}

		if ((all_node == NULL) && dom->len == 1 && ALL == dom->data[0]) {
			all_node = parent->subnodes[i];
		}
	}

	return all_node;
}


static char *
find_registered_domain(tldnode *root, dlist *dom)
{
	tldnode  *subtree, *subnode;

    subtree = root;

    while (subtree && dom) {

        subnode = find_tld_node(subtree, &dom->val);

#if 0
        printf("%.*s\n", (int)dom->val.len, dom->val.data);
        printf("subnode = %p\n", subnode);
#endif

        if (subnode == NULL) {

            /* Not match any node */
            if (subtree == root) {
                return NULL;
            }
            else {
                /* The first unmatch node */
                return dom->val.data;
            }

        } else if ((subnode->num_children == 1)
                    && (subnode->subnodes[0]->attr == THIS)) {

            return dom->val.data;
        }

        subtree = subnode;
        dom = dom->next;
    }

    return NULL;
}


static void
free_dom_labels(dlist *head)
{
	dlist *cur;

	while (head) {
		cur = head;
		head = cur->next;

		free(cur);
	}
}


static char * 
tld_strlchr(char *p, char *last, u_char c)
{
    while (p < last) {                                                                                                       
        if (*p == c) {
            return p;
        }

        p++;
    }

    return NULL;
}


static char *
get_registered_domain(tldnode *tree, string_t *domain, public_suffix_t *ps)
{
    char  *start, *p, *last, *result;
    dlist *cur, *head, *pre;

    if (domain->len == 0) {
        return NULL;
    }

    start = p = domain->data;
    last = domain->data + domain->len;

    head = NULL;
    while (p < last) {

        if (*p == '.') {

            if (start == p) {
                result = NULL;
                goto end;
            }

            cur = (dlist*) malloc(sizeof(dlist));
            if (cur == NULL) {
                result = NULL;
                goto end;
            }

            cur->val.data = start;
            cur->val.len = p - start;
            cur->next = head;
            head = cur;

            start = p + 1;
        }

        p++;
    }

    if (start < p) {
        cur = (dlist*) malloc(sizeof(dlist));
        if (cur == NULL) {
            result = NULL;
            goto end;
        }

        cur->val.data = start;
        cur->val.len = p - start;
        cur->next = head;
        head = cur;
    }

    if (head == NULL) {
        return NULL;
    }
    
	result = find_registered_domain(tree, head);
	if (result == NULL) {
        goto end;
	}

    if (result < domain->data || result >= last) {
        result = NULL;
        goto end;
    }

    /* assure there is at least 1 TLD in the stripped signing domain */
	if (tld_strlchr(result, last, '.') == NULL) {

		if (head->next == NULL) {
            result = NULL;
            goto end;
		} else {
            ps->tld.data = result;
            ps->tld.len = last - result;

            ps->sld = head->next->val;

			result = head->next->val.data;
		}
	}
    else {
        pre = NULL;
        cur = head;
        while (cur) {

            if (cur->val.data == result) {
                if (pre && ps->tld.len == 0) {
                    ps->tld.data = pre->val.data;
                    ps->tld.len = last - pre->val.data;
                }

                ps->sld = cur->val;

                if (cur->next) {
                    ps->trd.data = domain->data;
                    ps->trd.len = cur->val.data - 1 - domain->data;
                }

                break;
            }

            pre = cur;
            cur = cur->next;
        }
    }

end:

    if (result) {
        ps->domain.data = result;
        ps->domain.len  = last - result;

        ps->subdomain = *domain;
    }

    free_dom_labels(head);
	return result;
}


int 
parse_domain(tldnode *tree, http_result_t *r, string_t *domain)
{
    char *result;

    result = get_registered_domain(tree, domain, &r->complex_domain);
    if (result) {
        return 0;
    }

    return -1;
}
