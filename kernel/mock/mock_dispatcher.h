#ifndef _MOCK_DISPATCHER_H
#define _MOCK_DISPATCHER_H

#include <linux/types.h>

int set_mock(char const **function_names, unsigned long *return_values, size_t n);

int start_mocking(void);

int stop_mocking(void);

int unset_mock(void);

#endif // _MOCK_DISPATCHER_H