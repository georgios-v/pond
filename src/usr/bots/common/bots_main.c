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

/***********************************************************************
 * main function & common behavior of the benchmark.
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include "bots.h"
#include "bots_common.h"
#include "bots_main.h"

int wool_proc = 1;

/***********************************************************************
 * BENCHMARK HEADERS 
 *********************************************************************/
void bots_initialize(void);
void bots_finalize(void);
void bots_sequential_ini(void);
long bots_sequential(void);
void bots_sequential_fini(void);
int bots_check_result(void);
void bots_print_usage_specific(void);
void bots_get_params_specific(int argc, char **argv);

/***********************************************************************
 * DEFAULT VALUES 
 *********************************************************************/
/* common flags */
int bots_sequential_flag = FALSE;
int bots_check_flag = FALSE;
bots_verbose_mode_t bots_verbose_mode = BOTS_VERBOSE_DEFAULT;
int bots_result = BOTS_RESULT_NOT_REQUESTED;
int bots_output_format = 1;
int bots_print_header = FALSE;
/* common variables */
char bots_name[128];
char bots_execname[128];
char bots_parameters[128];
char bots_model[128];
char bots_resources[128];
/* compile and execution information */
char bots_exec_date[128];
char bots_exec_message[128];
//char bots_comp_date[128];
//char bots_comp_message[128];
//char bots_cc[128];
//char bots_cflags[128];
//char bots_ld[128];
//char bots_ldflags[128];
char bots_cutoff[128];

/* time variables */
size_t bots_time_program = 0;
size_t bots_time_sequential = 0;
int bots_number_of_tasks = 0;

/*
 * Application dependent info
 */

#ifndef BOTS_APP_NAME
//#error "Application name must be defined (#define BOTS_APP_NAME)"
#endif

#ifndef BOTS_APP_PARAMETERS_DESC
#define BOTS_APP_PARAMETERS_DESC ""
#endif

#ifndef BOTS_APP_PARAMETERS_LIST
#define BOTS_APP_PARAMETERS_LIST
#endif

#ifndef BOTS_APP_INIT
#define BOTS_APP_INIT
#endif

#ifndef BOTS_APP_FINI
#define BOTS_APP_FINI
#endif

#ifndef KERNEL_CALL
//#error "Initial kernell call must be specified (#define KERNEL_CALL)"
#endif

#ifndef KERNEL_INIT
#define KERNEL_INIT
#endif

#ifndef KERNEL_FINI
#define KERNEL_FINI
#endif

#ifndef KERNEL_SEQ_INIT
#define KERNEL_SEQ_INIT
#endif

#ifndef KERNEL_SEQ_FINI
#define KERNEL_SEQ_FINI
#endif

#ifndef BOTS_MODEL_DESC
#define BOTS_MODEL_DESC "Unknown"
#endif

//#ifdef BOTS_APP_USES_ARG_SIZE
//#ifndef BOTS_APP_DEF_ARG_SIZE
//#error "Default vaule for argument size must be specified (#define BOTS_APP_DEF_ARG_SIZE)"
//#endif
//#ifndef BOTS_APP_DESC_ARG_SIZE
//#error "Help description for argument size must be specified (#define BOTS_APP_DESC_ARG_SIZE)"
//#endif
int bots_arg_size = 0;
//#endif

//#ifdef BOTS_APP_USES_ARG_SIZE_1
//#ifndef BOTS_APP_DEF_ARG_SIZE_1
//#error "Default vaule for argument size must be specified (#define BOTS_APP_DEF_ARG_SIZE_1)"
//#endif
//#ifndef BOTS_APP_DESC_ARG_SIZE_1
//#error "Help description for argument size must be specified (#define BOTS_APP_DESC_ARG_SIZE_1)"
//#endif
int bots_arg_size_1 = 0;
//#endif

//#ifdef BOTS_APP_USES_ARG_SIZE_2
//#ifndef BOTS_APP_DEF_ARG_SIZE_2
//#error "Default vaule for argument size must be specified (#define BOTS_APP_DEF_ARG_SIZE_2)"
//#endif
//#ifndef BOTS_APP_DESC_ARG_SIZE_2
//#error "Help description for argument size must be specified (#define BOTS_APP_DESC_ARG_SIZE_2)"
//#endif
int bots_arg_size_2 = 0;
//#endif

//#ifdef BOTS_APP_USES_ARG_REPETITIONS
//#ifndef BOTS_APP_DEF_ARG_REPETITIONS
//#error "Default vaule for argument repetitions must be specified (#define BOTS_APP_DEF_ARG_REPETITIONS)"
//#endif
//#ifndef BOTS_APP_DESC_ARG_REPETITIONS
//#error "Help description for argument repetitions must be specified (#define BOTS_APP_DESC_ARG_REPETITIONS)"
//#endif
int bots_arg_repetitions = 0;
//#endif

