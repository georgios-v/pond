
#include <runtime0.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

struct item {
	int value;
	int weight;
};

int knapsack(APP_ARGS);
TASK_PROTO_4(int, knapsack, struct item *, e, int, c, int, n, int, v);
VOID_TASK_PROTO_0(knapsack_par);

int best_so_far = (-2147483647 - 1);

struct item items[28] = {
	{.value = 500, .weight = 96},
	{.value = 491, .weight = 100},
	{.value = 522, .weight = 96},
	{.value = 510, .weight = 96},
	{.value = 500, .weight = 95},
	{.value = 480, .weight = 99},
	{.value = 489, .weight = 99},
	{.value = 524, .weight = 100},
	{.value = 513, .weight = 102},
	{.value = 513, .weight = 104},
	{.value = 516, .weight = 95},
	{.value = 506, .weight = 98},
	{.value = 493, .weight = 106},
	{.value = 500, .weight = 99},
	{.value = 505, .weight = 104},
	{.value = 521, .weight = 105},
	{.value = 518, .weight = 102},
	{.value = 512, .weight = 101},
	{.value = 495, .weight = 98},
	{.value = 486, .weight = 104},
	{.value = 496, .weight = 100},
	{.value = 523, .weight = 95},
	{.value = 486, .weight = 106},
	{.value = 511, .weight = 99},
	{.value = 514, .weight = 103},
	{.value = 522, .weight = 93},
	{.value = 491, .weight = 103},
	{.value = 488, .weight = 101}
};

static int compare(struct item *a, struct item *b) {
	double c = ((double) a->value / a->weight) - ((double) b->value / b->weight);
	if (c > 0) return -1;
	if (c < 0) return 1;
	return 0;
}

TASK_4(int, knapsack, struct item *, e, int, c, int, n, int, v) {
	int with, without, best;
	double ub;
	if (c < 0) return ( -2147483647 - 1);
	if (n == 0 || c == 0) return v;
	ub = (double) v + c * e->value / e->weight;
	if (ub < best_so_far) {
		return ( -2147483647 - 1);
	}
	SPAWN(knapsack, e + 1, c, n - 1, v);
	with = CALL(knapsack, e + 1, c - e->weight, n - 1, v + e->value);
	without = SYNC(knapsack);
	best = with > without ? with : without;
	if (best > best_so_far) best_so_far = best;
	return best;
}

VOID_TASK_0(knapsack_par) {
	int n = 28, capacity = 1120, sol;

	qsort(items, n, sizeof (struct item), (int (*)(const void *, const void *))compare);
	SPAWN(knapsack, &items[0], capacity, n, 0);
	sol = SYNC(knapsack);

	print("Best value is %d\n\n", sol);
	return;
}

int knapsack(APP_ARGS) {

	ROOT_CALL(knapsack_par);

	print("\n\nKNAPSACK END\n\n");

	return 0;
}


