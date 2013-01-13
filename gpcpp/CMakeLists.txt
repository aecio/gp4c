set(SRC_LIST  src/compare.cc
	      src/config.cc
	      src/contain.cc
	      src/create.cc
	      src/cross.cc
	      src/eval.cc
	      src/gene.cc
	      src/generate.cc
	      src/gp.cc
	      src/gprand.cc
	      src/gpv.cc
	      src/loadsave.cc
	      src/misc.cc
	      src/mutate.cc
	      src/node.cc
	      src/pop.cc
	      src/select.cc
)

set(HEADER_LIST include/gp.h
		include/gpconfig.h
)

add_library(gp ${SRC_LIST} ${HEADER_LIST})