//#ifdef BOTS_APP_USES_ARG_FILE
//#ifndef BOTS_APP_DESC_ARG_FILE
//#error "Help description for argument file must be specified (#define BOTS_APP_DESC_ARG_FILE)"
//#endif
char bots_arg_file[255] = "";
//#endif

//#ifdef BOTS_APP_USES_ARG_BLOCK
//#ifndef BOTS_APP_DEF_ARG_BLOCK
//#error "Default value for argument block must be specified (#define BOTS_APP_DEF_ARG_BLOCK)"
//#endif
//#ifndef BOTS_APP_DESC_ARG_BLOCK
//#error "Help description for argument block must be specified (#define BOTS_APP_DESC_ARG_BLOCK)"
//#endif
int bots_arg_block = 0;
//#endif

//#ifdef BOTS_APP_USES_ARG_CUTOFF
//#ifndef BOTS_APP_DEF_ARG_CUTOFF
//#error "Default value for argument cutoff  must be specified (#define BOTS_APP_DEF_ARG_CUTOFF)"
//#endif
//#ifndef BOTS_APP_DESC_ARG_CUTOFF
//#error "Help description for argument cutoff must be specified (#define BOTS_APP_DESC_ARG_CUTOFF)"
//#endif
int bots_app_cutoff_value = 0;
//#endif

//#ifdef BOTS_APP_USES_ARG_CUTOFF_1
//#ifndef BOTS_APP_DEF_ARG_CUTOFF_1
//#error "Default value for argument cutoff  must be specified (#define BOTS_APP_DEF_ARG_CUTOFF_1)"
//#endif
//#ifndef BOTS_APP_DESC_ARG_CUTOFF_1
//#error "Help description for argument cutoff must be specified (#define BOTS_APP_DESC_ARG_CUTOFF_1)"
//#endif
int bots_app_cutoff_value_1 = 0;
//#endif

//#ifdef BOTS_APP_USES_ARG_CUTOFF_2
//#ifndef BOTS_APP_DEF_ARG_CUTOFF_2
//#error "Default value for argument cutoff  must be specified (#define BOTS_APP_DEF_ARG_CUTOFF_2)"
//#endif
//#ifndef BOTS_APP_DESC_ARG_CUTOFF_2
//#error "Help description for argument cutoff must be specified (#define BOTS_APP_DESC_ARG_CUTOFF_2)"
//#endif
int bots_app_cutoff_value_2 = 0;
//#endif

//#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF) || defined(FINAL_CUTOFF)
int bots_cutoff_value = 0;
//#endif

/***********************************************************************
 * print_usage: 
 **********************************************************************/
void bots_print_usage(void) {
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s -[options]\n", bots_execname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Where options are:\n");

	if (bots_arg_repetitions)
		fprintf(stderr, "  -r <value> : Set the number of repetitions (default = 1).\n");

	if (bots_arg_size)
		fprintf(stderr, "  -n <size>  : %d\n", bots_arg_size);

	if (bots_arg_size_1)
		fprintf(stderr, "  -m <size>  : %d\n", bots_arg_size_1);

	if (bots_arg_size_2)
		fprintf(stderr, "  -l <size>  : %d\n", bots_arg_size_2);

	if (bots_arg_file[0] != '\0')
		fprintf(stderr, "  -f <file>  \n");
	if (bots_cutoff_value)
		fprintf(stderr, "  -x <value> : OpenMP tasks cut-off value (default=%d)\n", bots_cutoff_value);
	if (bots_app_cutoff_value)
		fprintf(stderr, "  -y <value> : (default=%d)\n", bots_app_cutoff_value);
	if (bots_app_cutoff_value_1)
		fprintf(stderr, "  -a <value> : (default=%d)\n", bots_app_cutoff_value_1);
	if (bots_app_cutoff_value_2)
		fprintf(stderr, "  -b <value> : (default=%d)\n", bots_app_cutoff_value_2);


	fprintf(stderr, "\n");
	fprintf(stderr, "  -e <str>   : Include 'str' execution message.\n");
	fprintf(stderr, "  -v <level> : Set verbose level (default = 1).\n");
	fprintf(stderr, "               0 - none.\n");
	fprintf(stderr, "               1 - default.\n");
	fprintf(stderr, "               2 - debug.\n");
	fprintf(stderr, "  -o <value> : Set output format mode (default = 1).\n");
	fprintf(stderr, "               0 - no benchmark output.\n");
	fprintf(stderr, "               1 - detailed list format.\n");
	fprintf(stderr, "               2 - detailed row format.\n");
	fprintf(stderr, "               3 - abridged list format.\n");
	fprintf(stderr, "               4 - abridged row format.\n");
	fprintf(stderr, "  -z         : Print row header (if output format is a row variant).\n");
	fprintf(stderr, "\n");
