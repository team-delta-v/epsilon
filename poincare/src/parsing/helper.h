#ifndef POINCARE_PARSING_HELPER_H
#define POINCARE_PARSING_HELPER_H

#include <poincare_expressions.h>

namespace Poincare {

class ParsingHelper {

public:
  /* The method GetReservedFunction passes through the successive
   * entries of the reserved functions array in order to determine
   * whether a token corresponds to an entry. The constexpr static
   * s_reservedFunctionsUpperBound marks the end of the array. */
  static const Expression::FunctionHelper * const * GetReservedFunction(const char * name, size_t nameLength);
  static const Expression::FunctionHelper * const * GetInverseFunction(const char * name, size_t nameLength);
  static const Expression::FunctionHelper * const * ReservedFunctionsUpperBound() { return s_reservedFunctionsUpperBound; }
  static bool IsSpecialIdentifierName(const char * name, size_t nameLength);
  static bool IsParameteredExpression(const Expression::FunctionHelper * helper);
  static bool IsSquarableFunction(const Expression::FunctionHelper * helper);

  // This must be called with an identifier name
  typedef Expression (*IdentifierBuilder) ();
  static const IdentifierBuilder GetIdentifierBuilder(const char * name, size_t nameLength);

private:
  // The array of special identifiers
  struct SpecialIdentifier {
    AliasesList identifierAliasesList;
    IdentifierBuilder identifierBuilder;
  };
  constexpr static SpecialIdentifier s_specialIdentifiers[] = {
    {Symbol::k_ansAliases, [] {return static_cast<Expression>(Symbol::Ans());}},
    {Infinity::Name(), [] {return static_cast<Expression>(Infinity::Builder(false));}},
    {Nonreal::Name(), [] {return static_cast<Expression>(Nonreal::Builder());}},
    {Undefined::Name(), [] {return static_cast<Expression>(Undefined::Builder());}}
  };
  constexpr static int k_numberOfSpecialIdentifiers = sizeof(s_specialIdentifiers) / sizeof(SpecialIdentifier);

  static int SpecialIdentifierIndexForName(const char * name, size_t nameLength);

