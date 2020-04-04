#include "mock_dispatcher.h"
#include "mock.h"

#include <linux/errno.h>
#include <linux/err.h>

static struct mock * mock = NULL;

int set_mock(char const **function_names, unsigned long *return_values, size_t n)
{
    if (mock) {
        return -EBUSY;
    }

    mock = init_mock(function_names, return_values, n);
    if (IS_ERR(mock)) {
        mock = NULL;
		return PTR_ERR(mock);
    }
    return 0;
}

int start_mocking()
{
    if (!mock) {
        return -EINVAL;
    }
    return start_mock(mock);
}

int stop_mocking()
{
    if (!mock) {
        return -EINVAL;
    }
    stop_mock(mock);
    return 0;
}

int unset_mock(void)
{
    if (!mock) {
        return -EINVAL;
    }
    destroy_mock(mock);
    mock = NULL;
    return 0;
}