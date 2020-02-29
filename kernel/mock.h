#ifndef _MOCK_H
#define _MOCK_H

#include <linux/types.h>


struct mock;

struct mock * init_mock(char const **function_names, unsigned long *return_values, size_t n);

int start_mocking(struct mock const *mock);

void stop_mocking(struct mock const *mock);

void destroy_mock(struct mock *mock);

#endif // _MOCK_H