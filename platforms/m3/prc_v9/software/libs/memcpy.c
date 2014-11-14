// Naive memcpy

void *memcpy(void *dest, const void *src, int n) {
	char *d;
	char const *s = (char const *) src;

	while (n--) {
		*d++ = *s++;
	}

	return dest;
}
