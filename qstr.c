#define _XOPEN_SOURCE

#include "postgres.h"

#include <ctype.h>
#include <string.h>
#include <stddef.h>

#include "fmgr.h"

#include "utils/builtins.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

static int levenshtein_distance(const char *from, const char *to)
{
    int n, m, i, j;

    n = strlen(from) + 1;
    m = strlen(to) + 1;

    int d[n][m];

    for (i = 0; i < n; ++i) {
        for (j = 0; j < m; ++j) {
            if (i == 0 && j == 0) {
                d[i][j] = 0;
            } else if (i == 0 && j > 0) {
                d[i][j] = j;
            } else if (i > 0 && j == 0) {
                d[i][j] = i;
            } else {
                int p = d[i][j - 1] + 1;
                int q = d[i - 1][j] + 1;
                int r = d[i - 1][j - 1] + ((from[i - 1] == to[j - 1]) ? 0 : 1);
                int smallest = p;
                if (q < smallest) {
                    smallest = q;
                }

                if (r < smallest) {
                    smallest = r;
                }

                d[i][j] = smallest;
            }
        }
    }

    return d[n - 1][m - 1];
}

static int least_to_palindrome(const char *s, int i, int j)
{
    if (i >= j - 1) {
        return 0;
    }
    if (s[i] == s[j - 1]) {
        return least_to_palindrome(s, i + 1, j - 1);
    }
    return Min(least_to_palindrome(s, i + 1, j), least_to_palindrome(s, i, j - 1)) + 1;
}

static int str_is_palindrome(const char *s)
{
    int i = 0, j = strlen(s) - 1;
    while (i < j) {
        if (s[i++] != s[j--]) {
            return 0;
        }
    }
    return 1;
}

PG_FUNCTION_INFO_V1(q_str_levenshtein_distance);
PG_FUNCTION_INFO_V1(q_str_least_to_palindrome);
PG_FUNCTION_INFO_V1(q_str_is_palindrome);

Datum
q_str_levenshtein_distance(PG_FUNCTION_ARGS)
{
    char *from = text_to_cstring(PG_GETARG_TEXT_PP(0));
    char *to = text_to_cstring(PG_GETARG_TEXT_PP(1));
    int res = levenshtein_distance(from, to);
    PG_RETURN_INT32(res);
}

Datum
q_str_least_to_palindrome(PG_FUNCTION_ARGS)
{
    char *s = text_to_cstring(PG_GETARG_TEXT_PP(0));
    int len = strlen(s);
    int res = least_to_palindrome(s, 0, len);
    PG_RETURN_INT32(res);
}

Datum
q_str_is_palindrome(PG_FUNCTION_ARGS)
{
    char *s = text_to_cstring(PG_GETARG_TEXT_PP(0));
    int res = str_is_palindrome(s);
    PG_RETURN_BOOL(res);
}

