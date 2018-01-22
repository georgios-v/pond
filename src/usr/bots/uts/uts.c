/**********************************************************************************************/
/*  This program is part of the Barcelona OpenMP Tasks Suite                                  */
/*  Copyright (C) 2009 Barcelona Supercomputing Center - Centro Nacional de Supercomputacion  */
/*  Copyright (C) 2009 Universitat Politecnica de Catalunya                                   */
/**********************************************************************************************/
/*
 * Copyright (c) 2007 The Unbalanced Tree Search (UTS) Project Team:
 * -----------------------------------------------------------------
 *  
 *  This file is part of the unbalanced tree search benchmark.  This
 *  project is licensed under the MIT Open Source license.  See the LICENSE
 *  file for copyright and licensing information.
 *
 *  UTS is a collaborative project between researchers at the University of
 *  Maryland, the University of North Carolina at Chapel Hill, and the Ohio
 *  State University.
 *
 * University of Maryland:
 *   Chau-Wen Tseng(1)  <tseng at cs.umd.edu>
 *
 * University of North Carolina, Chapel Hill:
 *   Jun Huan         <huan,
 *   Jinze Liu         liu,
 *   Stephen Olivier   olivier,
 *   Jan Prins*        prins at cs.umd.edu>
 * 
 * The Ohio State University:
 *   James Dinan      <dinan,
 *   Gerald Sabin      sabin,
 *   P. Sadayappan*    saday at cse.ohio-state.edu>
 *
 * Supercomputing Research Center
 *   D. Pryor
 *
 * (1) - indicates project PI
 *
 * UTS Recursive Depth-First Search (DFS) version developed by James Dinan
 *
 * Adapted for OpenMP 3.0 Task-based version by Stephen Olivier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../common/bots.h"
#include "uts.h"


/*
1 TEST
2 TINY
3 SMALL
4 MEDIUM
5 LARGE
6 HUGE
 */
#define UTS_INPUT 3

#if (UTS_INPUT == 1)
#define UTS_INPUT_DATA "2000 0.124875 8 42 1 4112897 1572 3599034";
#elif (UTS_INPUT == 2)
#define UTS_INPUT_DATA "2000 0.333332 3 8 1 30399116 1 1";
#elif (UTS_INPUT == 3)
#define UTS_INPUT_DATA "2000 0.200014 5 7 1 111345631 17844 89076904";
#elif (UTS_INPUT == 4)
#define UTS_INPUT_DATA "2000 0.333344 3 23 1 1606963584 1 1";
#elif (UTS_INPUT == 5)
#define UTS_INPUT_DATA "2000 0.499999995 2 0 1 10612052303 216370 5306027151";
#elif (UTS_INPUT == 6)
#define UTS_INPUT_DATA "22000 0.4999995 2 559 1 157063495159 758577 78531748579";
#endif




/***********************************************************
 *  Global state                                           *
 ***********************************************************/
counter_t nLeaves = 0;
int maxTreeDepth = 0;
/***********************************************************
 *  tree generation and search parameters                  *
 *                                                         *
 *  Tree generation strategy is controlled via various     *
 *  parameters set from the command line.  The parameters  *
 *  and their default values are given below.              *
 ***********************************************************/
char *uts_trees_str[] = {"Binomial"};
/***********************************************************
 * Tree type
 *   Trees are generated using a Galton-Watson process, in 
 *   which the branching factor of each node is a random 
 *   variable.
 *   
 *   The random variable follow a binomial distribution.
 ***********************************************************/
tree_t type = BIN; // Default tree type
double b_0 = 4.0; // default branching factor at the root
int rootId = 0; // default seed for RNG state at root
/***********************************************************
 *  Tree type BIN (BINOMIAL)
 *  The branching factor at the root is specified by b_0.
 *  The branching factor below the root follows an 
 *     identical binomial distribution at all nodes.
 *  A node has m children with prob q, or no children with 
 *     prob (1-q).  The expected branching factor is q * m.
 *
 *  Default parameter values 
 ***********************************************************/
