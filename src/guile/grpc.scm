(define-module (opencog grpc))

(use-modules (opencog))
(use-modules (opencog grpc-config))


(load-extension (string-append opencog-ext-path-cog_grpc "libcog_grpc") "opencog_atom_service_init")

(export exec-pattern)

(set-procedure-property! exec-pattern 'documentation
  "
    exec-pattern ATOM_ID PATTERN - Execute PATTERN on a remote atomspace with id ATOM_ID
  "
)

(export check-node)

(set-procedure-property! check-node 'documentation
  "
    check-node ATOM_ID TYPE NAME - Check if an node with the specified TYPE and NAME exists in a remote atomspace
    ATOM_ID
  "
)

(export find-similar-node)

(set-procedure-property! find-similar-node 'documentation
  "
    find-similar-node ATOM_ID TYPE NAME - Find nodes that have similar names and same type in remote atomspace ATOM_ID
  "
)
