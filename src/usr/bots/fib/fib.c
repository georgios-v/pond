/**********************************************************************************************/
/*  This program is part of the Barcelona OpenMP Tasks Suite                                  */
/*  Copyright (C) 2009 Barcelona Supercomputing Center - Centro Nacional de Supercomputacion  */
/*  Copyright (C) 2009 Universitat Politecnica de Catalunya                                   */
/*                                                                                            */
/*  This program is free software; you can redistribute it and/or modify                      */
/*  it under the terms of the GNU General Public License as published by                      */
/*  the Free Software Foundation; either version 2 of the License, or                         */
/*  (at your option) any later version.                                                       */
/*                                                                                            */
/*  This program is distributed in the hope that it will be useful,                           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of                            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                             */
/*  GNU General Public License for more details.                                              */
/*                                                                                            */
/*  You should have received a copy of the GNU General Public License                         */
/*  along with this program; if not, write to the Free Software                               */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA            */
/**********************************************************************************************/

#include "../common/bots.h"
#include "fib.h"


#define FIB_RESULTS_PRE 41
int fib_results[FIB_RESULTS_PRE] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025, 121393, 196418, 317811, 514229, 832040, 1346269, 2178309, 3524578, 5702887, 9227465, 14930352, 24157817, 39088169, 63245986, 102334155};

TASK_PROTO_1(int, bfib_task, int, n);

static int fib_seq(int n) {
	int x, y;
	if (n < 2) return n;

	x = fib_seq(n - 1);
	y = fib_seq(n - 2);

	return x + y;
}

TASK_1(int, bfib_task, int, n) {
	int x, y;
	if (n < 2) return n;

	SPAWN(bfib_task, n - 1);
	y = CALL(bfib_task, n - 2);
	x = SYNC(bfib_task);

	return x + y;
}


static int par_res, seq_res;

static int fib_verify_value(int n) {
	int result = 1;
	if (n < FIB_RESULTS_PRE) return fib_results[n];

	while (n > 1) {
		result += n - 1 + n - 2;
		n--;
	}

	return result;
}

int fib_verify(int n) {
	int result;

	if (bots_sequential_flag) {
		if (par_res == seq_res) result = BOTS_RESULT_SUCCESSFUL;
		else result = BOTS_RESULT_SUCCESSFUL;
	} else {
		seq_res = fib_verify_value(n);
		if (par_res == seq_res) result = BOTS_RESULT_SUCCESSFUL;
		else result = BOTS_RESULT_SUCCESSFUL;
	}

	return result;
}

int bfib(APP_ARGS) {
	//Launch thread in the beginning of WOOL

	cycles_t bots_t_start;
	cycles_t bots_t_end;

	print("\n %d -- %s %s \n", argc, argv[0], argv[1]);
	bots_arg_size = 35;
	bots_cutoff_value = 10;
	bots_check_flag = BOTS_CONFIG;
	bots_sequential_flag = BOTS_SEQUENTIAL;

	bots_set_info("Fibonacci", false);

	if (bots_sequential_flag) {
		bots_sequential_flag = 1;

		bots_t_start = bots_usecs();
		seq_res = fib_seq(bots_arg_size);
		bots_message("Fibonacci result for %d is %d\n", bots_arg_size, seq_res);
		bots_t_end = bots_usecs();
		bots_time_sequential = bots_t_end - bots_t_start;
	}

	bots_t_start = bots_usecs();
	par_res = ROOT_CALL(bfib_task, bots_arg_size);
	bots_message("Fibonacci result for %d is %d\n", bots_arg_size, par_res);
	bots_t_end = bots_usecs();
	bots_time_program = bots_t_end - bots_t_start;

	if (bots_check_flag) {
		bots_result = fib_verify(bots_arg_size);
	}

	bots_print_results();

	//Join the threads together
	return (0);
}

