#include <poincare/complex_argument.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper ComplexArgument::s_functionHelper;

int ComplexArgumentNode::numberOfChildren() const { return ComplexArgument::s_functionHelper.numberOfChildren(); }

Layout ComplexArgumentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ComplexArgument(this), floatDisplayMode, numberOfSignificantDigits, ComplexArgument::s_functionHelper.name());
}

int ComplexArgumentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ComplexArgument::s_functionHelper.name());
}

Expression ComplexArgumentNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return ComplexArgument(this).shallowReduce(context, angleUnit, target);
}

template<typename T>
Complex<T> ComplexArgumentNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(std::arg(c));
}

Expression ComplexArgument::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  Expression arg = c.complexArgument(context, angleUnit);
  if (!arg.isUninitialized()) {
    replaceWithInPlace(arg);
    return arg.deepReduce(context, angleUnit, target);
  }
  return *this;
}

}
