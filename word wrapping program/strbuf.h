typedef struct {
    size_t length;
    size_t used;
    char *data;
} strbuf_t;

int sb_init(strbuf_t *, size_t);
void sb_destroy(strbuf_t *);
int sb_append(strbuf_t *, char);

