#include <check.h>
#include <string.h>
#include "protocol.h"

// ← TEST CASE SẼ VIẾT Ở ĐÂY
START_TEST(test_attach_from_idle)
{
    struct UE ue;
    ue.socket = -1;
    ue.id = 1;
    ue.state = IDLE;

    char response[1024];
    int should_close = process_message(&ue, "ATTACH_REQUEST", response);

    ck_assert_str_eq(response, "ATTACH_ACCEPT\n");
    ck_assert_int_eq(ue.state, ATTACHED);
}
END_TEST

START_TEST(test_attach_reject_from_attached)
{
    struct UE ue;
    ue.socket = -1;
    ue.id = 1;
    ue.state = ATTACHED;

    char response[1024];
    int should_close = process_message(&ue, "ATTACH_REQUEST", response);

    ck_assert_str_eq(response, "ATTACH_REJECT\n");
    ck_assert_int_eq(ue.state, ATTACHED);
}
END_TEST

START_TEST(test_ping_from_attached)
{
    struct UE ue;
    ue.socket = -1;
    ue.id = 1;
    ue.state = ATTACHED;

    char response[1024];
    int should_close = process_message(&ue, "PING", response);

    ck_assert_str_eq(response, "PONG\n");
    ck_assert_int_eq(ue.state, ATTACHED);
}
END_TEST

START_TEST(test_ping_reject_from_idle)
{
    struct UE ue;
    ue.socket = -1;
    ue.id = 1;
    ue.state = IDLE;

    char response[1024];
    int should_close = process_message(&ue, "PING", response);

    ck_assert_str_eq(response, "PING_REJECT\n");
    ck_assert_int_eq(ue.state, IDLE);
}
END_TEST

START_TEST(test_detach_from_attached)
{
    struct UE ue;
    ue.socket = -1;
    ue.id = 1;
    ue.state = ATTACHED;

    char response[1024];
    int should_close = process_message(&ue, "DETACH", response);

    ck_assert_str_eq(response, "DETACH_ACCEPT\n");
    ck_assert_int_eq(ue.state, IDLE);
    ck_assert_int_eq(should_close, 1);
}
END_TEST

START_TEST(test_detach_reject_from_idle)
{
    struct UE ue;
    ue.socket = -1;
    ue.id = 1;
    ue.state = IDLE;

    char response[1024];
    int should_close = process_message(&ue, "DETACH", response);

    ck_assert_str_eq(response, "DETACH_REJECT\n");
    ck_assert_int_eq(ue.state, IDLE);
    ck_assert_int_eq(should_close, 0);
}
END_TEST

START_TEST(test_unknown_message_from_idle)
{
    struct UE ue;
    ue.socket = -1;
    ue.id = 1;
    ue.state = IDLE;

    char response[1024];
    int should_close = process_message(&ue, "GARBAGE", response);

    ck_assert_str_eq(response, "UNKNOWN_MESSAGE\n");
    ck_assert_int_eq(ue.state, IDLE);
}
END_TEST

START_TEST(test_unknown_message_from_attached)
{
    struct UE ue;
    ue.socket = -1;
    ue.id = 1;
    ue.state = ATTACHED;

    char response[1024];
    int should_close = process_message(&ue, "GARBAGE", response);

    ck_assert_str_eq(response, "UNKNOWN_MESSAGE\n");
    ck_assert_int_eq(ue.state, ATTACHED);
    ck_assert_int_eq(should_close, 0);
}
END_TEST

// Sau đó mới tới protocol_suite() — chỉ ĐĂNG KÝ, không viết lại nội dung test
Suite *protocol_suite(void) {
    Suite *s = suite_create("Protocol");
    TCase *tc_core = tcase_create("Core");

    // ← ĐĂNG KÝ TEST CASE Ở ĐÂY, VÍ DỤ: tcase_add_test(tc_core, ten_test);
    tcase_add_test(tc_core, test_attach_from_idle);
    tcase_add_test(tc_core, test_attach_reject_from_attached);
    tcase_add_test(tc_core, test_ping_from_attached);
    tcase_add_test(tc_core, test_ping_reject_from_idle);
    tcase_add_test(tc_core, test_detach_from_attached);
    tcase_add_test(tc_core, test_detach_reject_from_idle);
    tcase_add_test(tc_core, test_unknown_message_from_idle);
    tcase_add_test(tc_core, test_unknown_message_from_attached);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void) {
    Suite *s = protocol_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? 0 : 1;
}