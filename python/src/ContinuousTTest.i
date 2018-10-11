// SWIG file ContinuousTTest.i

%{
#include <tuple>

#include "otagr/IndicesManip.hxx"
#include "otagr/ContinuousTTest.hxx"

using namespace OTAGR;
%}

%typemap(in) Indice = int;


%typemap(out) std::tuple<double, double, bool> {
  $result = Py_BuildValue("ddO",std::get<0>($1),
                                std::get<1>($1),
                                std::get<2>($1)?Py_True:Py_False);
}
//%include ContinuousTTest_doc.i
%include "otagr/ContinuousTTest.hxx"