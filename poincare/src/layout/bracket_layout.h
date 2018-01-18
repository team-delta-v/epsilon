#ifndef POINCARE_BRACKET_LAYOUT_H
#define POINCARE_BRACKET_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

class BracketLayout : public StaticLayoutHierarchy<1> {
public:
  using StaticLayoutHierarchy::StaticLayoutHierarchy;
  ExpressionLayout * clone() const override;
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;
  bool moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout = nullptr) override;
  bool moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout = nullptr) override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;
protected:
  ExpressionLayout * operandLayout();
  KDCoordinate externWidthMargin() const { return 2; }
  virtual KDCoordinate widthMargin() const { return 5; }
  virtual bool renderTopBar() const { return true; }
  virtual bool renderBottomBar() const { return true; }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_bracketWidth = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
};

}

#endif

