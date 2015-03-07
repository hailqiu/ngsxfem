add_library(ngsxfem_py ${NGS_LIB_TYPE}
          python_xfem.cpp
        )

set_target_properties(ngsxfem_py PROPERTIES SUFFIX ".so")

if(NOT WIN32)
    target_link_libraries(ngsxfem_py ngsxfem_xfem ngsxfem_spacetime ngsxfem_utils ngsxfem_cutint ${PYTHON_LIBS} ${LAPACK_LIBRARIES} ${NETGEN_LIBS})
    install( TARGETS ngsxfem_py ${ngs_install_dir})
endif(NOT WIN32)