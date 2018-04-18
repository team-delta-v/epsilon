#include <escher/expression_view_with_cursor.h>
#include <kandinsky/rect.h>
#include <assert.h>

using namespace Poincare;

ExpressionViewWithCursor::ExpressionViewWithCursor(ExpressionLayout * expressionLayout) :
  m_cursor(),
  m_expressionView(),
  m_cursorView(),
  m_isEditing(false)
{
  m_cursor.setPointedExpressionLayout(expressionLayout);
  m_cursor.setPosition(ExpressionLayoutCursor::Position::Right);
  m_expressionView.setExpressionLayout(expressionLayout);
}

void ExpressionViewWithCursor::setEditing(bool isEditing) {
  m_isEditing = isEditing;
  markRectAsDirty(bounds());
  layoutSubviews();
}

void ExpressionViewWithCursor::cursorPositionChanged() {
  layoutCursorSubview();
}

KDRect ExpressionViewWithCursor::cursorRect() {
  return m_cursorView.frame();
}

KDSize ExpressionViewWithCursor::minimalSizeForOptimalDisplay() const {
  KDSize expressionViewSize = m_expressionView.minimalSizeForOptimalDisplay();
  KDSize cursorSize = isEditing() ? m_cursorView.minimalSizeForOptimalDisplay() : KDSizeZero;
  KDCoordinate resultWidth = expressionViewSize.width() + cursorSize.width();
  KDCoordinate resultHeight = expressionViewSize.height() + cursorSize.height()/2;
  return KDSize(resultWidth, resultHeight);
}

View * ExpressionViewWithCursor::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  View * m_views[] = {&m_expressionView, &m_cursorView};
  return m_views[index];
}

void ExpressionViewWithCursor::layoutSubviews() {
  m_expressionView.setFrame(bounds());
  layoutCursorSubview();
}

void ExpressionViewWithCursor::layoutCursorSubview() {
  if (!m_isEditing) {
    m_cursorView.setFrame(KDRectZero);
    return;
  }
  KDPoint expressionViewOrigin = m_expressionView.absoluteDrawingOrigin();
  ExpressionLayout * pointedLayout = m_cursor.pointedExpressionLayoutEquivalentChild();
  KDPoint cursoredExpressionViewOrigin = pointedLayout->absoluteOrigin();
  KDCoordinate cursorX = expressionViewOrigin.x() + cursoredExpressionViewOrigin.x();
  if (m_cursor.position() == ExpressionLayoutCursor::Position::Right) {
    cursorX += pointedLayout->size().width();
  }
  KDPoint cursorTopLeftPosition(cursorX, expressionViewOrigin.y() + cursoredExpressionViewOrigin.y() + pointedLayout->baseline() - m_cursor.baseline());
  m_cursorView.setFrame(KDRect(cursorTopLeftPosition, 1, m_cursor.cursorHeight()));
}
