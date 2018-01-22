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

/* Original code from the Application Kernel Matrix by Cray */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../common/bots.h"
#include "floorplan.h"

#define INPUT_DATA \
"15\n\n2\n3 10\n6 5\n0 -1\n2\n\n3\n2 12\n3 8\n4 6	\n-1 1\n3\n\n2\n5 12\n6 10\n2 1\n4\n\n3\n3 8\n4 6\n8 3\n-1 3\n5\n\n4 \n4 10\n5 8\n8 5\n10 4\n-1 2\n6\n\n1\n3 3\n-1 5\n7\n\n3\n2 16\n4 8\n8 4\n6 5\n8\n\n3\n2 6\n3 4\n4 3\n-1 4\n9\n\n1\n3 3\n8 4\n10\n\n2\n5 6\n6 5\n9 4\n11\n\n1\n4 4\n-1 6\n12\n\n4\n2 12\n4 6\n6 4\n12 2\n7 8\n13\n\n3\n4 9\n6 6\n9 4\n-1 9\n14\n\n1\n3 9\n13 10\n15\n\n2\n3 22\n6 11\n11 13\n0\n\n713\n"



#define ROWS 64
#define COLS 64
#define DMAX 64
#define max(a, b) ((a > b) ? a : b)
#define min(a, b) ((a < b) ? a : b)

int solution = -1;

typedef int coor[2];
typedef char ibrd[ROWS][COLS];
typedef char (*pibrd)[COLS];

struct cell {
	int n;
	coor *alt;
	int top;
	int bot;
	int lhs;
	int rhs;
	int left;
	int above;
	int next;
};

struct cell * gcells;

int MIN_AREA;
//ibrd BEST_BOARD;
char BEST_BOARD[ROWS][COLS];
int MIN_FOOTPRINT[2];

int N;

pthread_mutex_t critical_l;
pthread_mutex_t atom_l;

/* compute all possible locations for nw corner for cell */
static int starts(int id, int shape, int *NWS, struct cell *cells) {
	int i, n, top, bot, lhs, rhs;
	int rows, cols, left, above;

	/* size of cell */
	rows = cells[id].alt[shape][0];
	cols = cells[id].alt[shape][1];

	/* the cells to the left and above */
	left = cells[id].left;
	above = cells[id].above;

	/* if there is a vertical and horizontal dependence */
	if ((left >= 0) && (above >= 0)) {

		top = cells[above].bot + 1;
		lhs = cells[left].rhs + 1;
		bot = top + rows;
		rhs = lhs + cols;

		/* if footprint of cell touches the cells to the left and above */
		if ((top <= cells[left].bot) && (bot >= cells[left].top) &&
				(lhs <= cells[above].rhs) && (rhs >= cells[above].lhs)) {
			n = 1;
			NWS[0] = top;
			NWS[1] = lhs;
		} else {
			n = 0;
		}

		/* if there is only a horizontal dependence */
	} else if (left >= 0) {

		/* highest initial row is top of cell to the left - rows */
		top = max(cells[left].top - rows + 1, 0);
		/* lowest initial row is bottom of cell to the left */
		bot = min(cells[left].bot, ROWS);
		n = bot - top + 1;

		for (i = 0; i < n; i++) {
			NWS[(i * 2) + 0] = i + top;
			NWS[(i * 2) + 1] = cells[left].rhs + 1;
		}

	} else {

		/* leftmost initial col is lhs of cell above - cols */
		lhs = max(cells[above].lhs - cols + 1, 0);
		/* rightmost initial col is rhs of cell above */
		rhs = min(cells[above].rhs, COLS);
		n = rhs - lhs + 1;

		for (i = 0; i < n; i++) {
			NWS[(i * 2) + 0] = cells[above].bot + 1;
			NWS[(i * 2) + 1] = i + lhs;
		}
	}

	return (n);
}

/* lay the cell down on the board in the rectangular space defined
   by the cells top, bottom, left, and right edges. If the cell can
   not be layed down, return 0; else 1.
 */
static int lay_down(int id, char *board, struct cell *cells) {
	int i, j, top, bot, lhs, rhs;

	top = cells[id].top;
	bot = cells[id].bot;
	lhs = cells[id].lhs;
	rhs = cells[id].rhs;

	for (i = top; i <= bot; i++) {
		for (j = lhs; j <= rhs; j++) {
			if (board[j + (i * COLS)] == 0) board[j + (i * COLS)] = (char) id;
			else return (0);
		}
	}

	return (1);
}


#define read_integer(file,var) sscanf(file, "%d", &var)

