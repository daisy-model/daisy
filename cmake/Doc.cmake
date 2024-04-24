if(${BUILD_DOC})
  add_custom_target(doc_setup
    COMMAND mkdir -p doc/gnufig
  )
  add_custom_command(OUTPUT doc/components.tex
    COMMAND ${DAISY_BIN_NAME} -d doc all.dai -p document
    COMMAND cd doc && gnuplot document.gnuplot
    DEPENDS
    doc_setup
  )

  add_custom_command(OUTPUT doc/stdlog.tex
    COMMAND ${DAISY_BIN_NAME} -d doc ${CMAKE_SOURCE_DIR}/txt/stdlog.dai
    DEPENDS doc_setup
  )

  add_custom_command(OUTPUT
    doc/reference.tex
    doc/tutorial.tex
    doc/exercises.tex
    doc/macro.tex
    doc/title.tex
    doc/introduction.tex
    doc/dwf.tex
    doc/daisy.bib
    doc/elsart-harv.bst
    COMMAND cp ${CMAKE_SOURCE_DIR}/txt/reference.tex doc
    COMMAND cp ${CMAKE_SOURCE_DIR}/txt/tutorial.tex doc
    COMMAND cp ${CMAKE_SOURCE_DIR}/txt/exercises.tex doc
    COMMAND cp ${CMAKE_SOURCE_DIR}/txt/macro.tex doc
    COMMAND cp ${CMAKE_SOURCE_DIR}/txt/title.tex doc
    COMMAND cp ${CMAKE_SOURCE_DIR}/txt/introduction.tex doc
    COMMAND cp ${CMAKE_SOURCE_DIR}/txt/dwf.tex doc
    COMMAND cp ${CMAKE_SOURCE_DIR}/txt/daisy.bib doc
    COMMAND cp ${CMAKE_SOURCE_DIR}/txt/elsart-harv.bst doc
    DEPENDS doc_setup
  )

  add_custom_command(OUTPUT doc/reference.pdf doc/reference.aux
    COMMAND pdflatex reference.tex >/dev/null
    COMMAND bibtex reference
    COMMAND pdflatex reference.tex >/dev/null
    COMMAND pdflatex reference.tex >/dev/null
    COMMAND pdflatex reference.tex
    DEPENDS
    doc/components.tex
    doc/reference.tex
    doc/macro.tex
    doc/title.tex
    doc/introduction.tex
    doc/dwf.tex
    doc/daisy.bib
    doc/elsart-harv.bst
    doc_setup
    WORKING_DIRECTORY doc
  )

  add_custom_command(OUTPUT doc/tutorial.pdf doc/tutorial.aux
    COMMAND pdflatex tutorial.tex >/dev/null
    COMMAND bibtex tutorial
    COMMAND pdflatex tutorial.tex >/dev/null
    COMMAND pdflatex tutorial.tex 
    DEPENDS
    doc/tutorial.tex
    doc/stdlog.tex
    doc/macro.tex
    doc/daisy.bib
    doc/elsart-harv.bst
    doc_setup
    WORKING_DIRECTORY doc
  )

  add_custom_command(OUTPUT doc/exercises.pdf
    COMMAND pdflatex exercises.tex >/dev/null
    COMMAND bibtex exercises
    COMMAND pdflatex exercises.tex >/dev/null
    COMMAND pdflatex exercises.tex >/dev/null
    COMMAND pdflatex exercises.tex 
    WORKING_DIRECTORY doc
    DEPENDS
    doc/exercises.tex
    doc/stdlog.tex
    doc/macro.tex
    doc/daisy.bib
    doc/elsart-harv.bst
    doc_setup
    doc/tutorial.aux
    doc/reference.aux
  )

  
  add_custom_target(tutorial SOURCES
    doc/tutorial.pdf
  )
  
  add_custom_target(reference SOURCES
    doc/reference.pdf
  )

  add_custom_target(exercises SOURCES
    doc/exercises.pdf
  )

  # The docs target first builds reference.pdf, tutorial.pdf and exercises.pdf; then it deletes all
  # the files used/generated during build.
  add_custom_target(docs
    COMMAND rm -r doc/gnufig
    COMMAND find doc/ -type f -regex .+\.[^pdf] | xargs rm
    DEPENDS reference tutorial exercises
  )
endif()
