/**
 * @file    test_framework.h
 * @brief   Minimal dependency-free unit-test harness for the host build.
 *
 * Each test file defines `static void test_*(void)` cases, runs them from main()
 * via RUN(), and ends with TEST_MAIN_END() which returns non-zero on any failure
 * (so CTest reports pass/fail). No third-party dependency required.
 */
#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <math.h>

static int g_checks = 0;
static int g_failed = 0;

#define CHECK(cond)                                                                                \
	do                                                                                             \
	{                                                                                              \
		g_checks++;                                                                                \
		if (!(cond))                                                                               \
		{                                                                                          \
			g_failed++;                                                                            \
			printf("  FAIL %s:%d: CHECK(%s)\n", __FILE__, __LINE__, #cond);                        \
		}                                                                                          \
	} while (0)

#define CHECK_TRUE(c)  CHECK(c)
#define CHECK_FALSE(c) CHECK(!(c))

#define CHECK_EQ_INT(a, b)                                                                         \
	do                                                                                             \
	{                                                                                              \
		g_checks++;                                                                                \
		long _a = (long)(a), _b = (long)(b);                                                       \
		if (_a != _b)                                                                              \
		{                                                                                          \
			g_failed++;                                                                            \
			printf("  FAIL %s:%d: %s == %s (%ld != %ld)\n", __FILE__, __LINE__, #a, #b, _a, _b);   \
		}                                                                                          \
	} while (0)

#define CHECK_NEAR(a, b, eps)                                                                      \
	do                                                                                             \
	{                                                                                              \
		g_checks++;                                                                                \
		double _d = fabs((double)(a) - (double)(b));                                               \
		if (_d > (double)(eps))                                                                    \
		{                                                                                          \
			g_failed++;                                                                            \
			printf("  FAIL %s:%d: |%s - %s| = %g > %g\n", __FILE__, __LINE__, #a, #b, _d,          \
			       (double)(eps));                                                                 \
		}                                                                                          \
	} while (0)

#define RUN(testfn)                                                                                \
	do                                                                                             \
	{                                                                                              \
		printf("- %s\n", #testfn);                                                                 \
		testfn();                                                                                  \
	} while (0)

#define TEST_MAIN_END()                                                                            \
	do                                                                                             \
	{                                                                                              \
		printf("%s: %d checks, %d failed\n", __FILE__, g_checks, g_failed);                        \
		return g_failed ? 1 : 0;                                                                   \
	} while (0)

#endif /* TEST_FRAMEWORK_H */
