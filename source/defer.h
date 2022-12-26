#pragma once

#include <functional>

#define CONCAT2(x, y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)
#define UNIQUE(a) CONCAT(a, __LINE__)
struct __defer_t {
	std::function<void()> foo;
	~__defer_t() {
		foo();
	}
};
#define DEFER(FOO) __defer_t UNIQUE(_defer){[&](){FOO}};

template<typename T>
void clear(T obj) { }
template<typename T>
void clear(std::vector<T>& obj) {
	for(auto& it : obj) {
		clear(it);
	}
}

#define AC(obj) DEFER(clear(obj);)