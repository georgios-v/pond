[
  build application {
			target = "bw_alignment",
			cFiles = ["bots/alignment/alignment.c","bots/alignment/sequence.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_ALIGNMENT"],addLinkFlags = [ "-Xlinker --undefined=vfs_dummy"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "woolroot","vfs","nfs","lwip","contmng","posixcompat","msun" ]},

  build application {
			target = "bw_fft",
			cFiles = ["bots/fft/fft.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_FFT"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "woolroot","posixcompat","msun" ]},

  build application {
			target = "bw_fib",
			cFiles = ["bots/fib/fib.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_FIB"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "woolroot","posixcompat" ]},

  build application {
			target = "bw_floorplan",
			cFiles = ["bots/floorplan/floorplan.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_FLOORPLAN"],addLinkFlags = [ "-Xlinker --undefined=vfs_dummy"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "woolroot","vfs","nfs","lwip","contmng","posixcompat" ]},

  build application {
			target = "bw_health",
			cFiles = ["bots/health/health.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_HEALTH"],addLinkFlags = [ "-Xlinker --undefined=vfs_dummy"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "woolroot","vfs","nfs","lwip","contmng","posixcompat" ]},

  build application {
			target = "bw_nqueens",
			cFiles = ["bots/nqueens/nqueens.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_NQUEENS"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "woolroot","posixcompat" ]},

  build application {
			target = "bw_sort",
			cFiles = ["bots/sort/sort.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_SORT"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "woolroot","posixcompat" ]},

  build application {
			target = "bw_sparselu",
			cFiles = ["bots/sparselu/sparselu.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_SPARSELU"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "woolroot","posixcompat","msun" ]},

  build application {
			target = "bw_strassen",
			cFiles = ["bots/strassen/strassen.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_STRASSEN"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "woolroot","posixcompat" ]},

  build application {
			target = "bw_uts",
			cFiles = ["bots/uts/uts.c","bots/uts/brg_sha1.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_UTS"],addLinkFlags = [ "-Xlinker --undefined=vfs_dummy"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "woolroot","vfs","nfs","lwip","contmng","posixcompat","msun" ]},





  build application {
			target = "bb_alignment",
			cFiles = ["bots/alignment/alignment.c","bots/alignment/sequence.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_ALIGNMENT"],addLinkFlags = [ "-Xlinker --undefined=vfs_dummy"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "biasusr_root","vfs","nfs","lwip","contmng","posixcompat","msun" ]},

  build application {
			target = "bb_fft",
			cFiles = ["bots/fft/fft.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_FFT"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "biasusr_root","posixcompat","msun" ]},

  build application {
			target = "bb_fib",
			cFiles = ["bots/fib/fib.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_FIB"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "biasusr_root","posixcompat" ]},

  build application {
			target = "bb_floorplan",
			cFiles = ["bots/floorplan/floorplan.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_FLOORPLAN"],addLinkFlags = [ "-Xlinker --undefined=vfs_dummy"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "biasusr_root","vfs","nfs","lwip","contmng","posixcompat" ]},

  build application {
			target = "bb_health",
			cFiles = ["bots/health/health.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_HEALTH"],addLinkFlags = [ "-Xlinker --undefined=vfs_dummy"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "biasusr_root","vfs","nfs","lwip","contmng","posixcompat" ]},

  build application {
			target = "bb_nqueens",
			cFiles = ["bots/nqueens/nqueens.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_NQUEENS"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "biasusr_root","posixcompat" ]},

  build application {
			target = "bb_sort",
			cFiles = ["bots/sort/sort.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_SORT"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "biasusr_root","posixcompat" ]},

  build application {
			target = "bb_sparselu",
			cFiles = ["bots/sparselu/sparselu.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_SPARSELU"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "biasusr_root","posixcompat","msun" ]},

  build application {
			target = "bb_strassen",
			cFiles = ["bots/strassen/strassen.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_STRASSEN"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "biasusr_root","posixcompat" ]},

  build application {
			target = "bb_uts",
			cFiles = ["bots/uts/uts.c","bots/uts/brg_sha1.c","bots/common/bots_main.c","bots/common/bots_common.c"],
			addCFlags = ["-D__BOTS_APP_UTS"],addLinkFlags = [ "-Xlinker --undefined=vfs_dummy"],
			omitCFlags = ["-nostdinc","-Wmissing-prototypes","-Werror"],addLibraries = [ "biasusr_root","vfs","nfs","lwip","contmng","posixcompat","msun" ]}

]
