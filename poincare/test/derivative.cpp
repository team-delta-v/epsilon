#include <ion/storage/file_system.h>
#include <poincare/derivative.h>
#include <poincare/nonreal.h>
#include <poincare/undefined.h>

#include "helper.h"

using namespace Poincare;

void assert_reduces_to_formal_expression(
    const char* expression, const char* result,
    Preferences::AngleUnit angleUnit = Radian,
    Preferences::ComplexFormat complexFormat = Cartesian) {
  assert_parsed_expression_simplify_to(expression, result, User, angleUnit,
                                       MetricUnitFormat, complexFormat,
                                       ReplaceAllDefinedSymbolsWithDefinition);
}

QUIZ_CASE(poincare_derivative_formal) {
  assert_reduces_to_formal_expression("diff(undef,x,x)", Undefined::Name());
  assert_reduces_to_formal_expression("diff(nonreal,x,x)", Nonreal::Name());
  assert_reduces_to_formal_expression("diff(inf,x,x)", Undefined::Name());

  assert_reduces_to_formal_expression("diff(1,x,x)", "0");
  assert_reduces_to_formal_expression("diff(π,x,x)", "0");
  assert_reduces_to_formal_expression("diff(y,x,x)", "\u0014dep(0,{y})");
  assert_reduces_to_formal_expression("diff(x,x,x)", "\u0014dep(1,{x})");
  assert_reduces_to_formal_expression("diff(x^2,x,x)", "2×x");
  assert_reduces_to_formal_expression("diff((x-1)(x-2)(x-3),x,x)",
                                      "3×x^2-12×x+11");
  assert_reduces_to_formal_expression("diff(√(x),x,x)", "1/\u00122×√(x)\u0013");
  assert_reduces_to_formal_expression("diff(1/x,x,x)", "-1/x^2");

  assert_reduces_to_formal_expression("diff(e^x,x,x)", "e^\u0012x\u0013");
  assert_reduces_to_formal_expression("diff(2^x,x,x)", "2^x×ln(2)");
  assert_reduces_to_formal_expression("diff(ln(x),x,x)",
                                      "\u0014dep(1/x,{ln(x)})");
  assert_reduces_to_formal_expression(
      "diff(log(x),x,x)", "\u0014dep(1/\u0012x×ln(10)\u0013,{log(x)})");

  assert_reduces_to_formal_expression("diff(sin(x),x,x)",
                                      "\u0014dep(cos(x),{sin(x)})");
  assert_reduces_to_formal_expression(
      "diff(sin(x),x,x)", "\u0014dep(\u0012π×cos(x)\u0013/180,{sin(x)})",
      Degree);
  assert_reduces_to_formal_expression("diff(cos(x),x,x)",
                                      "\u0014dep(-sin(x),{cos(x)})");
  assert_reduces_to_formal_expression(
      "diff(cos(x),x,x)", "\u0014dep(-\u0012π×sin(x)\u0013/200,{cos(x)})",
      Gradian);
  assert_reduces_to_formal_expression("diff(tan(x),x,x)",
                                      "\u0014dep(tan(x)^2+1,{sec(x),sin(x)})");
  assert_reduces_to_formal_expression(
      "diff(tan(x),x,x)",
      "\u0014dep(\U00000012π×tan(x)^2+π\U00000013/180,{sec(x),sin(x)})",
      Degree);

  assert_reduces_to_formal_expression("diff(asin(x),x,x)",
                                      "\u0014dep(1/√(-x^2+1),{arcsin(x)})");
  assert_reduces_to_formal_expression(
      "diff(asin(x),x,x)", "\u0014dep(180/\u0012π×√(-x^2+1)\u0013,{arcsin(x)})",
      Degree);
  assert_reduces_to_formal_expression("diff(acos(x),x,x)",
                                      "\u0014dep(-1/√(-x^2+1),{arccos(x)})");
  assert_reduces_to_formal_expression(
      "diff(acos(x),x,x)",
      "\u0014dep(-180/\u0012π×√(-x^2+1)\u0013,{arccos(x)})", Degree);
  assert_reduces_to_formal_expression(
      "diff(atan(x),x,x)", "\u0014dep(1/\u0012x^2+1\u0013,{arctan(x)})");
  assert_reduces_to_formal_expression(
      "diff(atan(x),x,x)", "\u0014dep(180/\u0012π×x^2+π\u0013,{arctan(x)})",
      Degree);
  assert_reduces_to_formal_expression(
      "diff(arcsec(x),x,x)",
      "\u0014dep(1/\u0012x^2×√(\u0012x^2-1\u0013/x^2)\u0013,{arccos(1/x)})");
  assert_reduces_to_formal_expression(
      "diff(arccsc(x),x,x)",
      "\u0014dep(-1/\u0012x^2×√(\u0012x^2-1\u0013/x^2)\u0013,{arcsin(1/x)})");
  assert_reduces_to_formal_expression(
      "diff(arccot(x),x,x)", "\u0014dep(-1/\u0012x^2+1\u0013,{arccot(x)})");

  assert_reduces_to_formal_expression("diff(sinh(x),x,x)",
                                      "\u0014dep(cosh(x),{sinh(x)})");
  assert_reduces_to_formal_expression("diff(cosh(x),x,x)",
                                      "\u0014dep(sinh(x),{cosh(x)})");
  assert_reduces_to_formal_expression("diff(tanh(x),x,x)",
                                      "\u0014dep(1/cosh(x)^2,{tanh(x)})");
  assert_reduces_to_formal_expression("diff(sin(x)^2,x,x)", "2×sin(x)×cos(x)");

  assert_reduces_to_formal_expression("diff(diff(x^3,x,x),x,x)", "6×x");
  assert_reduces_to_formal_expression("diff(sinh(sin(y)),x,x)",
                                      "\u0014dep(0,{sinh(sin(y))})");

  assert_reduces_to_formal_expression(
      "diff(piecewise(x,x≥1,-x,x≠0),x,x)",
      "\u0014dep(piecewise(1,x>1,undef,x≥1,-1,"
      "x≠0,undef,x=0),{piecewise(x,x≥1,-x,x≠0)})");
  assert_reduces_to_formal_expression(
      "diff(piecewise(x,x=1,2×x,x<5,3),x,x)",
      "\u0014dep(piecewise(undef,x=1,2,x<5,undef,x≤5,0),{"
      "piecewise(x,x=1,2×x,x<5,3)})");

  assert_reduce_and_store("2→a");
  assert_reduce_and_store("-1→b");
  assert_reduce_and_store("3→c");
  assert_reduce_and_store("x/2→f(x)");

  assert_reduces_to_formal_expression("diff(a×x^2+b×x+c,x,x)", "4×x-1");
  assert_reduces_to_formal_expression("diff(f(x),x,x)", "\u0014dep(1/2,{x})");
  assert_reduces_to_formal_expression("diff(a^2,a,x)", "2×x");
  assert_reduces_to_formal_expression("diff(a^2,a,a)", "4");
  assert_reduces_to_formal_expression("diff(b^2,b,2)", "4", Radian, Real);

  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("b.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("c.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();

  // Polar
  assert_reduces_to_formal_expression("diff(3θ^2,θ,θ)", "6×θ");
  assert_reduces_to_formal_expression("diff(cos(θ),θ,θ)",
                                      "\U00000014dep(-sin(θ),{cos(θ)})");
  assert_reduce_and_store("cos(θ)^2→r1(θ)");
  assert_reduces_to_formal_expression("diff(r1(θ),θ,θ)", "-2×sin(θ)×cos(θ)");
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("r1.func").destroy();

  // Parametric 1D
  assert_reduces_to_formal_expression("diff(3t^2,t,t)", "6×t");
  assert_reduces_to_formal_expression("diff(cos(t),t,t)",
                                      "\U00000014dep(-sin(t),{cos(t)})");
  assert_reduce_and_store("cos(t)^2→f(t)");
  assert_reduces_to_formal_expression("diff(f(t),t,t)", "-2×sin(t)×cos(t)");
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("t.func").destroy();

  // Parametric 2D
  assert_reduces_to_formal_expression("diff((sin(t),cos(t)),t,t)",
                                      "(cos(t),-sin(t))");
  assert_reduce_and_store("(3t,-2t^2)→f(t)");
  assert_reduces_to_formal_expression("diff(f(t),t,t)", "(3,-4×t)");
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();

  // Undefined on matrix
  assert_reduces_to_formal_expression("diff([[x]],x,x)", Undefined::Name());
  assert_reduces_to_formal_expression("diff([[2t,3t]],t,t)", Undefined::Name());
  assert_reduces_to_formal_expression("diff([[2t][3t]],t,t)",
                                      Undefined::Name());
}

QUIZ_CASE(poincare_derivative_formal_higher_order) {
  assert_reduces_to_formal_expression("diff(x^3,x,x,2)", "6×x");
  assert_reduces_to_formal_expression("diff(cos(x),x,x,3)",
                                      "\u0014dep(sin(x),{cos(x)})");
  assert_reduces_to_formal_expression("diff(x^5+1,x,x,10)", "\u0014dep(0,{x})");
  assert_reduces_to_formal_expression("diff(e^(2x),x,x,8)",
                                      "256×e^\u00122×x\u0013");
  assert_reduces_to_formal_expression("diff(x^3,x,x,0)", "x^3");
  assert_reduces_to_formal_expression("diff(x^3,x,x,-1)", Undefined::Name());
  assert_reduces_to_formal_expression("diff(x^3,x,x,1.3)", Undefined::Name());
  assert_reduces_to_formal_expression("diff(x^3,x,x,n)", "diff(x^3,x,x,n)");

  // Polar
  assert_reduces_to_formal_expression("diff(θ^3,θ,θ,2)", "6×θ");
  assert_reduces_to_formal_expression("diff(sin(θ),θ,θ,4)", "sin(θ)");

  // Parametric 1D
  assert_reduces_to_formal_expression("diff(t^4,t,t,2)", "12×t^2");
  assert_reduces_to_formal_expression("diff(cos(t),t,t,4)", "cos(t)");

  // Parametric 2D
  assert_reduces_to_formal_expression("diff((sin(t),cos(t)),t,t,2)",
                                      "(-sin(t),-cos(t))");
  assert_reduces_to_formal_expression("diff((t,2t^2),t,t,3)",
                                      "\U00000014dep((0,0),{t})");
}

void assert_reduces_for_approximation(
    const char* expression, const char* result,
    Preferences::AngleUnit angleUnit = Radian,
    Poincare::Preferences::ComplexFormat complexFormat = Real) {
  assert_parsed_expression_simplify_to(
      expression, result, SystemForApproximation, angleUnit, MetricUnitFormat,
      complexFormat, ReplaceAllSymbolsWithDefinitionsOrUndefined);
}

QUIZ_CASE(poincare_derivative_reduced_approximation) {
  assert(Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp")
             .isNull());
  assert_reduces_for_approximation("diff(ln(x),x,1)", "1");
  assert_reduces_for_approximation("diff(ln(x),x,2.2)", "5/11");
  assert_reduces_for_approximation("diff(ln(x),x,0)", Undefined::Name());
  assert_reduces_for_approximation("diff(ln(x),x,-3.1)", Nonreal::Name());
  assert_reduces_for_approximation("diff(log(x),x,-10)", Nonreal::Name());

  assert_reduces_for_approximation("diff(abs(x),x,123)", "1");
  assert_reduces_for_approximation("diff(abs(x),x,-2.34)", "-1");
  assert_reduces_for_approximation("diff(abs(x),x,0)", Undefined::Name());

  assert_reduces_for_approximation("diff(sign(x),x,123)", "0");
  assert_reduces_for_approximation("diff(sign(x),x,-2.34)", "0");
  assert_reduces_for_approximation("diff(sign(x),x,0)", Undefined::Name());

  assert_reduces_for_approximation("diff(√(x),x,-1)", "-i/2", Radian,
                                   Cartesian);

  assert_reduces_for_approximation("diff(asin(x),x,1)", Undefined::Name());
  assert_reduces_for_approximation("diff(asin(x),x,-1)", Undefined::Name());
  assert_reduces_for_approximation("diff(acos(x),x,1)", Undefined::Name());
  assert_reduces_for_approximation("diff(acos(x),x,-1)", Undefined::Name());
  assert_reduces_for_approximation("diff(arccot(x),x,0)", "-1");

  assert_reduces_for_approximation("diff(1/x,x,-2)", "-1/4");
  assert_reduces_for_approximation("diff(x^3+5*x^2,x,0)", "0");
  assert_reduces_for_approximation("diff(5^(sin(x)),x,3)",
                                   "5^sin(3)×cos(3)×ln(5)");
  assert_reduces_for_approximation("diff((-1)^(4-2*2),x,3)", "0");
  assert_reduce_and_store("0→a");
  assert_reduces_for_approximation("diff((-1)^(a*x),x,3)", "0");
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();

  // Polar
  assert_reduces_for_approximation("diff(θ^3/5,θ,3)", "27/5");
  assert_reduces_for_approximation("diff(sin(θ),θ,π/6)", "√(3)/2");

  // Parametric 1D
  assert_reduces_for_approximation("diff(t^3/7,t,3)", "27/7");
  assert_reduces_for_approximation("diff(cos(t),t,π/3)", "-√(3)/2");

  // Parametric 2D
  assert_reduces_to_formal_expression("diff((sin(t),cos(t)),t,π/6)",
                                      "(√(3)/2,-1/2)");
  assert_reduces_to_formal_expression("diff((1,2),t,1)", "(0,0)");
}

void assert_approximate_to(const char* expression, const char* result,
                           Preferences::AngleUnit angleUnit = Radian) {
  /* Reduce significant numbers to 3 to handle platforms discrepancies when
   * computing floats. This allows to expect the same results from both double
   * and float approximations. */
  assert_expression_approximates_to<float>(expression, result, angleUnit,
                                           MetricUnitFormat, Real, 3);
  assert_expression_approximates_to<double>(expression, result, angleUnit,
                                            MetricUnitFormat, Real, 3);
}

QUIZ_CASE(poincare_derivative_approximation) {
  assert_approximate_to("diff(2×x, x, 2)", "2");
  assert_approximate_to("diff(2×\"TO\"^2, \"TO\", 7)", "28");
  assert_approximate_to("diff(ln(x),x,1)", "1");
  assert_approximate_to("diff(ln(x),x,2.2)", "0.455");
  assert_approximate_to("diff(ln(x),x,0)", Undefined::Name());
  assert_approximate_to("diff(ln(x),x,-3.1)", Nonreal::Name());
  assert_approximate_to("diff(log(x),x,-10)", Nonreal::Name());
  assert_approximate_to("diff(abs(x),x,123)", "1");
  assert_approximate_to("diff(abs(x),x,-2.34)", "-1");
  assert_approximate_to("diff(1/x,x,-2)", "-0.25");
  assert_approximate_to("diff(x^3+5*x^2,x,0)", "0");
  assert_approximate_to("diff(abs(x),x,0)", "0");  // Undefined::Name());
  // FIXME error too big on floats
  // assert_expression_approximates_to<float>("diff(-1/3×x^3+6x^2-11x-50,x,11)",
  // "0");
  assert_expression_approximates_to<double>("diff(-1/3×x^3+6x^2-11x-50,x,11)",
                                            "0");

  // Polar
  assert_approximate_to("diff(sin(θ),θ,π/3)", "0.5");

  // Parametric 1D
  assert_approximate_to("diff(cos(t),t,π/6)", "-0.5");

  // Parametric 2D
  assert_approximate_to("diff((sin(t),cos(t)),t,π/2)", "(0,-1)");
}

QUIZ_CASE(poincare_derivative_approximation_higher_order) {
  // We have to approximate to double because error is too big on floats
  assert_expression_approximates_to<double>("diff(x^3,x,10,2)", "60");
  assert_expression_approximates_to<double>("diff(x^3,x,1,4)", "0");
  assert_expression_approximates_to<double>("diff(e^(2x),x,0,4)", "16");
  assert_approximate_to("diff(x^3,x,3,0)", "27");
  assert_approximate_to("diff(x^3,x,3,-1)", Undefined::Name());
  assert_approximate_to("diff(x^3,x,3,1.3)", Undefined::Name());
  // Order 5 and above are not handled because recursively too long
  assert_approximate_to("diff(e^(2x),x,0,5)", Undefined::Name());

  // Polar
  assert_expression_approximates_to<double>("diff(θ^3,θ,10,2)", "60");

  // Parametric 1D
  assert_expression_approximates_to<double>("diff(t^3,t,1,4)", "0");

  // Parametric 2D
  assert_expression_approximates_to<double>("diff((2t,ln(t)),t,2,2)",
                                            "(0,-0.25)");
}