  // The array of reserved functions' helpers
  constexpr static const Expression::FunctionHelper * s_reservedFunctions[] = {
    /* This MUST be ordered according to name, and then by numberOfChildren
     * otherwise GetReservedFunction won't work properly.
     * If the function has multiple aliases, take the first alias
     * in alphabetical order to choose position in list.
     *
     * Example:
     * { acos, arccos } -> take acos as reference for position
     * { asin, arcsin } -> take arcsin as reference for position
     *
     * The // at the end of each line indicates the first name in alphabetical
     * order
     * =======================
     * WARNING: For now all function aliases that can be used in a country
     * can also be used in other languages. If it was no more the case, this
     * implementation would be deprecated.
     *
     * Ex:
     * if "sen = sin" is understood in PT but not in EN:
     * When parsing "sign", the name comparison with { "sen", "sin" } occurs
     * before the comparison with "sign". But if "sen" was skipped, the
     * comparison would only be between "sin" and "sign" which is < 0.
     * So the iteration through the list would stop and not recognize "sign".
     * */
    &AbsoluteValue::s_functionHelper,
    &ArcCosine::s_functionHelper, // acos
    &ArcCosecant::s_functionHelper, // acosec
    &HyperbolicArcCosine::s_functionHelper, // acosh
    &ArcCotangent::s_functionHelper, // acot
    &ArcSecant::s_functionHelper, // arcsec
    &ArcSine::s_functionHelper, // arcsen
    &ArcTangent::s_functionHelper, // arctan
    &ComplexArgument::s_functionHelper, // arg
    &HyperbolicArcSine::s_functionHelper, // arsenh
    &HyperbolicArcTangent::s_functionHelper, // artanh
    &BinomCDF::s_functionHelper,
    &BinomialCoefficient::s_functionHelper,
    &BinomPDF::s_functionHelper,
    &Ceiling::s_functionHelper,
    &Conjugate::s_functionHelper,
    &Cosine::s_functionHelper,
    &Cosecant::s_functionHelper, // cosec
    &HyperbolicCosine::s_functionHelper,
    &Cotangent::s_functionHelper, // cot
    &VectorCross::s_functionHelper,
    &Dependency::s_functionHelper,
    &Determinant::s_functionHelper,
    &Derivative::s_functionHelperFirstOrder,
    &Derivative::s_functionHelper,
    &Dimension::s_functionHelper,
    &VectorDot::s_functionHelper,
    &Factor::s_functionHelper,
    &Floor::s_functionHelper,
    &FracPart::s_functionHelper,
    &GreatCommonDivisor::s_functionHelper,
    &GeomCDF::s_functionHelper,
    &GeomCDFRange::s_functionHelper,
    &GeomPDF::s_functionHelper,
    &MatrixIdentity::s_functionHelper,
    &ImaginaryPart::s_functionHelper,
    &Integral::s_functionHelper,
    &InvBinom::s_functionHelper,
    &MatrixInverse::s_functionHelper,
    &InvGeom::s_functionHelper,
    &InvNorm::s_functionHelper,
    &InvStudent::s_functionHelper,
    &LeastCommonMultiple::s_functionHelper,
    &NaperianLogarithm::s_functionHelper,
    &Logarithm::s_functionHelper,
    &ListMaximum::s_functionHelper,
    &ListMean::s_functionHelper,
    &ListMedian::s_functionHelper,
    &ListMinimum::s_functionHelper,
    &VectorNorm::s_functionHelper,
    &NormCDF::s_functionHelper,
    &NormCDFRange::s_functionHelper,
    &NormPDF::s_functionHelper,
    &PermuteCoefficient::s_functionHelper,
    &PoissonCDF::s_functionHelper,
    &PoissonPDF::s_functionHelper,
    &ListProduct::s_functionHelper,
    &Product::s_functionHelper,
    &DivisionQuotient::s_functionHelper,
    &Randint::s_functionHelper,
    &Random::s_functionHelper,
    &RealPart::s_functionHelper,
    &MatrixRowEchelonForm::s_functionHelper,
    &DivisionRemainder::s_functionHelper,
    &NthRoot::s_functionHelper,
    &Round::s_functionHelper,
    &MatrixReducedRowEchelonForm::s_functionHelper,
    &ListSampleStandardDeviation::s_functionHelper,
    &Secant::s_functionHelper,
    &Sine::s_functionHelper, // sen
    &HyperbolicSine::s_functionHelper, // senh
    &ListSequence::s_functionHelper,
    &SignFunction::s_functionHelper,
    &ListSort::s_functionHelper,
    &ListStandardDeviation::s_functionHelper,
    &ListSum::s_functionHelper,
    &Sum::s_functionHelper,
    &Tangent::s_functionHelper, // tan
    &HyperbolicTangent::s_functionHelper,
    &StudentCDF::s_functionHelper,
    &StudentCDFRange::s_functionHelper,
    &StudentPDF::s_functionHelper,
    &MatrixTrace::s_functionHelper,
    &MatrixTranspose::s_functionHelper,
    &ListVariance::s_functionHelper,
    &SquareRoot::s_functionHelper,
  };

  constexpr static const Expression::FunctionHelper * const * s_reservedFunctionsUpperBound = s_reservedFunctions + (sizeof(s_reservedFunctions)/sizeof(Expression::FunctionHelper *));

  // The array of functions that can be aliases to f^-1
  struct FunctionMapping { const Expression::FunctionHelper * mainFunction; const Expression::FunctionHelper * inverseFunction; };
  constexpr static const FunctionMapping s_inverses[] = {
    {&Cosine::s_functionHelper, &ArcCosine::s_functionHelper},
    {&Sine::s_functionHelper, &ArcSine::s_functionHelper},
    {&Tangent::s_functionHelper, &ArcTangent::s_functionHelper},
  };
  constexpr static int k_numberOfInverses = sizeof(s_inverses) / sizeof(FunctionMapping);
  constexpr static FunctionMapping const * s_inverseFunctionsUpperBound = s_inverses + (k_numberOfInverses);
};

}

#endif
