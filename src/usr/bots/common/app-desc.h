//alignment:
#ifdef __BOTS_APP_ALIGNMENT
#include "../alignment/alignment.h"
#define BOTS_APP_NAME "Protein alignment (Single version)"
#define BOTS_APP_PARAMETERS_DESC "%s"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_file
#define BOTS_APP_USES_ARG_FILE
#define BOTS_APP_DESC_ARG_FILE "Protein sequences file (mandatory)"
#define BOTS_APP_INIT pairalign_init(bots_arg_file)
#define KERNEL_INIT align_init()
#define KERNEL_CALL align();
#define KERNEL_FINI align_end()
#define KERNEL_SEQ_INIT align_seq_init()
#define KERNEL_SEQ_CALL align_seq()
#define KERNEL_CHECK align_verify()
#define BOTS_APP_CHECK_USES_SEQ_RESULT
#endif


//fft:
#ifdef __BOTS_APP_FFT
#include "../fft/fft.h"
#define BOTS_APP_NAME "FFT"
#define BOTS_APP_PARAMETERS_DESC "Size=%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size
#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE 32*1024*1024
#define BOTS_APP_DESC_ARG_SIZE "Matrix Size"
#define BOTS_APP_INIT int i;\
     COMPLEX *in, *out1, *out2;\
     in = (COMPLEX*) malloc(bots_arg_size * sizeof(COMPLEX));
#define KERNEL_INIT\
  out1 = (COMPLEX*) malloc(bots_arg_size * sizeof(COMPLEX));	\
     for (i = 0; i < bots_arg_size; ++i) {\
          c_re(in[i]) = 1.0;\
          c_im(in[i]) = 1.0;\
     }
#define KERNEL_CALL fft(bots_arg_size, in, out1);
#define KERNEL_FINI 
#define KERNEL_SEQ_INIT\
  out2 = (COMPLEX*) malloc(bots_arg_size * sizeof(COMPLEX));	\
     for (i = 0; i < bots_arg_size; ++i) {\
          c_re(in[i]) = 1.0;\
          c_im(in[i]) = 1.0;\
     }
#define KERNEL_SEQ_CALL fft_seq(bots_arg_size, in, out2);
#define KERNEL_SEQ_FINI
#define BOTS_APP_CHECK_USES_SEQ_RESULT
#define KERNEL_CHECK test_correctness(bots_arg_size, out1, out2)
#endif


//fib:
#ifdef __BOTS_APP_FIB
#include "../fib/fib.h"
#define BOTS_APP_NAME "Fibonacci"
#define BOTS_APP_PARAMETERS_DESC "N=%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size
#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE 10
#define BOTS_APP_DESC_ARG_SIZE "Number to compute"
#define KERNEL_CALL fib0(bots_arg_size)
#define KERNEL_SEQ_CALL fib0_seq(bots_arg_size)
#define KERNEL_CHECK fib_verify(bots_arg_size)
#define BOTS_CUTOFF_DEF_VALUE 10
#endif


//floorplan:
#ifdef __BOTS_APP_FLOORPLAN
#include "../floorplan/floorplan.h"
#define BOTS_APP_NAME "Floorplan"
#define BOTS_APP_PARAMETERS_DESC "%s"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_file
#define BOTS_APP_USES_ARG_FILE
#define BOTS_APP_DESC_ARG_FILE "Cell description file (mandatory)"
#define BOTS_CUTOFF_DEF_VALUE 5
#define KERNEL_INIT floorplan_init(bots_arg_file)
#define KERNEL_CALL compute_floorplan()
#define KERNEL_FINI floorplan_end()
#define KERNEL_CHECK floorplan_verify()
#define FINAL_CUTOFF
#endif


//health:
#ifdef __BOTS_APP_HEALTH
#include "../health/health.h"
#define BOTS_APP_NAME "Health"
#define BOTS_APP_PARAMETERS_DESC "%s"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_file
#define BOTS_APP_USES_ARG_FILE
#define BOTS_APP_DEF_ARG_FILE "Input filename"
#define BOTS_APP_DESC_ARG_FILE "Health input file (mandatory)"
#define BOTS_CUTOFF_DEF_VALUE 2
#define BOTS_APP_INIT \
   struct Village *top;\
   read_input_data(bots_arg_file);
