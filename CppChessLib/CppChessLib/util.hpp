namespace util {
	static constexpr char tolower(char c) {
		if ('A' <= c && c <= 'Z') return c - 'A' + 'a';
		else return c;
	}

	static constexpr bool isupper(char c) {
		return 'A' <= c && c <= 'Z';
	}
}