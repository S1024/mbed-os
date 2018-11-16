/* mbed Microcontroller Library
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed.h"
#include "mbed_error.h"
#include "greentea-client/test_env.h"
#include "unity/unity.h"

#if !MBED_CONF_PLATFORM_CRASH_CAPTURE_ENABLED
#error [NOT_SUPPORTED] crash_reporting test not supported
#endif

#define MSG_VALUE_DUMMY "0"
#define MSG_VALUE_LEN 32
#define MSG_KEY_LEN 64

#define MSG_KEY_DEVICE_READY "crash_reporting_ready"
#define MSG_KEY_DEVICE_ERROR "crash_reporting_inject_error"

void mbed_error_reboot_callback(mbed_error_ctx *error_context)
{
    TEST_ASSERT_EQUAL_UINT(MBED_ERROR_OUT_OF_MEMORY, error_context->error_status);
    TEST_ASSERT_EQUAL_UINT(1, error_context->error_reboot_count);
    mbed_reset_reboot_error_info();

    //Send the ready msg to host to indicate test pass
    greentea_send_kv(MSG_KEY_DEVICE_READY, MSG_VALUE_DUMMY);
}

void test_crash_reporting()
{
    // Report readiness
    greentea_send_kv(MSG_KEY_DEVICE_READY, MSG_VALUE_DUMMY);

    static char _key[MSG_KEY_LEN + 1] = { };
    static char _value[MSG_VALUE_LEN + 1] = { };

    printf("\nWaiting for inject error\n");
    greentea_parse_kv(_key, _value, MSG_KEY_LEN, MSG_VALUE_LEN);
    printf("\nGot inject error\n");
    if (strcmp(_key, MSG_KEY_DEVICE_ERROR) == 0) {
        printf("\nErroring...\n");
        MBED_ERROR1(MBED_ERROR_OUT_OF_MEMORY, "Executing crash reporting test.", 0xDEADBAD);
        TEST_ASSERT_MESSAGE(0, "crash_reporting() error call failed.");
    }
    printf("\nWaiting for inject error");
    TEST_ASSERT_MESSAGE(0, "Unexpected message received.");
}

int main(void)
{
    GREENTEA_SETUP(30, "crash_reporting");
    test_crash_reporting();
    GREENTEA_TESTSUITE_RESULT(0);

    return 0;
}
