#include <poincare/decimal.h>
#include <poincare/rational.h>
#include <poincare/opposite.h>
#include <poincare/infinity.h>
#include <poincare/layout_helper.h>
#include <poincare/ieee754.h>
#include <assert.h>
#include <ion.h>
#include <cmath>
#include <assert.h>

namespace Poincare {

void removeZeroAtTheEnd(Integer * i) {
  if (i->isZero()) {
    return;
  }
  Integer base = Integer(10);
  IntegerDivision d = Integer::Division(*i, base);
  while (d.remainder.isZero()) {
    *i = d.quotient;
    d = Integer::Division(*i, base);
  }
  assert(!i->isInfinity());
}

void DecimalNode::setValue(native_uint_t * mantissaDigits, size_t mantissaSize, int exponent, bool negative) {
  m_negative = negative;
  m_exponent = exponent;
  m_numberOfDigitsInMantissa = mantissaSize;
  memcpy(m_mantissa, mantissaDigits, mantissaSize*sizeof(native_uint_t));
}

DecimalNode * DecimalNode::FailedAllocationStaticNode() {
  static AllocationFailureDecimalNode failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

NaturalIntegerPointer DecimalNode::mantissa() const {
  return NaturalIntegerPointer((native_uint_t *)m_mantissa, m_numberOfDigitsInMantissa);
}

size_t DecimalNode::size() const {
  return sizeof(DecimalNode)+ sizeof(native_uint_t)*m_numberOfDigitsInMantissa;
}

Expression DecimalNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  return Decimal(this).setSign(s, context, angleUnit);
}

int DecimalNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->type() == Type::Decimal);
  const DecimalNode * other = static_cast<const DecimalNode *>(e);
  if (m_negative && !other->m_negative) {
    return -1;
  }
  if (!m_negative && other->m_negative) {
    return 1;
  }
  assert(m_negative == other->m_negative);
  int unsignedComparison = 0;
  if (m_exponent < other->m_exponent) {
    unsignedComparison = -1;
  } else if (m_exponent > other->m_exponent) {
    unsignedComparison = 1;
  } else {
    assert(m_exponent == other->m_exponent);
    NaturalIntegerPointer m = mantissa();
    NaturalIntegerPointer otherM = other->mantissa();
    unsignedComparison = NaturalIntegerAbstract::ucmp(&m, &otherM);
  }
  return ((int)sign())*unsignedComparison;
}

Expression DecimalNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  return Decimal(this).shallowReduce(context, angleUnit, futureParent);
}

Expression DecimalNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  return Decimal(this).shallowBeautify(context, angleUnit);
}

bool DecimalNode::needsParenthesesWithParent(const SerializationHelperInterface * e) const {
  if (!m_negative) {
    return false;
  }
  Type types[] = {Type::Addition, Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Power, Type::Factorial};
  return static_cast<const ExpressionNode *>(e)->isOfType(types, 7);
}

LayoutRef DecimalNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  char buffer[k_maxBufferSize];
  int numberOfChars = convertToText(buffer, k_maxBufferSize, floatDisplayMode, numberOfSignificantDigits);
  return LayoutHelper::String(buffer, numberOfChars);
}

int DecimalNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return convertToText(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
}

