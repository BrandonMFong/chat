/**
 * author: Brando
 * date: 1/9/25
 */

#ifndef OPERAND_TESTS_HPP
#define OPERAND_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include <bflibcpp/bflibcpp.hpp>
#include "operand.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bftest/bftest.h>
}

using namespace BF;

BFTEST_UNIT_FUNC(test_operandinit, 2<<10, {
	Operand op0("draft", 'i');
	Operand op1("help", '?');
})

BFTEST_COVERAGE_FUNC(operand_tests, {
	BFTEST_LAUNCH(test_operandinit);
})

#endif // OPERAND_TESTS_HPP

