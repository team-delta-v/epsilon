#include <poincare/expression_layout.h>
#include "empty_visible_layout.h"
#include "horizontal_layout.h"
#include "matrix_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <assert.h>
#include <stdlib.h>
#include <ion/display.h>

namespace Poincare {

ExpressionLayout::ExpressionLayout() :
  m_parent(nullptr),
  m_baseline(0),
  m_sized(false),
  m_baselined(false),
  m_positioned(false),
  m_frame(KDRectZero)
{
}

void ExpressionLayout::draw(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  int i = 0;
  while (ExpressionLayout * c = editableChild(i++)) {
    c->draw(ctx, p, expressionColor, backgroundColor);
  }
  render(ctx, absoluteOrigin().translatedBy(p), expressionColor, backgroundColor);
}

KDPoint ExpressionLayout::origin() {
  if (m_parent == nullptr) {
    return absoluteOrigin();
  } else {
    return KDPoint(absoluteOrigin().x() - m_parent->absoluteOrigin().x(),
        absoluteOrigin().y() - m_parent->absoluteOrigin().y());
  }
}

KDPoint ExpressionLayout::absoluteOrigin() {
  if (!m_positioned) {
    if (m_parent != nullptr) {
      m_frame.setOrigin(m_parent->absoluteOrigin().translatedBy(m_parent->positionOfChild(this)));
    } else {
      m_frame.setOrigin(KDPointZero);
    }
    m_positioned = true;
  }
  return m_frame.origin();
}

KDSize ExpressionLayout::size() {
  if (!m_sized) {
    m_frame.setSize(computeSize());
    m_sized = true;
  }
  return m_frame.size();
}

KDCoordinate ExpressionLayout::baseline() {
  if (!m_baselined) {
    computeBaseline();
    m_baselined = true;
  }
  return m_baseline;
}


void ExpressionLayout::invalidAllSizesPositionsAndBaselines() {
  m_sized = false;
  m_positioned = false;
  m_baselined = false;
  for (int i = 0; i < numberOfChildren(); i++) {
    editableChild(i)->invalidAllSizesPositionsAndBaselines();
  }
}

const ExpressionLayout * ExpressionLayout::child(int i) const {
  assert(i >= 0);
  if (i < numberOfChildren()) {
    assert(children()[i]->parent() == nullptr || children()[i]->parent() == this);
    return children()[i];
  }
  return nullptr;
}

int ExpressionLayout::indexOfChild(const ExpressionLayout * child) const {
  if (child == nullptr) {
    return -1;
  }
  for (int i = 0; i < numberOfChildren(); i++) {
    if (children()[i] == child) {
      return i;
    }
  }
  return -1;
}

bool ExpressionLayout::hasAncestor(const ExpressionLayout * e) const {
  assert(m_parent != this);
  if (m_parent == e) {
    return true;
  }
  if (m_parent == nullptr) {
    return false;
  }
  return m_parent->hasAncestor(e);
}

void ExpressionLayout::addBrother(ExpressionLayoutCursor * cursor, ExpressionLayout * brother) {
  // First, assess the special case when the layout is horizontal.
  // If so, add the "brother" as a child.
  if (isHorizontal()) {
    // If there is only one empty child, remove it before adding the layout.
    if (numberOfChildren() == 1 && editableChild(0)->isEmpty()) {
      removeChildAtIndex(0, true);
    }
    if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
      addChildAtIndex(brother, 0);
      return;
    }
    assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
    addChildAtIndex(brother, numberOfChildren());
    return;
  }

  // If the layout is not horizontal, it must have a parent.
  assert(m_parent);
  int brotherIndex = cursor->position() == ExpressionLayoutCursor::Position::Left ? m_parent->indexOfChild(this) : m_parent->indexOfChild(this) + 1;
  if (m_parent->isHorizontal()) {
    static_cast<HorizontalLayout *>(m_parent)->addOrMergeChildAtIndex(brother, brotherIndex, true);
    return;
  }
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    replaceWithJuxtapositionOf(brother, this, false);
    return;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  replaceWithJuxtapositionOf(this, brother, false);
  return;
}

ExpressionLayout * ExpressionLayout::replaceWith(ExpressionLayout * newChild, bool deleteAfterReplace) {
  assert(m_parent != nullptr);
  m_parent->replaceChild(this, newChild, deleteAfterReplace);
  return newChild;
}

