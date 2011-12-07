unsigned fib(unsigned f) {
	if (f == 0)
		return 0;
	if (f == 1)
		return 1;
	return fib(f-1) + fib(f-2);
}

int main() {
	unsigned f = fib(20);
	return !(f == 6765);
}
