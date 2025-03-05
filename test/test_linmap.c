/* Sensor scaling helper. */
#include "control/linmap.h"
#include "test_framework.h"

static void test_basic_mapping(void)
{
	CHECK_NEAR(linmap(200, 200, 300, 0, 100), 0.0, 1.0);
	CHECK_NEAR(linmap(300, 200, 300, 0, 100), 100.0, 1.0);
	CHECK_NEAR(linmap(250, 200, 300, 0, 100), 50.0, 1.0);
}

static void test_degenerate_range(void)
{
	/* in_min == in_max -> midpoint of out range, no divide-by-zero. */
	CHECK_NEAR(linmap(5, 100, 100, 0, 100), 50.0, 1.0);
}

int main(void)
{
	RUN(test_basic_mapping);
	RUN(test_degenerate_range);
	TEST_MAIN_END();
}