ExpressionLayout * ExpressionLayout::replaceWithAndMoveCursor(ExpressionLayout * newChild, bool deleteAfterReplace, ExpressionLayoutCursor * cursor) {
  assert(m_parent != nullptr);
  m_parent->replaceChildAndMoveCursor(this, newChild, deleteAfterReplace, cursor);
  return newChild;
}

void ExpressionLayout::replaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild) {
  assert(newChild != nullptr);
  // Caution: handle the case where we replace an operand with a descendant of ours.
  if (newChild->hasAncestor(this)) {
    newChild->editableParent()->detachChild(newChild);
  }
  ExpressionLayout ** op = const_cast<ExpressionLayout **>(children());
  for (int i = 0; i < numberOfChildren(); i++) {
    if (op[i] == oldChild) {
      if (oldChild != nullptr && oldChild->parent() == this) {
        const_cast<ExpressionLayout *>(oldChild)->setParent(nullptr);
      }
      if (deleteOldChild) {
        delete oldChild;
      }
      if (newChild != nullptr) {
        const_cast<ExpressionLayout *>(newChild)->setParent(this);
      }
      op[i] = newChild;
      break;
    }
  }
  m_sized = false;
  m_positioned = false;
  m_baselined = false;
}

void ExpressionLayout::replaceChildAndMoveCursor(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor) {
  assert(indexOfChild(oldChild) >= 0);
  if (!newChild->hasAncestor(oldChild)) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
  }
  replaceChild(oldChild, newChild, deleteOldChild);
  cursor->setPointedExpressionLayout(newChild);
}

void ExpressionLayout::detachChild(const ExpressionLayout * e) {
  assert(indexOfChild(e) >= 0);
  detachChildAtIndex(indexOfChild(e));
}

void ExpressionLayout::detachChildren() {
  for (int i = 0; i < numberOfChildren(); i++) {
    detachChildAtIndex(i);
  }
}

void ExpressionLayout::removeChildAtIndex(int index, bool deleteAfterRemoval) {
  assert(index >= 0 && index < numberOfChildren());
  replaceChild(editableChild(index), new EmptyVisibleLayout(), deleteAfterRemoval);
}

void ExpressionLayout::removePointedChildAtIndexAndMoveCursor(int index, bool deleteAfterRemoval, ExpressionLayoutCursor * cursor) {
  assert(index >= 0 && index < numberOfChildren());
  removeChildAtIndex(index, deleteAfterRemoval);
  if (index < numberOfChildren()) {
    cursor->setPointedExpressionLayout(editableChild(index));
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return;
  }
  int indexOfNewPointedLayout = index - 1;
  assert(indexOfNewPointedLayout >= 0);
  assert(indexOfNewPointedLayout < numberOfChildren());
  cursor->setPointedExpressionLayout(editableChild(indexOfNewPointedLayout));
}


bool ExpressionLayout::insertLayoutAtCursor(ExpressionLayout * newChild, ExpressionLayoutCursor * cursor) {
  cursor->pointedExpressionLayout()->addBrother(cursor, newChild);
  return true;
}

void ExpressionLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  int indexOfPointedExpression = indexOfChild(cursor->pointedExpressionLayout());
  if (indexOfPointedExpression >= 0) {
    // Case: The pointed layout is a child.
    // Move Left.
    assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
    cursor->moveLeft();
    return;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: this is the pointed layout.
  if (m_parent == nullptr) {
    // Case: No parent.
    // Return.
    return;
  }
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    // Case: Left.
    // Ask the parent.
    if (m_parent) {
      m_parent->backspaceAtCursor(cursor);
    }
    return;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right.
  // Delete the layout.
  m_parent->removePointedChildAtIndexAndMoveCursor(m_parent->indexOfChild(this), true, cursor);
}

char ExpressionLayout::XNTChar() const {
  if (m_parent == nullptr) {
    return 'x';
  }
  return m_parent->XNTChar();
}

bool ExpressionLayout::moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  if (m_parent) {
    return m_parent->moveUp(cursor, shouldRecomputeLayout, this, previousLayout);
  }
  return false;
}

bool ExpressionLayout::moveUpInside(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  return moveInside(VerticalDirection::Up, cursor, shouldRecomputeLayout);
}

bool ExpressionLayout::moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  if (m_parent) {
    return m_parent->moveDown(cursor, shouldRecomputeLayout, this, previousLayout);
  }
  return false;
}

bool ExpressionLayout::moveDownInside(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  return moveInside(VerticalDirection::Down, cursor, shouldRecomputeLayout);
}

