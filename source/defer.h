#pragma once

#include <functional>

#define CONCAT2(x, y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)
#define UNIQUE(a) CONCAT(a, __LINE__)
#define DEFER(FOO) struct UNIQUE(__defer_t){std::function<void()> foo;~UNIQUE(__defer_t)(){foo();}}UNIQUE(__defer){[&](){FOO}};

template<typename T>
void clear(T obj) { }

#define AC(obj) DEFER(cloar(obj);)