#define KERNEL_INIT \
   allocate_village(&top, NULL, NULL, sim_level, 0);
#define KERNEL_CALL sim_village_main_par(top);
 
#define KERNEL_FINI
#define KERNEL_CHECK check_village(top);
#endif


//sort:
#ifdef __BOTS_APP_SORT
#include "../sort/sort.h"
#define BOTS_APP_NAME "Sort"
#define BOTS_APP_PARAMETERS_DESC "N=%d:Q=%d:I=%d:M=%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size,bots_app_cutoff_value_1,bots_app_cutoff_value_2,bots_app_cutoff_value
#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE (32*1024*1024)
#define BOTS_APP_DESC_ARG_SIZE "Array size"
#define BOTS_APP_USES_ARG_CUTOFF
#define BOTS_APP_DEF_ARG_CUTOFF (2*1024)
#define BOTS_APP_DESC_ARG_CUTOFF "Sequential Merge cutoff value"
#define BOTS_APP_USES_ARG_CUTOFF_1
#define BOTS_APP_DEF_ARG_CUTOFF_1 (2*1024)
#define BOTS_APP_DESC_ARG_CUTOFF_1 "Sequential Quicksort cutoff value"
#define BOTS_APP_USES_ARG_CUTOFF_2
#define BOTS_APP_DEF_ARG_CUTOFF_2 (20)
#define BOTS_APP_DESC_ARG_CUTOFF_2 "Sequential Insertion cutoff value"
#define BOTS_APP_INIT sort_init()
#define KERNEL_INIT
#define KERNEL_CALL sort_par()
#define KERNEL_CHECK sort_verify()
#endif


//nqueens:
#ifdef __BOTS_APP_NQUEENS
#include "../nqueens/nqueens.h"
#define BOTS_APP_NAME "N Queens"
#define BOTS_APP_PARAMETERS_DESC "N=%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size
#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE 13
#define BOTS_APP_DESC_ARG_SIZE "Board size"
#define KERNEL_CALL find_queens(bots_arg_size)
#define KERNEL_CHECK verify_queens(bots_arg_size)
#define BOTS_CUTOFF_DEF_VALUE 3
#endif


//sort:
#ifdef __BOTS_APP_SORT
#include "../sort/sort.h"
#define BOTS_APP_NAME "Sort"
#define BOTS_APP_PARAMETERS_DESC "N=%d:Q=%d:I=%d:M=%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size,bots_app_cutoff_value_1,bots_app_cutoff_value_2,bots_app_cutoff_value
#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE (32*1024*1024)
#define BOTS_APP_DESC_ARG_SIZE "Array size"
#define BOTS_APP_USES_ARG_CUTOFF
#define BOTS_APP_DEF_ARG_CUTOFF (2*1024)
#define BOTS_APP_DESC_ARG_CUTOFF "Sequential Merge cutoff value"
#define BOTS_APP_USES_ARG_CUTOFF_1
#define BOTS_APP_DEF_ARG_CUTOFF_1 (2*1024)
#define BOTS_APP_DESC_ARG_CUTOFF_1 "Sequential Quicksort cutoff value"
#define BOTS_APP_USES_ARG_CUTOFF_2
#define BOTS_APP_DEF_ARG_CUTOFF_2 (20)
#define BOTS_APP_DESC_ARG_CUTOFF_2 "Sequential Insertion cutoff value"
#define BOTS_APP_INIT sort_init()
#define KERNEL_INIT
#define KERNEL_CALL sort_par()
#define KERNEL_CHECK sort_verify()
#endif


