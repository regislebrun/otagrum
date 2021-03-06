

set ( ADAPT_CHECK_TESTS_EXPONENT )
set ( REPLACE_EOL " | cat " )
set ( PATH_SEP ":" )
if ( MINGW )
  # replace dos eols
  set ( REPLACE_EOL " | tr -d '\\\\015' " )
  # By default, Mingw-g++ puts 3 digits to real exponent whereas g++ puts 2.
  set ( ADAPT_CHECK_TESTS_EXPONENT "PRINTF_EXPONENT_DIGITS=2" )
  set ( PATH_SEP "\\;" )
endif ()


set ( PYINSTALLCHECK_ENVIRONMENT "OPENTURNS_CONFIG_PATH=${INSTALL_DESTDIR}${SYSCONFIG_PATH}/openturns"
                                 "PYTHONPATH=${AGRUM_PYTHON_MODULE_PATH}${PATH_SEP}${OPENTURNS_PYTHON${PYTHON_VERSION_MAJOR}_MODULE_PATH}${PATH_SEP}${INSTALL_DESTDIR}${PYTHON_MODULE_PATH}${PATH_SEP}$ENV{PYTHONPATH}"
                                 ${ADAPT_CHECK_TESTS_EXPONENT}
    )


set ( PYINSTALLCHECK_TO_BE_RUN )
macro ( ot_pyinstallcheck_test TESTNAME )
  set ( TESTNAME_TGT t_${TESTNAME} )
  set ( TESTNAME_SRC ${TESTNAME_TGT}.py )
  set ( TESTNAME_LOC ${CMAKE_CURRENT_SOURCE_DIR}/${TESTNAME_SRC})
  set ( _CMD )
  set ( _PRE )
  set ( _POST )
  set ( _PARAMS )
  set ( _IGNOREOUT OFF )
  foreach ( _ARG ${ARGN} )
    string ( TOUPPER ${_ARG} ARG )
    if ( ${ARG} MATCHES PRE )
      set ( _CMD PRE )
    elseif ( ${ARG} MATCHES POST )
      set ( _CMD POST )
    elseif ( ${ARG} MATCHES PARAMS )
      set ( _CMD PARAMS )
    elseif ( ${ARG} MATCHES IGNOREOUT )
      set ( _IGNOREOUT ON )
    else ( ${ARG} MATCHES PRE )
      if ( ${_CMD} MATCHES PRE )
        set ( _PRE "${_PRE} ${_ARG} && " )
      elseif ( ${_CMD} MATCHES POST )
        set ( _POST "${_POST} && ${_ARG}" )
      elseif ( ${_CMD} MATCHES PARAMS )
        set ( _PARAMS "${_PARAMS} ${_ARG}" )
      endif  ( ${_CMD} MATCHES PRE )
      set ( _CMD )
    endif ( ${ARG} MATCHES PRE )
  endforeach ( _ARG )

  if (_IGNOREOUT)
    set ( COMMAND "( ${PYTHON_EXECUTABLE} ${TESTNAME_LOC} ${_PARAMS} > /dev/null )" )
  else (_IGNOREOUT)
    set ( TESTNAME_OUT ${TESTNAME_TGT}.expout )
    set ( OUTFILE_LOC ${CMAKE_CURRENT_SOURCE_DIR}/${TESTNAME_OUT} )
    set ( COMMAND "( ${PYTHON_EXECUTABLE} ${TESTNAME_LOC} ${_PARAMS} ${REPLACE_EOL} > ${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME_TGT}.out ) && diff -u ${OUTFILE_LOC} ${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME_TGT}.out" )
  endif (_IGNOREOUT)

  if (_PRE OR _POST)
    set ( COMMAND "${_PRE}${COMMAND}${_POST}" )
  endif (_PRE OR _POST)

  # pyAgrum might not be available when cross-compiling
  if (NOT CMAKE_CROSSCOMPILING)
    add_test (pyinstallcheck_${TESTNAME} "sh" "-c" "${COMMAND}")
    set_tests_properties ( pyinstallcheck_${TESTNAME} PROPERTIES ENVIRONMENT "${PYINSTALLCHECK_ENVIRONMENT}" )
  endif ()
  list ( APPEND PYINSTALLCHECK_TO_BE_RUN ${TESTNAME_SRC} )
  install ( FILES ${TESTNAME_SRC}
            DESTINATION ${OTAGRUM_EXAMPLE_PATH}
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
          )
endmacro ()

ot_pyinstallcheck_test (Utils_std)
ot_pyinstallcheck_test (docstring)
ot_pyinstallcheck_test (MixedHistogramUserDefined_std)
ot_pyinstallcheck_test (ContinuousPC_std)
ot_pyinstallcheck_test (NamedJunctionTree_std)
ot_pyinstallcheck_test (JunctionTreeBernsteinCopula_std)
if (NBSPHINX_FOUND) # depends on nbconvert, nbformat
  ot_pyinstallcheck_test (notebook IGNOREOUT)
endif ()
ot_pyinstallcheck_test (plant_growth)

add_custom_target ( pyinstallcheck COMMAND ${CMAKE_CTEST_COMMAND} -R "^pyinstallcheck_"
                    DEPENDS ${PYINSTALLCHECK_TO_BE_RUN}
                    COMMENT "Run Python post-installation tests" )


