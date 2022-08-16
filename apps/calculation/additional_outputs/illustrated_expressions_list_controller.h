#ifndef CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATED_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATED_EXPRESSIONS_LIST_CONTROLLER_H

#include "chained_expressions_list_controller.h"
#include "../calculation_store.h"
#include <poincare/variable_context.h>
#include <apps/i18n.h>

namespace Calculation {

class IllustratedExpressionsListController : public ChainedExpressionsListController, public Escher::SelectableTableViewDelegate {
public:
  IllustratedExpressionsListController(EditExpressionController * editExpressionController) :
    ChainedExpressionsListController(editExpressionController, this) {}

  // Responder
  void didEnterResponderChain(Responder * previousFirstResponder) override;

  // MemoizedListViewDataSource
  int numberOfRows() const override;
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  int typeAtIndex(int index) const override { return index == 0 && showIllustration() ? k_illustrationCellType : k_expressionCellType; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  /* Since we can't focus the illustration, we need to keep its height small
   * enough for a sufficient part of the first expressionCell to be visible when
   * the focus is there and the table view scrolled at the top. */
  constexpr static KDCoordinate k_illustrationHeight = 120;
protected:
  constexpr static uint8_t k_illustrationCellType = 0;
  constexpr static uint8_t k_expressionCellType = 1;
  Poincare::VariableContext illustratedExpressionsListContext();
  bool showIllustration() const { return m_showIllustration; }
  void setShowIllustration(bool showIllustration) { m_showIllustration = showIllustration; }
private:
  int textAtIndex(char * buffer, size_t bufferSize, int index) override;
  virtual const char * symbol() const = 0;
  // Cells
  virtual Escher::HighlightCell * illustrationCell() = 0;
  bool m_showIllustration;
};

}

#endif