//sparselu:
#ifdef __BOTS_APP_SPARSELU
#include "../sparselu/sparselu.h"
#define BOTS_APP_NAME "SparseLU (Single version)"
#define BOTS_APP_PARAMETERS_DESC "S1=%dx%d, S2=%dx%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size,bots_arg_size,bots_arg_size_1,bots_arg_size_1
#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE 50
#define BOTS_APP_DESC_ARG_SIZE "Matrix Size"
#define BOTS_APP_USES_ARG_SIZE_1
#define BOTS_APP_DEF_ARG_SIZE_1 100
#define BOTS_APP_DESC_ARG_SIZE_1 "Submatrix Size"
#define BOTS_APP_INIT float **seq,**BENCH;
#define KERNEL_INIT sparselu_init(&BENCH,"benchmark");
#define KERNEL_CALL sparselu_par_call(BENCH);
#define KERNEL_FINI sparselu_fini(BENCH,"benchmark");
#define KERNEL_SEQ_INIT sparselu_init(&seq,"serial");
#define KERNEL_SEQ_CALL sparselu_seq_call(seq);
#define KERNEL_SEQ_FINI sparselu_fini(seq,"serial");
#define BOTS_APP_CHECK_USES_SEQ_RESULT
#define KERNEL_CHECK sparselu_check(seq,BENCH);
#endif


//strassen:
#ifdef __BOTS_APP_STRASSEN
#include "../strassen/strassen.h"
#define BOTS_APP_NAME "Strassen"
#define BOTS_APP_PARAMETERS_DESC "N=%d:Y=%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size,bots_app_cutoff_value
#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE 1024
#define BOTS_APP_DESC_ARG_SIZE "Matrix Size"
#define BOTS_APP_USES_ARG_BLOCK
#define BOTS_APP_DEF_ARG_BLOCK 32
#define BOTS_APP_DESC_ARG_BLOCK "Matrix Block Size"
#define BOTS_APP_USES_ARG_CUTOFF
#define BOTS_APP_DEF_ARG_CUTOFF 64
#define BOTS_APP_DESC_ARG_CUTOFF "Strassen Cutoff"
#define BOTS_CUTOFF_DEF_VALUE 3
#define BOTS_APP_INIT\
    double *A, *B, *C, *D;\
    if ((bots_arg_size & (bots_arg_size - 1)) != 0 || (bots_arg_size % 16) != 0) {\
        bots_message("Error: matrix size (%d) must be a power of 2 and a multiple of %d", bots_arg_size, 16);\
        exit (1);\
    }\
    A = (double *) malloc (bots_arg_size * bots_arg_size * sizeof(double));\
    B = (double *) malloc (bots_arg_size * bots_arg_size * sizeof(double));\
    C = (double *) malloc (bots_arg_size * bots_arg_size * sizeof(double));\
    D = (double *) malloc (bots_arg_size * bots_arg_size * sizeof(double));\
    init_matrix(bots_arg_size,A,bots_arg_size);\
    init_matrix(bots_arg_size,B,bots_arg_size);
#define KERNEL_CALL strassen_main_par(C,A,B,bots_arg_size);
#define KERNEL_SEQ_CALL strassen_main_seq(D,A,B,bots_arg_size);
#define BOTS_APP_CHECK_USES_SEQ_RESULT
#define KERNEL_CHECK compare_matrix(bots_arg_size,C,bots_arg_size,D,bots_arg_size);
#endif


//uts:
#ifdef __BOTS_APP_UTS
#include "../uts/uts.h"
#define BOTS_APP_NAME "Unbalance Tree Search"
#define BOTS_APP_PARAMETERS_DESC "%s"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_file
#define BOTS_APP_USES_ARG_FILE
#define BOTS_APP_DEF_ARG_FILE "Input filename"
#define BOTS_APP_DESC_ARG_FILE "UTS input file (mandatory)"
#define BOTS_APP_INIT \
  Node root; \
  uts_read_file(bots_arg_file);
#define KERNEL_INIT uts_initRoot(&root, type);
#define KERNEL_CALL bots_number_of_tasks = parallel_uts(&root);
 
#define KERNEL_FINI uts_show_stats();
#define KERNEL_CHECK uts_check_result();
#endif


