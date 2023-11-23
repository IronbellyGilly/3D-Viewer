#include <check.h>

#include "./base/affine.h"
#include "./base/object_parser.h"

#define ERR_NONE 0
#define EPS 1e-5f
#define _USE_MATH_DEFINES

START_TEST(nonexistent_file_test) {
        Model obj;
        int result = parser(&obj, "non_existent_file.obj");
        ck_assert_int_eq(result, ERROR);
}
END_TEST

START_TEST(existent_file_test) {
    char *fp = "./datasets/1.txt";
    Model obj;

    int result = parser(&obj, fp);
    ck_assert_int_eq(result, OK);
    ck_assert_uint_eq(obj.v_count, 72);
    ck_assert_uint_eq(obj.f_count, 72);
    cleanStructs(obj);
}
END_TEST

START_TEST(line_counter_test) {
    char *fp = "./datasets/1.txt";
    Model obj;
    FILE *file = fopen(fp, "rb");
    if (file == NULL) {
        ck_abort_msg("Error opening file");
    }
    initializeStruct(&obj);
    lineCounter(&obj, file);
    ck_assert_uint_eq(obj.v_count, 72);
    ck_assert_uint_eq(obj.f_count, 72);
    fclose(file);
    cleanStructs(obj);
}
END_TEST

START_TEST(parser_test) {
    char *fp = "./datasets/1.txt";
    Model obj;
    int result = parser(&obj, fp);
    ck_assert_int_eq(result, OK);
    int facets_count_check = 22;
    int vertex_count_check = 33;
    int arr_of_facets_check[22] = {
            0, 2, 2, 1, 1, 0, 2, 3, 3, 1, 1, 2, 4, 6,
            6, 5, 5, 4, 6,7, 7, 5};
    double arr_of_vertexes_check[33] = {0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5,
                                        -0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5,
                                        -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5};
    for (int i = 0; i < facets_count_check; i++) {
        ck_assert_int_eq(arr_of_facets_check[i], obj.facets_arr[i]);
    }
    for (int i = 0; i < vertex_count_check; i++) {
        ck_assert_int_eq(arr_of_vertexes_check[i], obj.vertex_arr[i]);
    }
    cleanStructs(obj);
}
END_TEST

START_TEST(test_moves)
{
    Model obj;
    obj.v_count = 6;
    obj.vertex_arr = (double*)malloc(sizeof(double) * 6);
    obj.vertex_arr[0] = 1.0;
    obj.vertex_arr[1] = 2.0;
    obj.vertex_arr[2] = 3.0;
    obj.vertex_arr[3] = 4.0;
    obj.vertex_arr[4] = 5.0;
    obj.vertex_arr[5] = 6.0;

    moveOX(&obj, 1.0);
    moveOY(&obj, 1.0);
    moveOZ(&obj, 1.0);

    ck_assert_double_eq(obj.vertex_arr[0], 2.0);
    ck_assert_double_eq(obj.vertex_arr[1], 3.0);
    ck_assert_double_eq(obj.vertex_arr[2], 4.0);
    ck_assert_double_eq(obj.vertex_arr[3], 5.0);
    ck_assert_double_eq(obj.vertex_arr[4], 6.0);
    ck_assert_double_eq(obj.vertex_arr[5], 7.0);

    free(obj.vertex_arr);
}
END_TEST

int main() {
    int nf;
    Suite *s1 = suite_create("Core");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);

    suite_add_tcase(s1, tc1_1);

    tcase_add_test(tc1_1, nonexistent_file_test);
    tcase_add_test(tc1_1, existent_file_test);
    tcase_add_test(tc1_1, line_counter_test);
    tcase_add_test(tc1_1, parser_test);
    tcase_add_test(tc1_1, test_moves);


    srunner_run_all(sr, CK_ENV);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? 0 : 1;
}
