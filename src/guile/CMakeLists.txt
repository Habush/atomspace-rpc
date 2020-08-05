include(OpenCogGuile)
add_library(cog_grpc SHARED AtomServiceSCM.cpp)

target_link_libraries(cog_grpc
        atom_service_client
        ${ATOMSPACE_LIBRARIES})

DECLARE_GUILE_CONFIG_TARGET(SCM_CONFIG "opencog grpc-config" "ATOMSPACE_TEST")

ADD_GUILE_EXTENSION(SCM_CONFIG cog_grpc "opencog-ext-path-cog_grpc")

install(TARGETS cog_grpc
        DESTINATION "lib${LIB_DIR_SUFFIX}/opencog")

ADD_GUILE_MODULE(FILES
        grpc.scm
        MODULE_DESTINATION "${GUILE_SITE_DIR}/opencog"
        )

WRITE_GUILE_CONFIG(${GUILE_BIN_DIR}/opencog/grpc-config.scm SCM_CONFIG TRUE)
WRITE_GUILE_CONFIG(${GUILE_BIN_DIR}/opencog/grpc-config-installable.scm SCM_CONFIG FALSE)
install(FILES ${GUILE_BIN_DIR}/opencog/grpc-config-installable.scm DESTINATION ${GUILE_SITE_DIR}/opencog RENAME
        grpc-config.scm)