//	if (kernel_seq_call)
//		fprintf(stderr, "  -s         : Run sequential version.\n");
//	if (bots_app_check_uses_seq_result)
//		fprintf(stderr, "  -c         : Check mode ON (implies running sequential version).\n");
//	else
//		fprintf(stderr, "  -c         : Check mode ON.\n");

	fprintf(stderr, "\n");
	fprintf(stderr, "  -h         : Print program's usage (this help).\n");
	fprintf(stderr, "\n");
}

/***********************************************************************
 * bots_get_params_common: 
 **********************************************************************/
//void bots_get_params_common(int argc, char **argv, uint32_t bots_app_def_arg_size, uint16_t bots_app_def_arg_cutoff, uint8_t bots_cutoff_def_value);
//
//void bots_get_params_common(int argc, char **argv, uint32_t bots_app_def_arg_size, uint16_t bots_app_def_arg_cutoff, uint8_t bots_cutoff_def_value) {
//	int i;
//	strcpy(bots_execname, basename(argv[0]));
//	bots_get_date(bots_exec_date);
//	strcpy(bots_exec_message, "");
//	for (i = 1; i < argc; i++) {
//		if (argv[i][0] == '-') {
//			switch (argv[i][1]) {
//				case 'a':
//					argv[i][1] = '*';
//					i++;
//					if (argc == i) {
//						bots_print_usage();
//						exit(100);
//					}
//					bots_app_cutoff_value_1 = atoi(argv[i]);
//					break;
//				case 'b':
//					argv[i][1] = '*';
//					i++;
//					if (argc == i) {
//						bots_print_usage();
//						exit(100);
//					}
//					bots_app_cutoff_value_2 = atoi(argv[i]);
//					break;
//				case 'c': /* set/unset check mode */
//					argv[i][1] = '*';
//					//i++;
//					//if (argc == i) { bots_print_usage(); exit(100); }
//					//bots_check_flag = atoi(argv[i]);
//					bots_check_flag = TRUE;
//					break;
//				case 'e': /* include execution message */
//					argv[i][1] = '*';
//					i++;
//					if (argc == i) {
//						bots_print_usage();
//						exit(100);
//					}
//					strcpy(bots_exec_message, argv[i]);
//					break;
//				case 'f': /* read argument file name */
//					argv[i][1] = '*';
//					i++;
//					if (argc == i) {
//						bots_print_usage();
//						exit(100);
//					}
//					strcpy(bots_arg_file, argv[i]);
//					break;
//				case 'h': /* print usage */
//					argv[i][1] = '*';
//					bots_print_usage();
//					exit(100);
//				case 'l': /* read argument size 2 */
//					argv[i][1] = '*';
//					i++;
//					if (argc == i) {
//						bots_print_usage();
//						exit(100);
//					}
//					bots_arg_size_2 = atoi(argv[i]);
//					break;
//				case 'm': /* read argument size 1 */
//					argv[i][1] = '*';
//					i++;
//					if (argc == i) {
//						bots_print_usage();
//						exit(100);
//					}
//					bots_arg_size_1 = atoi(argv[i]);
//					break;
//				case 'n': /* read argument size 0 */
//					argv[i][1] = '*';
//					i++;
//					if (argc == i) {
//						bots_print_usage();
//						exit(100);
//					}
//					bots_arg_size = atoi(argv[i]);
//					break;
//				case 'o': /* set output mode */
//					argv[i][1] = '*';
//					i++;
//					if (argc == i) {
//						bots_print_usage();
//						exit(100);
//					}
//					bots_output_format = atoi(argv[i]);
//					break;
//				case 'r': /* set number of repetitions */
//					argv[i][1] = '*';
//					i++;
//					if (argc == i) {
//						bots_print_usage();
//						exit(100);
//					}
//					bots_arg_repetitions = atoi(argv[i]);
//					break;
//				case 's': /* set sequential execution */
//					argv[i][1] = '*';
//					//i++;
//					//if (argc == i) { bots_print_usage(); exit(100); }
//					//bots_sequential_flag = atoi(argv[i]);
//					bots_sequential_flag = TRUE;
//					break;
//				case 'v': /* set/unset verbose level */
//					argv[i][1] = '*';
//					i++;
//					if (argc == i) {
//						bots_print_usage();
//						exit(100);
//					}
//					bots_verbose_mode = (bots_verbose_mode_t) atoi(argv[i]);
//					if (bots_verbose_mode > 1) {
//						fprintf(stderr, "Error: Configure the suite using '--debug' option in order to use a verbose level greather than 1.\n");
//						exit(100);
//					}
//					break;
//				case 'x':
//					argv[i][1] = '*';
//					i++;
//					if (argc == i) {
//						bots_print_usage();
//						exit(100);
//					}
//					bots_cutoff_value = atoi(argv[i]);
//					break;
//				case 'y':
//					argv[i][1] = '*';
//					i++;
//					if (argc == i) {
//						bots_print_usage();
//						exit(100);
//					}
//					bots_app_cutoff_value = atoi(argv[i]);
//					break;
//				case 'z':
//					argv[i][1] = '*';
//					bots_print_header = TRUE;
//					break;
//				default:
//					// As at the moment there are only common paramenters
//					// we launch an error. Otherwise we have to ignore the
//					// parameter and to check, after specific parameters are
//					// completely read, if there are unrecognized parameters.
////					fprintf(stderr, "Error: Unrecognized parameter.\n");
////					bots_print_usage();
////					exit(100);
//					break;
//			}
//		} else {
//			// As at the moment there are only common paramenters
//			// we launch an error. Otherwise we have to ignore the
//			// parameter and to check, after specific parameters are
//			// completely read, if there are unrecognized parameters.
//			fprintf(stderr, "Error: Unrecognized parameter.\n");
//			bots_print_usage();
//			exit(100);
//		}
//	}
//}
//
///***********************************************************************
// * bots_get_params_common: 
// **********************************************************************/
//void bots_get_params(int argc, char **argv);
//
//void bots_get_params(int argc, char **argv, uint32_t bots_app_def_arg_size, uint16_t bots_app_def_arg_cutoff, uint8_t bots_cutoff_def_value) {
//	bots_get_params_common(argc, argv, uint32_t bots_app_def_arg_size, uint16_t bots_app_def_arg_cutoff, uint8_t bots_cutoff_def_value);
//	//   bots_get_params_specific(argc, argv);
//}