static void read_inputs(char *data) {
	int i, j, n;

	read_integer(data, n);
	N = n;

	gcells = (struct cell *) malloc((n + 1) * sizeof (struct cell));

	gcells[0].n = 0;
	gcells[0].alt = 0;
	gcells[0].top = 0;
	gcells[0].bot = 0;
	gcells[0].lhs = -1;
	gcells[0].rhs = -1;
	gcells[0].left = 0;
	gcells[0].above = 0;
	gcells[0].next = 0;

	for (i = 1; i < n + 1; i++) {

		read_integer(data, gcells[i].n);
		gcells[i].alt = (coor *) malloc(gcells[i].n * sizeof (coor));

		for (j = 0; j < gcells[i].n; j++) {
			read_integer(data, gcells[i].alt[j][0]);
			read_integer(data, gcells[i].alt[j][1]);
		}

		read_integer(data, gcells[i].left);
		read_integer(data, gcells[i].above);
		read_integer(data, gcells[i].next);
	}

	if (data) {
		read_integer(data, solution);
	}
}

static void write_outputs(void) {
	int i, j;

	bots_message("Minimum area = %d\n\n", MIN_AREA);

	for (i = 0; i < MIN_FOOTPRINT[0]; i++) {
		for (j = 0; j < MIN_FOOTPRINT[1]; j++) {
			if (BEST_BOARD[i][j] == 0) {
				bots_message(" ");
			} else bots_message("%c", 'A' + BEST_BOARD[i][j] - 1);
		}
		bots_message("\n");
	}
}

// declare task prototypes
VOID_TASK_PROTO_10(wl_task, int*, NWS, int, i, int, j, int, id, int, nn, int, level, int*, nnc, int*, FOOTPRINT, char*, BOARD, struct cell*, CELLS);
VOID_TASK_PROTO_10(wl_task_ser, int*, NWS, int, i, int, j, int, id, int, nn, int, level, int*, nnc, int*, FOOTPRINT, char*, BOARD, struct cell*, CELLS);
LOOP_BODY_PROTO_9(task_loop, 100000, int, lp, int, i, int, id, int*, FOOTPRINT, char*, BOARD, struct cell*, CELLS, int, level, int*, NWS, int, nn, int*, nnc);
TASK_PROTO_5(int, add_cell, int, id, int*, FOOTPRINT, char*, BOARD, struct cell*, CELLS, int, level);

VOID_TASK_10(wl_task, int*, NWS, int, i, int, j, int, id, int, nn, int, level, int*, nnc, int*, FOOTPRINT, char*, BOARD, struct cell*, CELLS) {
	struct cell *cells;
	char board[ROWS][COLS];
	int footprint[2];
	int area;

	cells = alloca(sizeof (struct cell) *(N + 1));
	memcpy(cells, CELLS, sizeof (struct cell) *(N + 1));

	cells[id].top = NWS[(j * 2) + 0];
	cells[id].bot = cells[id].top + cells[id].alt[i][0] - 1;
	cells[id].lhs = NWS[(j * 2) + 1];
	cells[id].rhs = cells[id].lhs + cells[id].alt[i][1] - 1;

	memcpy(board, BOARD, sizeof (ibrd));


	if (!lay_down(id, &board[0][0], cells)) {
		bots_debug("Chip %d, shape %d does not fit\n", id, i);
		goto _end;
	}

	footprint[0] = max(FOOTPRINT[0], cells[id].bot + 1);
	footprint[1] = max(FOOTPRINT[1], cells[id].rhs + 1);
	area = footprint[0] * footprint[1];


	if (cells[id].next == 0) {


		if (area < MIN_AREA) {
			pthread_mutex_lock(&critical_l);
			if (area < MIN_AREA) {
				MIN_AREA = area;
				MIN_FOOTPRINT[0] = footprint[0];
				MIN_FOOTPRINT[1] = footprint[1];
				memcpy(BEST_BOARD, board, sizeof (ibrd));
				bots_debug("N  %d\n", MIN_AREA);
			};
			pthread_mutex_unlock(&critical_l);
		}


	} else if (area < MIN_AREA) {
		int nnc_tmp;
		nnc_tmp = CALL(add_cell, cells[id].next, footprint, &board[0][0], cells, level + 1);
		pthread_mutex_lock(&atom_l);
		(*nnc) += nnc_tmp;
		pthread_mutex_unlock(&atom_l);
	} else {
		bots_debug("T  %d, %d\n", area, MIN_AREA);
	}
_end:
	return;
}

