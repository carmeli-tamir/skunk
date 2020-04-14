#include "mock_dispatcher.h"
#include "mock.h"

#include "skunk.pb-c.h"

#include <linux/errno.h>
#include <linux/err.h>

static struct mock * mock = NULL;

long parse_message_and_set_mock(char *buffer, u32 *length)
{
    long ret;
    Skunk__ReturnValueMock *mock_setup;
    pr_info("OMG set mock");

    mock_setup = skunk__return_value_mock__unpack(NULL, *length, buffer);
    if (NULL == mock_setup) {
        return -EINVAL;
    }

    ret = set_mock((const char **)mock_setup->function_names, (unsigned long *)mock_setup->eight_byte_ret, mock_setup->n_function_names);

    skunk__return_value_mock__free_unpacked(mock_setup, NULL);

    return ret;
}

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
        return 0;
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