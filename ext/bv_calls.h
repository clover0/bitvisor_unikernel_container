
// BV hyper calls
void bv_yield(void);
int bv_net_write(char *buf, int size);
int bv_net_read(char *buf, int size);

// BV functions
int printf (const char *format, ...)
	__attribute__ ((format (printf, 1, 2)));