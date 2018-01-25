
void str_replace(char *target, const char *pat, const char *replacement)
{
    char buffer[1024] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t pat_len = strlen(pat);
    size_t repl_len = strlen(replacement);

    while (1) {
        const char *p = strstr(tmp, pat);

        // walked past last occurrence of pat; copy remaining part
        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }

        // copy part before patttern
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        // copy replacement string
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        // adjust pointers, move on
        tmp = p + pat_len;
    }

    // write altered string back to target
    strcpy(target, buffer);
}

