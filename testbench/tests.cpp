/**
 * author: brando
 * date: 2/15/24
 */

#include "inputbuffer_tests.hpp"
#include "log.hpp"

LOG_INIT

int main() {
	int pass = 0, fail = 0;
	float tp = 0, tf = 0;

	printf("\n---------------------------\n");

	inputbuffer_tests(&pass, &fail);
	printf("[+ %d, - %d]\n", pass, fail);
	tp += pass; tf += fail;
	pass = 0; fail = 0;

	printf("Grade - %.2f%% (%d/%d)\n", (tp / (tp + tf)) * 100, (int) tp, (int) (tp + tf));

	return 0;
}