int DecimalNode::convertToText(char * buffer, int bufferSize, Preferences::PrintFloatMode mode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int currentChar = 0;
  if (currentChar >= bufferSize-1) { return bufferSize-1; }
  if (mantissa().isZero()) {
    buffer[currentChar++] = '0';
    buffer[currentChar] = 0;
    return currentChar;
  }
  int exponent = m_exponent;
  char tempBuffer[PrintFloat::k_numberOfStoredSignificantDigits+1];
  // Round the integer if m_mantissa > 10^numberOfSignificantDigits-1
  NaturalIntegerPointer m = mantissa();
  Integer mantissaRef(&m);
  int numberOfDigitsInMantissa = Integer::NumberOfBase10Digits(mantissaRef);
  if (numberOfDigitsInMantissa > numberOfSignificantDigits) {
    IntegerDivision d = Integer::Division(mantissaRef, Integer((int64_t)std::pow(10.0, numberOfDigitsInMantissa - numberOfSignificantDigits)));
    mantissaRef = d.quotient;
    if (Integer::NaturalOrder(d.remainder, Integer((int64_t)(5.0*std::pow(10.0, numberOfDigitsInMantissa-numberOfSignificantDigits-1)))) >= 0) {
      mantissaRef = Integer::Addition(mantissaRef, Integer(1));
      // if 9999 was rounded to 10000, we need to update exponent and mantissa
      if (Integer::NumberOfBase10Digits(mantissaRef) > numberOfSignificantDigits) {
        exponent++;
        mantissaRef = Integer::Division(mantissaRef, Integer(10)).quotient;
      }
    }
    removeZeroAtTheEnd(&mantissaRef);
  }
  if (m_negative) {
    buffer[currentChar++] = '-';
    if (currentChar >= bufferSize-1) { return bufferSize-1; }
  }
  int mantissaLength = mantissaRef.serialize(tempBuffer, PrintFloat::k_numberOfStoredSignificantDigits+1, mode, numberOfSignificantDigits);
  if (strcmp(tempBuffer, "inf") == 0) {
    currentChar += strlcpy(buffer+currentChar, tempBuffer, bufferSize-currentChar);
    return currentChar;
  }
  /* We force scientific mode if the number of digits before the dot is superior
   * to the number of significant digits (ie with 4 significant digits,
   * 12345 -> 1.235E4 or 12340 -> 1.234E4). */
  bool forceScientificMode = mode == Preferences::PrintFloatMode::Scientific || exponent >= numberOfSignificantDigits;
  int numberOfRequiredDigits = mantissaLength;
  if (!forceScientificMode) {
    numberOfRequiredDigits = mantissaLength > exponent ? mantissaLength : exponent;
    numberOfRequiredDigits = exponent < 0 ? mantissaLength-exponent : numberOfRequiredDigits;
  }
  /* Case 0: Scientific mode. Three cases:
   * - the user chooses the scientific mode
   * - the exponent is too big compared to the number of significant digits, so
   *   we force the scientific mode to avoid inventing digits
   * - the number would be too long if we print it as a natural decimal */
  if (numberOfRequiredDigits > PrintFloat::k_numberOfStoredSignificantDigits || forceScientificMode) {
    if (mantissaLength == 1) {
      currentChar += strlcpy(buffer+currentChar, tempBuffer, bufferSize-currentChar);
    } else {
      currentChar++;
      int decimalMarkerPosition = currentChar;
      if (currentChar >= bufferSize-1) { return bufferSize-1; }
      currentChar += strlcpy(buffer+currentChar, tempBuffer, bufferSize-currentChar);
      buffer[decimalMarkerPosition-1] = buffer[decimalMarkerPosition];
      buffer[decimalMarkerPosition] = '.';
    }
    if (exponent == 0) {
      return currentChar;
    }
    if (currentChar >= bufferSize-1) { return bufferSize-1; }
    buffer[currentChar++] = Ion::Charset::Exponent;
    currentChar += Integer(exponent).serialize(buffer+currentChar, bufferSize-currentChar, mode, numberOfSignificantDigits);
    return currentChar;
  }
  /* Case 1: Decimal mode */
  int deltaCharMantissa = exponent < 0 ? -exponent+1 : 0;
  strlcpy(buffer+currentChar+deltaCharMantissa, tempBuffer, bufferSize-deltaCharMantissa-currentChar);
  if (exponent < 0) {
    for (int i = 0; i <= -exponent; i++) {
      if (currentChar >= bufferSize-1) { return bufferSize-1; }
      if (i == 1) {
        buffer[currentChar++] = '.';
        continue;
      }
      buffer[currentChar++] = '0';
    }
  }
  currentChar += mantissaLength;
  if (exponent >= 0 && exponent < mantissaLength-1) {
    if (currentChar+1 >= bufferSize-1) { return bufferSize-1; }
    int decimalMarkerPosition = m_negative ? exponent + 1 : exponent;
    for (int i = currentChar-1; i > decimalMarkerPosition; i--) {
      buffer[i+1] = buffer[i];
    }
    if (currentChar >= bufferSize-1) { return bufferSize-1; }
    buffer[decimalMarkerPosition+1] = '.';
    currentChar++;
  }
  if (exponent >= 0 && exponent > mantissaLength-1) {
    int endMarkerPosition = m_negative ? exponent+1 : exponent;
    for (int i = currentChar-1; i < endMarkerPosition; i++) {
      if (currentChar+1 >= bufferSize-1) { return bufferSize-1; }
      buffer[currentChar++] = '0';
    }
  }
  if (currentChar >= bufferSize-1) { return bufferSize-1; }
  buffer[currentChar] = 0;
  return currentChar;
}

template<typename T> Evaluation<T> DecimalNode::templatedApproximate() const {
  NaturalIntegerPointer m = mantissa();
  T f = m.approximate<T>();
  int numberOfDigits = NaturalIntegerAbstract::NumberOfBase10Digits(&m);
  T result = f*std::pow((T)10.0, (T)(m_exponent-numberOfDigits+1));
  return Complex<T>(m_negative ? -result : result);
}