/***********************************************************************
 * bots_set_info 
 **********************************************************************/
void bots_set_info(char *bots_app_name, bool final_cutoff) {
	/* program specific info */
	sprintf(bots_name, "%s", bots_app_name);
//	sprintf(bots_parameters, BOTS_APP_PARAMETERS_DESC BOTS_APP_PARAMETERS_LIST);
//	sprintf(bots_model, BOTS_MODEL_DESC);
//	sprintf(bots_resources, "%d", omp_get_max_threads());

	/* compilation info (do not modify) */
//	strcpy(bots_comp_date, CDATE);
//	strcpy(bots_comp_message, CMESSAGE);
//	strcpy(bots_cc, CC);
//	strcpy(bots_cflags, CFLAGS);
//	strcpy(bots_ld, LD);
//	strcpy(bots_ldflags, LDFLAGS);

if(final_cutoff)
	sprintf(bots_cutoff, "final (%d)", bots_cutoff_value);
else
	strcpy(bots_cutoff, "none");

}

/***********************************************************************
 * main: 
 **********************************************************************/
//
//typedef void (*bots_t)(void);
//
////Cilk want's cilk_main so we give it
//#ifdef CILK
//int cilk_main(int argc, char* argv[])
//#else
////TASK_2(int,main, int, argc, char**, argv)
//
////int main(int argc, char* argv[])
//void bots(bots_t app_init, bots_t k_seq, bots_t k_init, bots_t k_call)
//#endif
//{
//
//	//Launch thread in the beginning of WOOL
//
//	WOOL_INIT;
//
//	print("\n %d -- %s %s \n", argc, argv[0], argv[1]);
//	bots_get_params(argc, argv);
//
//
//	BOTS_APP_INIT;
//	bots_set_info();
//
//
//#ifdef KERNEL_SEQ_CALL
//#ifdef BOTS_APP_CHECK_USES_SEQ_RESULT
//	if (bots_sequential_flag || bots_check_flag)
//#else
//	if (bots_sequential_flag)
//#endif
//	{
//		bots_sequential_flag = 1;
//		KERNEL_SEQ_INIT;
//		
//		bots_t_start = bots_usecs();
//		KERNEL_SEQ_CALL;
//		bots_t_end = bots_usecs();
//		bots_time_sequential = ((double) (bots_t_end - bots_t_start)) / 1000000;
//
//		KERNEL_SEQ_FINI;
//	}
//#endif
//
//	KERNEL_INIT;
//
//	bots_t_start = bots_usecs();
//	KERNEL_CALL;
//	bots_t_end = bots_usecs();
//	bots_time_program = ((double) (bots_t_end - bots_t_start)) / 1000000;
//
//	KERNEL_FINI;
//
//#ifdef KERNEL_CHECK
//	if (bots_check_flag) {
//		bots_result = KERNEL_CHECK;
//	}
//#endif
//
//	BOTS_APP_FINI;
//
//	bots_print_results();
//
//
//	WOOL_FINI;
//	//Join the threads together
//
//
//	return (0);
//}

