(define-module (opencog atom-service))

(use-modules (opencog))
(use-modules (opencog atom-service-config))

(load-extension
  (string-append opencog-ext-path-atom-service-scm "libatom-service-scm")
  "opencog_atom_service_init"
)

(export exec-pattern)

(set-procedure-property! exec-pattern 'documentation
  "
    exec-pattern ATOM_ID PATTERN - Execute PATTERN on a remote atomspace with id ATOM_ID
  "
)