int Decimal::Exponent(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, const char * exponent, int exponentLength) {
  bool exponentNegative = false;
  if (exponentLength > 0 && exponent[0] == '-') {
    exponent++;
    exponentNegative = true;
    exponentLength--;
  }
  int base = 10;
  int exp = 0;
  for (int i = 0; i < exponentLength; i++) {
    exp *= base;
    exp += *exponent-'0';
    exponent++;
  }
  if (exponentNegative) {
    exp = -exp;
  }
  const char * integralPartEnd = integralPart + integralPartLength;
  if (integralPart != nullptr) {
    while (*integralPart == '0' && integralPart < integralPartEnd) {
      integralPart++;
    }
  }
  exp += integralPartEnd-integralPart-1;
  if (integralPart == integralPartEnd) {
    const char * fractionalPartEnd = fractionalPart + fractionalPartLength;
    if (fractionalPart != nullptr) {
      while (*fractionalPart == '0' && fractionalPart < fractionalPartEnd) {
        fractionalPart++;
        exp--;
      }
    }
    if (fractionalPart == fractionalPartEnd) {
      exp += fractionalPartLength+1;
    }
  }
  return exp;
}

Decimal::Decimal(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, int exponent) : Number() {
  /* Create a Decimal whose mantissa has less than
   * k_numberOfStoredSignificantDigits. We round exceeding number if necessary. */
  Integer zero(0);
  Integer base(10);
  // Get rid of useless preceeding 0s
  while (*integralPart == '0' & integralPartLength > 1) {
    integralPart++;
    integralPartLength--;
  }
  //TODO: set a FLAG to tell that a rounding happened?
  bool rounding = integralPartLength > PrintFloat::k_numberOfStoredSignificantDigits && integralPart[PrintFloat::k_numberOfStoredSignificantDigits] >= '5';
  // Cap the length of the integralPart
  integralPartLength = integralPartLength > PrintFloat::k_numberOfStoredSignificantDigits ? PrintFloat::k_numberOfStoredSignificantDigits : integralPartLength;
  Integer numerator(integralPart, integralPartLength, false);
  assert(!numerator.isInfinity());
  // Special case for 0.??? : get rid of useless 0s in front of the integralPartLength
  if (integralPartLength == 1 && integralPart[0] == '0') {
    integralPartLength = 0;
    while (*fractionalPart == '0') {
      fractionalPart++;
      fractionalPartLength--;
    }
  }
  rounding |= integralPartLength+fractionalPartLength > PrintFloat::k_numberOfStoredSignificantDigits && fractionalPart[PrintFloat::k_numberOfStoredSignificantDigits-integralPartLength] >= '5';
  fractionalPartLength = integralPartLength+fractionalPartLength > PrintFloat::k_numberOfStoredSignificantDigits ? PrintFloat::k_numberOfStoredSignificantDigits - integralPartLength : fractionalPartLength;
  for (int i = 0; i < fractionalPartLength; i++) {
    numerator = Integer::Multiplication(numerator, base);
    numerator = Integer::Addition(numerator, Integer(*fractionalPart-'0'));
    fractionalPart++;
  }
  numerator = rounding ? Integer::Addition(numerator, Integer(1)) : numerator;
  new (this) Decimal(numerator, exponent);
}

template <typename T>
Decimal::Decimal(T f) : Number() {
  assert(!std::isnan(f) && !std::isinf(f));
  int exp = IEEE754<T>::exponentBase10(f);
  int64_t mantissaf = std::round((double)f * std::pow((double)10.0, (double)(-exp+PrintFloat::k_numberOfStoredSignificantDigits+1)));
  Integer m(mantissaf);
  new (this) Decimal(Integer(mantissaf), exp);
}

Decimal::Decimal(Integer m, int e) :
  Decimal(sizeof(DecimalNode)+m.numberOfDigits()*sizeof(native_uint_t), m, e) {}


Decimal::Decimal(size_t size, const Integer & m, int e) : Number(TreePool::sharedPool()->createTreeNode<DecimalNode>(size)) {
  node()->setValue(m.node()->digits(), m.node()->numberOfDigits(), e, m.isNegative());
}

Expression Decimal::setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  Decimal result = *this;
  result.node()->setNegative(s == ExpressionNode::Sign::Negative);
  return result;
}

Expression Decimal::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  // this = e
  bool negative = sign() == ExpressionNode::Sign::Negative;
  int exp = node()->exponent();
  NaturalIntegerPointer m = node()->mantissa();
  Integer numerator(&m);
  removeZeroAtTheEnd(&numerator);
  int numberOfDigits = Integer::NumberOfBase10Digits(numerator);
  Integer denominator(1);
  if (exp >= numberOfDigits-1) {
    numerator = Integer::Multiplication(numerator, Integer::Power(Integer(10), Integer(exp-numberOfDigits+1)));
  } else {
    denominator = Integer::Power(Integer(10), Integer(numberOfDigits-1-exp));
  }
  numerator.setNegative(negative);
  if (numerator.isInfinity() || denominator.isInfinity()) {
    return Number::FloatNumber(m.approximate<double>()*std::pow(10.0, (double)exp));
  }
  return Rational(numerator, denominator);
}

Expression Decimal::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  if (sign() == ExpressionNode::Sign::Negative) {
    Expression abs = setSign(ExpressionNode::Sign::Positive, context, angleUnit);
    return Opposite(abs);
  }
  return *this;
}

template Decimal::Decimal(double);
template Decimal::Decimal(float);

}