bool ExpressionLayout::canBeOmittedMultiplicationLeftFactor() const {
  // WARNING: canBeOmittedMultiplicationLeftFactor is true when and only when
  // isCollapsable is true too. If isCollapsable changes, it might not be the
  // case anymore so make sure to modify this function if needed.
  int numberOfOpenParentheses = 0;
  return isCollapsable(&numberOfOpenParentheses, true);
}

bool ExpressionLayout::canBeOmittedMultiplicationRightFactor() const {
  // WARNING: canBeOmittedMultiplicationLeftFactor is true when and only when
  // isCollapsable is true and mustHaveLeftBrother is false. If one of these
  // functions changes, , it might not be the case anymore so make sure to
  // modify canBeOmittedMultiplicationRightFactor if needed.
  int numberOfOpenParentheses = 0;
  return isCollapsable(&numberOfOpenParentheses, false) && !mustHaveLeftBrother();
}

void ExpressionLayout::detachChildAtIndex(int i) {
  ExpressionLayout ** op = const_cast<ExpressionLayout **>(children());
  if (op[i] != nullptr && op[i]->parent() == this) {
    const_cast<ExpressionLayout *>(op[i])->setParent(nullptr);
  }
  op[i] = nullptr;
  m_sized = false;
  m_positioned = false;
  m_baselined = false;
}

bool ExpressionLayout::moveInside(VerticalDirection direction, ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  ExpressionLayout *  chilResult = nullptr;
  ExpressionLayout ** childResultPtr = &chilResult;
  ExpressionLayoutCursor::Position resultPosition = ExpressionLayoutCursor::Position::Left;
  // The distance between the cursor and its next position cannot be greater
  // than this initial value of score.
  int resultScore = Ion::Display::Width*Ion::Display::Width + Ion::Display::Height*Ion::Display::Height;

  moveCursorInsideAtDirection(direction, cursor, shouldRecomputeLayout, childResultPtr, &resultPosition, &resultScore);

  // If there is a valid result
  if (*childResultPtr == nullptr) {
    return false;
  }
  cursor->setPointedExpressionLayout(*childResultPtr);
  cursor->setPosition(resultPosition);
  return true;
}

void ExpressionLayout::moveCursorInsideAtDirection (
    VerticalDirection direction,
    ExpressionLayoutCursor * cursor,
    bool * shouldRecomputeLayout,
    ExpressionLayout ** childResult,
    void * resultPosition,
    int * resultScore)
{
  ExpressionLayoutCursor::Position * castedResultPosition = static_cast<ExpressionLayoutCursor::Position *>(resultPosition);
  KDPoint cursorMiddleLeft = cursor->middleLeftPoint();
  bool layoutIsUnderOrAbove = direction == VerticalDirection::Up ? m_frame.isAbove(cursorMiddleLeft) : m_frame.isUnder(cursorMiddleLeft);
  bool layoutContains = m_frame.contains(cursorMiddleLeft);

  if (layoutIsUnderOrAbove) {
    // Check the distance to a Left cursor.
    int currentDistance = cursor->middleLeftPointOfCursor(this, ExpressionLayoutCursor::Position::Left).squareDistanceTo(cursorMiddleLeft);
    if (currentDistance <= *resultScore ){
      *childResult = this;
      *castedResultPosition = ExpressionLayoutCursor::Position::Left;
      *resultScore = currentDistance;
    }

    // Check the distance to a Right cursor.
    currentDistance = cursor->middleLeftPointOfCursor(this, ExpressionLayoutCursor::Position::Right).squareDistanceTo(cursorMiddleLeft);
    if (currentDistance < *resultScore) {
      *childResult = this;
      *castedResultPosition = ExpressionLayoutCursor::Position::Right;
      *resultScore = currentDistance;
    }
  }
  if (layoutIsUnderOrAbove || layoutContains) {
    int childIndex = 0;
    while (child(childIndex++)) {
      editableChild(childIndex-1)->moveCursorInsideAtDirection(direction, cursor, shouldRecomputeLayout, childResult, castedResultPosition, resultScore);
    }
  }
}

ExpressionLayout * ExpressionLayout::replaceWithJuxtapositionOf(ExpressionLayout * leftChild, ExpressionLayout * rightChild, bool deleteAfterReplace) {
  assert(m_parent != nullptr);
  /* One of the children to juxtapose might be "this", so we first have to
   * replace "this" with an horizontal layout, then add "this" to the layout. */
  ExpressionLayout * layout = new HorizontalLayout();
  m_parent->replaceChild(this, layout, deleteAfterReplace);
  layout->addChildAtIndex(leftChild, 0);
  layout->addChildAtIndex(rightChild, 1);
  return layout;
}

}