int nonLeafBF = 4; // m
double nonLeafProb = 15.0 / 64.0; // q
/***********************************************************
 * compute granularity - number of rng evaluations per
 * tree node
 ***********************************************************/
int computeGranularity = 1;
/***********************************************************
 * expected results for execution
 ***********************************************************/
counter_t exp_tree_size = 0;
int exp_tree_depth = 0;
counter_t exp_num_leaves = 0;
/***********************************************************
 *  FUNCTIONS                                              *
 ***********************************************************/

// Interpret 32 bit positive integer as value on [0,1)
VOID_TASK_PROTO_4(parTreeSearch, int, depth, Node*, parent, int, numChildren, counter_t*, save);
LOOP_BODY_PROTO_4(iteration, 10000, int, i, Node*, n, counter_t*, partialCount, Node*, parent, int, depth);

double rng_toProb(int n) {
	if (n < 0) {
		printf("*** toProb: rand n = %d out of range\n", n);
	}
	return ((n < 0) ? 0.0 : ((double) n) / 2147483648.0);
}

void uts_initRoot(Node * root, int mytype) {
	root->height = 0;
	root->numChildren = -1; // means not yet determined
	rng_init(root->state.state, rootId);

	bots_message("Root node of type %d at %p\n", mytype, root);
}

int uts_numChildren_bin(Node * parent) {
	// distribution is identical everywhere below root
	int v = rng_rand(parent->state.state);
	double d = rng_toProb(v);

	return (d < nonLeafProb) ? nonLeafBF : 0;
}

int uts_numChildren(Node *parent) {
	int numChildren = 0;

	/* Determine the number of children */
	if (parent->height == 0) numChildren = (int) floor(b_0);
	else numChildren = uts_numChildren_bin(parent);

	// limit number of children
	// only a BIN root can have more than MAXNUMCHILDREN
	if (parent->height == 0) {
		int rootBF = (int) ceil(b_0);
		if (numChildren > rootBF) {
			bots_debug("*** Number of children of root truncated from %d to %d\n", numChildren, rootBF);
			numChildren = rootBF;
		}
	} else {
		if (numChildren > MAXNUMCHILDREN) {
			bots_debug("*** Number of children truncated from %d to %d\n", numChildren, MAXNUMCHILDREN);
			numChildren = MAXNUMCHILDREN;
		}
	}

	return numChildren;
}

/***********************************************************
 * Recursive depth-first implementation                    *
 ***********************************************************/

static int getNumRootChildren(Node *root) {
	int numChildren;

	numChildren = uts_numChildren(root);
	root->numChildren = numChildren;

	return numChildren;
}

#define FOR_VERSION

//counter_t parTreeSearch(int depth, Node *parent, int numChildren,counter_t *save)

VOID_TASK_4(parTreeSearch, int, depth, Node*, parent, int, numChildren, counter_t*, save) {
	Node n[numChildren];
	int i;
	counter_t subtreesize = 1, partialCount[numChildren];

	// Recurse on the children

#if defined(FOR_VERSION)
	FOR(iteration, 0, numChildren, n, partialCount, parent, depth);
#else
	Node *nodePtr;
	int j;
	for (i = 0; i < numChildren; i++) {
		nodePtr = &n[i];
		nodePtr->height = parent->height + 1;
		// The following line is the work (one or more SHA-1 ops)
		for (j = 0; j < computeGranularity; j++) {
			rng_spawn(parent->state.state, nodePtr->state.state, i);
		}
		nodePtr->numChildren = uts_numChildren(nodePtr);
		SPAWN(parTreeSearch, depth + 1, nodePtr, nodePtr->numChildren, &partialCount[i]);
	}


	for (i = 0; i < numChildren; i++){
		SYNC(parTreeSearch);
	}

#endif

	for (i = 0; i < numChildren; i++) {
		subtreesize += partialCount[i];
	}

	//  return subtreesize;
	(*save) = subtreesize;
}