VOID_TASK_10(wl_task_ser, int*, NWS, int, i, int, j, int, id, int, nn, int, level, int*, nnc, int*, FOOTPRINT, char*, BOARD, struct cell*, CELLS) {
	struct cell *cells;
	char board[ROWS][COLS];
	int footprint[2];
	int area;

	cells = alloca(sizeof (struct cell) *(N + 1));
	memcpy(cells, CELLS, sizeof (struct cell) *(N + 1));

	cells[id].top = NWS[(j * 2) + 0];
	cells[id].bot = cells[id].top + cells[id].alt[i][0] - 1;
	cells[id].lhs = NWS[(j * 2) + 1];
	cells[id].rhs = cells[id].lhs + cells[id].alt[i][1] - 1;

	memcpy(board, BOARD, sizeof (ibrd));


	if (!lay_down(id, &board[0][0], cells)) {
		bots_debug("Chip %d, shape %d does not fit\n", id, i);
		goto _end;
	}

	footprint[0] = max(FOOTPRINT[0], cells[id].bot + 1);
	footprint[1] = max(FOOTPRINT[1], cells[id].rhs + 1);
	area = footprint[0] * footprint[1];


	if (cells[id].next == 0) {


		if (area < MIN_AREA) {
			pthread_mutex_lock(&critical_l);
			if (area < MIN_AREA) {
				MIN_AREA = area;
				MIN_FOOTPRINT[0] = footprint[0];
				MIN_FOOTPRINT[1] = footprint[1];
				memcpy(BEST_BOARD, board, sizeof (ibrd));
				bots_debug("N  %d\n", MIN_AREA);
			}
			pthread_mutex_unlock(&critical_l);
		}


	} else if (area < MIN_AREA) {
		(*nnc) += CALL(add_cell, cells[id].next, footprint, &board[0][0], cells, level + 1);
	} else {
		bots_debug("T  %d, %d\n", area, MIN_AREA);
	}
_end:
	return;
}

LOOP_BODY_9(task_loop, 100000, int, lp, int, i, int, id, int*, FOOTPRINT, char*, BOARD, struct cell*, CELLS, int, level, int*, NWS, int, nn, int*, nnc) {
	//printf("\niteration: %d\n",lp);
	CALL(wl_task, &NWS[0], i, lp, id, nn, level, nnc, FOOTPRINT, BOARD, CELLS);
}





//int add_cell(int id, coor FOOTPRINT, char *BOARD, struct cell *CELLS,int level)

TASK_5(int, add_cell, int, id, int*, FOOTPRINT, char*, BOARD, struct cell*, CELLS, int, level) {
	int i, j, nn, nnc, nnl;//, area;


	//  coor footprint, NWS[DMAX];
//	int footprint[2];
	int *NWS = malloc(sizeof(int) *(DMAX * 2));

	nnc = nnl = 0;

	if (level < bots_cutoff_value) {
		for (i = 0; i < CELLS[id].n; i++) {
			nn = starts(id, i, NWS, CELLS);
			nnl += nn;
#if defined(FOR_VERSION)
			FOR(task_loop, 0, nn, i, id, FOOTPRINT, BOARD, CELLS, level, &NWS, nn, &nnc);
#else
			for (j = 0; j < nn; j++) {
				SPAWN(wl_task, NWS, i, j, id, nn, level, &nnc, FOOTPRINT, BOARD, CELLS);
			}
			for (j = 0; j < nn; j++)
				SYNC(wl_task);
			free(NWS);
#endif
		}
		return nnc + nnl;
	} else {
		for (i = 0; i < CELLS[id].n; i++) {
			nn = starts(id, i, NWS, CELLS);
			nnl += nn;
			for (j = 0; j < nn; j++) {
				CALL(wl_task, NWS, i, j, id, nn, level, &nnc, FOOTPRINT, BOARD, CELLS);
			}
		}
		return nnc + nnl;
	}

}

char board[ROWS][COLS];

void floorplan_init(char *data) {
	int i, j;

	/* read input file and initialize global minimum area */
	read_inputs(data);
	MIN_AREA = ROWS * COLS;

	/* initialize board is empty */
	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLS; j++) board[i][j] = 0;

}

void compute_floorplan(opt_worker_t self) {
	int *footprint = malloc(sizeof(int)*2);
	/* footprint of initial board is zero */
	footprint[0] = 0;
	footprint[1] = 0;
	bots_message("Computing floorplan ");

	bots_number_of_tasks = ROOT_CALL(add_cell, 1, &footprint[0], &board[0][0], gcells, 0);
	free(footprint);
	bots_message(" completed!\n");

}

void floorplan_end(void) {
	/* write results */
	write_outputs();
}

int floorplan_verify(void) {
	if (solution != -1)
		return MIN_AREA == solution ? BOTS_RESULT_SUCCESSFUL : BOTS_RESULT_UNSUCCESSFUL;
	else
		return BOTS_RESULT_NA;
}

int floorplan(APP_ARGS) {
	//Launch thread in the beginning of WOOL

	cycles_t bots_t_start;
	cycles_t bots_t_end;

	print("\n %d -- %s %s \n", argc, argv[0], argv[1]);
	bots_check_flag = BOTS_CONFIG;
	
	char *floorplan_data = INPUT_DATA;

	bots_set_info("Floorplan", false);

	floorplan_init(floorplan_data);

	bots_t_start = bots_usecs();
	compute_floorplan(self);
	bots_t_end = bots_usecs();
	bots_time_program = bots_t_end - bots_t_start;

	floorplan_end();

	if (bots_check_flag) {
		bots_result = floorplan_verify();
	}

	bots_print_results();

	//Join the threads together
	return (0);
}

