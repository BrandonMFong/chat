/**
 * author: Brando
 * date: 1/21/25
 */

#ifndef UTILS_TESTS_HPP
#define UTILS_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include <bflibcpp/bflibcpp.hpp>
#include "utils.hpp"
#include "inputbuffer.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bftest/bftest.h>
}

using namespace BF;

BFTEST_UNIT_FUNC(test_utilsIsReady, 2<<10, {
	InputBuffer b0("i");
	BF_ASSERT(Utils::inputReady(b0, kInterfaceStateDraft));
	InputBuffer b1("asdf");
	BF_ASSERT(!Utils::inputReady(b1, kInterfaceStateDraft));
	InputBuffer b2("asdf");
	b2.addChar('\n');
	BF_ASSERT(Utils::inputReady(b2, kInterfaceStateDraft));

})

BFTEST_COVERAGE_FUNC(utils_tests, {
	BFTEST_LAUNCH(test_utilsIsReady);
})

#endif // UTILS_TESTS_HPP