LOOP_BODY_4(iteration, 10000, int, i, Node*, n, counter_t*, partialCount, Node*, parent, int, depth) {
	int j;
	Node *nodePtr;

	nodePtr = &n[i];
	nodePtr->height = parent->height + 1;
	// The following line is the work (one or more SHA-1 ops)
	for (j = 0; j < computeGranularity; j++) {
		rng_spawn(parent->state.state, nodePtr->state.state, i);
	}
	nodePtr->numChildren = uts_numChildren(nodePtr);
	CALL(parTreeSearch, depth + 1, nodePtr, nodePtr->numChildren, &partialCount[i]);
}

static counter_t parallel_uts(opt_worker_t self, Node *root) {
	counter_t num_nodes;

	bots_message("Computing Unbalanced Tree Search algorithm ");

	ROOT_CALL(parTreeSearch, 0, root, getNumRootChildren(root), &num_nodes);

	bots_message(" completed!");
	return num_nodes;
}

void uts_read_file(char *input) {
	sscanf(input, "%lf %lf %d %d %d %llu %d %llu",
			&b_0,
			&nonLeafProb,
			&nonLeafBF,
			&rootId,
			&computeGranularity,
			&exp_tree_size,
			&exp_tree_depth,
			&exp_num_leaves
			);

	computeGranularity = max(1, computeGranularity);

	// Printing input data
	bots_message("\n");
	bots_message("Root branching factor                = %f\n", b_0);
	bots_message("Root seed (0 <= 2^31)                = %d\n", rootId);
	bots_message("Probability of non-leaf node         = %f\n", nonLeafProb);
	bots_message("Number of children for non-leaf node = %d\n", nonLeafBF);
	bots_message("E(n)                                 = %f\n", (double) (nonLeafProb * nonLeafBF));
	bots_message("E(s)                                 = %f\n", (double) (1.0 / (1.0 - nonLeafProb * nonLeafBF)));
	bots_message("Compute granularity                  = %d\n", computeGranularity);
	bots_message("Tree type                            = %d (%s)\n", type, uts_trees_str[type]);
	bots_message("Random number generator              = ");
	rng_showtype();
}

static void uts_show_stats(void) {
	int nPes = atoi(bots_resources);
	int chunkSize = 0;

	bots_message("\n");
	bots_message("Tree size                            = %llu\n", (unsigned long long) bots_number_of_tasks);
	bots_message("Maximum tree depth                   = %d\n", maxTreeDepth);
	bots_message("Chunk size                           = %d\n", chunkSize);
	bots_message("Number of leaves                     = %llu (%.2f%%)\n", nLeaves, nLeaves / (float) bots_number_of_tasks * 100.0);
	bots_message("Number of PE's                       = %.4d threads\n", nPes);
	bots_message("Wallclock time                       = %lu sec\n", bots_time_program);
	bots_message("Overall performance                  = %lu nodes/sec\n", (bots_number_of_tasks / bots_time_program));
	bots_message("Performance per PE                   = %lu nodes/sec\n", (bots_number_of_tasks / bots_time_program / nPes));
}

static int uts_check_result(void) {
	int answer = BOTS_RESULT_SUCCESSFUL;

	if (bots_number_of_tasks != exp_tree_size) {
		answer = BOTS_RESULT_UNSUCCESSFUL;
		bots_message("Tree size value is non valid.\n");
	}

	return answer;
}

int uts(APP_ARGS) {
	//Launch thread in the beginning of WOOL

	cycles_t bots_t_start;
	cycles_t bots_t_end;

//	print("\n %d -- %s %s \n", argc, argv[0], argv[1]);
	char *uts_arg_file = UTS_INPUT_DATA;
	bots_check_flag = BOTS_CONFIG;

	Node root;
	uts_read_file(uts_arg_file);
	bots_set_info("Unbalanced Tree Search", false);

	uts_initRoot(&root, type);

	bots_t_start = bots_usecs();
	bots_number_of_tasks = parallel_uts(self, &root);
	bots_t_end = bots_usecs();
	bots_time_program = bots_t_end - bots_t_start;

	uts_show_stats();

	if (bots_check_flag) {
		bots_result = uts_check_result();
	}

	bots_print_results();

	return (0);
}
