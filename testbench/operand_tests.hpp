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
	Operand OP_DRAFT("draft", 'i');
	Operand OP_HELP("help", '?');
})

BFTEST_UNIT_FUNC(test_operandcompare, 2<<10, {
	Operand OP_DRAFT("draft", 'i');
	Operand OP_HELP("help", '?');

	Operand op_draft_long("draft");
	Operand op_draft_short('i');
	BF_ASSERT(op_draft_long == OP_DRAFT);
	BF_ASSERT(op_draft_short == OP_DRAFT);
	BF_ASSERT(op_draft_long != OP_HELP);
	BF_ASSERT(op_draft_short != OP_HELP);
})

BFTEST_COVERAGE_FUNC(operand_tests, {
	BFTEST_LAUNCH(test_operandinit);
	BFTEST_LAUNCH(test_operandcompare);

})

#endif // OPERAND_TESTS_HPP

