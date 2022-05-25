#pragma once

// this is for tracking bugs for a public release. needs an improvement but concept should be like this.
#define LINE_ID (crt::get_hash(__FILE__) + crt::get_hash(__LINE__))

extern void show_assert_message(const char* msg, const char* file, int line);

#ifdef _DEBUG
#define CRT_ASSERT(expr, msg) if(!(expr)) {show_assert_message((msg), __FILE__, __LINE__);  *(volatile int*)0 = 0xCC; }
#else
#define CRT_ASSERT
#endif

