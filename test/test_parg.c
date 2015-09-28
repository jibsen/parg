/*
 * test_parg - parg unit test
 *
 * Written in 2015 by Joergen Ibsen
 *
 * To the extent possible under law, the author(s) have dedicated all
 * copyright and related and neighboring rights to this software to the
 * public domain worldwide. This software is distributed without any
 * warranty. <http://creativecommons.org/publicdomain/zero/1.0/>
 */

#include "parg.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "greatest.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static const char *os_def = ":no::r:";

static const struct parg_option po_def[] = {
	{ "noarg", PARG_NOARG, NULL, 'n' },
	{ "optarg", PARG_OPTARG, NULL, 'o' },
	{ "reqarg", PARG_REQARG, NULL, 'r' },
	{ "foo", PARG_NOARG, NULL, 'f' },
	{ "foobar", PARG_NOARG, NULL, 'b' },
	{ 0, 0, 0, 0 }
};

/*
 * parg_getopt tests
 */

TEST parg_getopt_app_only(void)
{
	struct parg_state ps;
	char *argv[] = { "app" };

	parg_init(&ps);

	ASSERT_EQ(-1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(1, ps.optind);

	PASS();
}

TEST parg_getopt_dash(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "-" };

	parg_init(&ps);

	ASSERT_EQ(1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(argv[1], ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_double_dash(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--" };

	parg_init(&ps);

	ASSERT_EQ(-1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_nonopt(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "foo" };

	parg_init(&ps);

	ASSERT_EQ(1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(argv[1], ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_no_match(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "-u" };

	parg_init(&ps);

	ASSERT_EQ('?', parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ('u', ps.optopt);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_noarg(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "-n" };

	parg_init(&ps);

	ASSERT_EQ('n', parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_optarg_missing(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "-o" };

	parg_init(&ps);

	ASSERT_EQ('o', parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_optarg_inline(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "-oarg" };

	parg_init(&ps);

	ASSERT_EQ('o', parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(&argv[1][2], ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_reqarg_missing(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "-r" };

	parg_init(&ps);

	ASSERT_EQ(':', parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ('r', ps.optopt);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_reqarg_inline(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "-rarg" };

	parg_init(&ps);

	ASSERT_EQ('r', parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(&argv[1][2], ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_reqarg_nextarg(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "-r", "arg" };

	parg_init(&ps);

	ASSERT_EQ('r', parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(argv[2], ps.optarg);
	ASSERT_EQ(3, ps.optind);

	ASSERT_EQ(-1, parg_getopt(&ps, ARRAY_SIZE(argv), argv, os_def));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(3, ps.optind);

	PASS();
}

/*
 * parg_getopt_long tests
 */

TEST parg_getopt_long_app_only(void)
{
	struct parg_state ps;
	char *argv[] = { "app" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(1, ps.optind);

	PASS();
}

TEST parg_getopt_long_dash(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "-" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ(1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(argv[1], ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_double_dash(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_nonopt(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "foo" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ(1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(argv[1], ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_no_match(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--unknown" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ('?', parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_flag(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--flag" };
	int li = -1;
	int flag = 0;
	struct parg_option po_flag[] = {
		{ "flag", PARG_NOARG, 0, 1 },
		{ 0, 0, 0, 0 }
	};

	po_flag[0].flag = &flag;

	parg_init(&ps);

	ASSERT_EQ(0, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_flag, &li));
	ASSERT_EQ(0, li);
	ASSERT_EQ(1, flag);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_flag, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_noarg(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--noarg" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ('n', parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(0, li);
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_noarg_inline(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--noarg=arg" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ(':', parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(0, li);
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ('n', ps.optopt);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_optarg_missing(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--optarg" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ('o', parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(1, li);
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_optarg_inline(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--optarg=arg" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ('o', parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(1, li);
	ASSERT_EQ(&argv[1][9], ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_reqarg_missing(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--reqarg" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ(':', parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(2, li);
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ('r', ps.optopt);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_reqarg_inline(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--reqarg=arg" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ('r', parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(2, li);
	ASSERT_EQ(&argv[1][9], ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_reqarg_nextarg(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--reqarg", "arg" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ('r', parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(2, li);
	ASSERT_EQ(argv[2], ps.optarg);
	ASSERT_EQ(3, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(3, ps.optind);

	PASS();
}

TEST parg_getopt_long_prefix_ambiguous(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--fo" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ('?', parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_prefix_exact(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--foo" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ('f', parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(3, li);
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

TEST parg_getopt_long_prefix_unambiguous(void)
{
	struct parg_state ps;
	char *argv[] = { "app", "--foob" };
	int li = -1;

	parg_init(&ps);

	ASSERT_EQ('b', parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(4, li);
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	ASSERT_EQ(-1, parg_getopt_long(&ps, ARRAY_SIZE(argv), argv, ":", po_def, &li));
	ASSERT_EQ(NULL, ps.optarg);
	ASSERT_EQ(2, ps.optind);

	PASS();
}

/*
 * parg_reorder tests
 */

TEST parg_reorder_short(void)
{
	char *argv[] = {
		"app", "first", "-r", "--", "foo", "-u", "bar", "-o", "--",
		"-n", "last"
	};
	char *argv_expected[] = {
		"app", "-r", "--", "-u", "-o", "--", "first", "foo", "bar",
		"-n", "last"
	};
	size_t i;

	ASSERT_EQ(6, parg_reorder(ARRAY_SIZE(argv), argv, os_def, NULL));

	for (i = 0; i < ARRAY_SIZE(argv); ++i) {
		ASSERT_STR_EQ(argv_expected[i], argv[i]);
	}

	PASS();
}

TEST parg_reorder_long(void)
{
	char *argv[] = {
		"app", "first", "--reqarg", "--", "foo", "--unknown", "bar",
		"--optarg", "--", "--noarg", "last"
	};
	char *argv_expected[] = {
		"app", "--reqarg", "--", "--unknown", "--optarg", "--",
		"first", "foo", "bar", "--noarg", "last"
	};
	size_t i;

	ASSERT_EQ(6, parg_reorder(ARRAY_SIZE(argv), argv, "", po_def));

	for (i = 0; i < ARRAY_SIZE(argv); ++i) {
		ASSERT_STR_EQ(argv_expected[i], argv[i]);
	}

	PASS();
}

SUITE(parg_getopt_tests)
{
	RUN_TEST(parg_getopt_app_only);

	RUN_TEST(parg_getopt_dash);

	RUN_TEST(parg_getopt_double_dash);

	RUN_TEST(parg_getopt_nonopt);

	RUN_TEST(parg_getopt_no_match);

	RUN_TEST(parg_getopt_noarg);

	RUN_TEST(parg_getopt_optarg_missing);
	RUN_TEST(parg_getopt_optarg_inline);

	RUN_TEST(parg_getopt_reqarg_missing);
	RUN_TEST(parg_getopt_reqarg_inline);
	RUN_TEST(parg_getopt_reqarg_nextarg);
}

SUITE(parg_getopt_long_tests)
{
	RUN_TEST(parg_getopt_long_app_only);

	RUN_TEST(parg_getopt_long_dash);

	RUN_TEST(parg_getopt_long_double_dash);

	RUN_TEST(parg_getopt_long_nonopt);

	RUN_TEST(parg_getopt_long_no_match);

	RUN_TEST(parg_getopt_long_flag);

	RUN_TEST(parg_getopt_long_noarg);
	RUN_TEST(parg_getopt_long_noarg_inline);

	RUN_TEST(parg_getopt_long_optarg_missing);
	RUN_TEST(parg_getopt_long_optarg_inline);

	RUN_TEST(parg_getopt_long_reqarg_missing);
	RUN_TEST(parg_getopt_long_reqarg_inline);
	RUN_TEST(parg_getopt_long_reqarg_nextarg);

	RUN_TEST(parg_getopt_long_prefix_ambiguous);
	RUN_TEST(parg_getopt_long_prefix_exact);
	RUN_TEST(parg_getopt_long_prefix_unambiguous);
}

SUITE(parg_reorder_tests)
{
	RUN_TEST(parg_reorder_short);
	RUN_TEST(parg_reorder_long);
}

GREATEST_MAIN_DEFS();

int main(int argc, char *argv[])
{
	GREATEST_MAIN_BEGIN();
	RUN_SUITE(parg_getopt_tests);
	RUN_SUITE(parg_getopt_long_tests);
	RUN_SUITE(parg_reorder_tests);
	GREATEST_MAIN_END();
}
