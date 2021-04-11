#include "pch.h"

#include "StepByStep.h"

#include <coroutine>

namespace CheaPU {

	StepByStep<uint8_t> test_process() {
		co_yield 1;
		co_yield 2;
		co_return 3;
	}

	TEST(StepByStep, steps) {
		auto step_by_step = test_process();
		ASSERT_EQ(1, step_by_step());
		ASSERT_EQ(2, step_by_step());
		ASSERT_EQ(3, step_by_step());
	}


	TEST(StepByStep, completeWork) {
		auto step_by_step = test_process();
		ASSERT_FALSE(step_by_step.completed());
		step_by_step();
		step_by_step();
		ASSERT_FALSE(step_by_step.completed());
		step_by_step();
		ASSERT_TRUE(step_by_step.completed());
	}

}