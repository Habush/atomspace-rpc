include(OpenCogGuile)
add_library(atom-service-scm SHARED AtomServiceSCM.cpp)

target_link_libraries(atom-service-scm
        atom_service_client
        ${COGUTIL_LIBRARIES}
        ${ATOMSPACE_LIBRARIES})

DECLARE_GUILE_CONFIG_TARGET(SCM_CONFIG "opencog atom-service-config" "ATOMSPACE_TEST")

ADD_GUILE_EXTENSION(SCM_CONFIG atom-service-scm "opencog-ext-path-atom-service-scm")

install(TARGETS atom-service-scm
        DESTINATION "lib${LIB_DIR_SUFFIX}/opencog")

ADD_GUILE_MODULE(FILES
        atom-service.scm
        MODULE_DESTINATION "${GUILE_SITE_DIR}/opencog"
        )

WRITE_GUILE_CONFIG(${GUILE_BIN_DIR}/opencog/atom-service-config.scm SCM_CONFIG TRUE)
WRITE_GUILE_CONFIG(${GUILE_BIN_DIR}/opencog/atom-service-config-installable.scm SCM_CONFIG FALSE)
install(FILES ${GUILE_BIN_DIR}/opencog/atom-service-config-installable.scm DESTINATION ${GUILE_SITE_DIR}/opencog RENAME
        atom-service-config.scm)