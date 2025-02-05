#include "history_view_cell.h"
#include "app.h"
#include "../constant.h"
#include "selectable_table_view.h"
#include <assert.h>
#include <string.h>

namespace Calculation {

static inline KDCoordinate minCoordinate(KDCoordinate x, KDCoordinate y) { return x < y ? x : y; }
static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

/* HistoryViewCellDataSource */

HistoryViewCellDataSource::HistoryViewCellDataSource() :
  m_selectedSubviewType(SubviewType::Output) {}

void HistoryViewCellDataSource::setSelectedSubviewType(SubviewType subviewType, HistoryViewCell * cell) {
  m_selectedSubviewType = subviewType;
  if (cell) {
    cell->setHighlighted(cell->isHighlighted());
    cell->cellDidSelectSubview(subviewType);
  }
  historyViewCellDidChangeSelection();
}

/* HistoryViewCell */

HistoryViewCell::HistoryViewCell(Responder * parentResponder) :
  Responder(parentResponder),
  m_calculation(),
  m_calculationExpanded(false),
  m_inputView(this),
  m_scrollableOutputView(this)
{
}

Shared::ScrollableExactApproximateExpressionsView * HistoryViewCell::outputView() {
  return &m_scrollableOutputView;
}

void HistoryViewCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_inputView.setBackgroundColor(backgroundColor());
  m_scrollableOutputView.setBackgroundColor(backgroundColor());
  m_scrollableOutputView.evenOddCell()->setEven(even);
}

void HistoryViewCell::setHighlighted(bool highlight) {
  assert(m_dataSource);
  m_highlighted = highlight;
  m_inputView.setExpressionBackgroundColor(backgroundColor());
  m_scrollableOutputView.evenOddCell()->setHighlighted(false);
  if (isHighlighted()) {
    if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) {
      m_inputView.setExpressionBackgroundColor(Palette::Select);
    } else {
      m_scrollableOutputView.evenOddCell()->setHighlighted(true);
    }
  }
}

Poincare::Layout HistoryViewCell::layout() const {
  assert(m_dataSource);
  if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) {
    return m_inputView.layout();
  } else {
    return m_scrollableOutputView.layout();
  }
}

void HistoryViewCell::reloadScroll() {
  m_inputView.reloadScroll();
  m_scrollableOutputView.reloadScroll();
}

void HistoryViewCell::reloadOutputSelection() {
  App * calculationApp = (App *)app();
  Calculation::DisplayOutput display = m_calculation.displayOutput(calculationApp->localContext());
  /* Select the right output according to the calculation display output. This
   * will reload the scroll to display the selected output. */
  if (display == Calculation::DisplayOutput::ExactAndApproximate) {
    m_scrollableOutputView.setSelectedSubviewPosition(Shared::ScrollableExactApproximateExpressionsView::SubviewPosition::Left);
  } else {
    assert(display == Calculation::DisplayOutput::ApproximateOnly || (display == Calculation::DisplayOutput::ExactAndApproximateToggle) || display == Calculation::DisplayOutput::ExactOnly);
    m_scrollableOutputView.setSelectedSubviewPosition(Shared::ScrollableExactApproximateExpressionsView::SubviewPosition::Right);
  }
}

void HistoryViewCell::cellDidSelectSubview(HistoryViewCellDataSource::SubviewType type) {
  // Init output selection
  if (type == HistoryViewCellDataSource::SubviewType::Output) {
    reloadOutputSelection();
  }
  /* The selected subview has changed. The displayed outputs might have changed.
   * For example, for the calculation 1.2+2 --> 3.2, selecting the output would
   * display 1.2+2 --> 16/5 = 3.2. */
  setCalculation(&m_calculation, type == HistoryViewCellDataSource::SubviewType::Output);
  // Reload scroll when switching from one subview to another
  reloadScroll();
}

KDColor HistoryViewCell::backgroundColor() const {
  KDColor background = m_even ? KDColorWhite : Palette::WallScreen;
  return background;
}


int HistoryViewCell::numberOfSubviews() const {
  return 2;
}

View * HistoryViewCell::subviewAtIndex(int index) {
  View * views[2] = {&m_inputView, &m_scrollableOutputView};
  return views[index];
}

void HistoryViewCell::layoutSubviews() {
  KDCoordinate maxFrameWidth = bounds().width();
  KDSize inputSize = m_inputView.minimalSizeForOptimalDisplay();
  m_inputView.setFrame(KDRect(
    0,
    0,
    minCoordinate(maxFrameWidth, inputSize.width()),
    inputSize.height()
  ));

  KDSize outputSize = m_scrollableOutputView.minimalSizeForOptimalDisplay();
  int outputY = oneLine() ? maxCoordinate(0, inputSize.height() - outputSize.height()) / 2 : inputSize.height();

  m_scrollableOutputView.setFrame(KDRect(
    maxCoordinate(0, maxFrameWidth - outputSize.width()),
    outputY,
    minCoordinate(maxFrameWidth, outputSize.width()),
    oneLine() ? outputSize.height() : (bounds().height() - inputSize.height())
  ));
}

bool HistoryViewCell::oneLine() {
  KDSize inputSize = m_inputView.minimalSizeForOptimalDisplay();
  KDSize outputSize = m_scrollableOutputView.minimalSizeForOptimalDisplay();
  return outputSize.width() + inputSize.width() < bounds().width() - 6;
}

void HistoryViewCell::setCalculation(Calculation * calculation, bool expanded) {
  if (m_calculationExpanded == expanded && *calculation == m_calculation) {
    return;
  }
  // Memoization
  m_calculation = *calculation;
  m_calculationExpanded = expanded;
  App * calculationApp = (App *)app();
  Calculation::DisplayOutput display = calculation->displayOutput(calculationApp->localContext());
  m_inputView.setLayout(calculation->createInputLayout());
  /* Both output expressions have to be updated at the same time. Otherwise,
   * when updating one layout, if the second one still points to a deleted
   * layout, calling to layoutSubviews() would fail. */
  Poincare::Layout leftOutputLayout = Poincare::Layout();
  Poincare::Layout rightOutputLayout;
  if (display == Calculation::DisplayOutput::ExactOnly) {
    rightOutputLayout = calculation->createExactOutputLayout();
  } else {
    rightOutputLayout = calculation->createApproximateOutputLayout(calculationApp->localContext());
    if (display == Calculation::DisplayOutput::ExactAndApproximate || (display == Calculation::DisplayOutput::ExactAndApproximateToggle && expanded)) {
      leftOutputLayout = calculation->createExactOutputLayout();
    }
  }
  m_scrollableOutputView.setLayouts(rightOutputLayout, leftOutputLayout);
  I18n::Message equalMessage = calculation->exactAndApproximateDisplayedOutputsAreEqual(calculationApp->localContext()) == Calculation::EqualSign::Equal ? I18n::Message::Equal : I18n::Message::AlmostEqual;
  m_scrollableOutputView.setEqualMessage(equalMessage);

  /* The displayed input and outputs have changed. We need to re-layout the cell
   * and re-initialize the scroll. */
  layoutSubviews();
  reloadScroll();
}

void HistoryViewCell::didBecomeFirstResponder() {
  assert(m_dataSource);
  if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) {
    app()->setFirstResponder(&m_inputView);
  } else {
    app()->setFirstResponder(&m_scrollableOutputView);
  }
}

bool HistoryViewCell::handleEvent(Ion::Events::Event event) {
  assert(m_dataSource);
  if ((event == Ion::Events::Down && m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) ||
    (event == Ion::Events::Up && m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Output)) {
    HistoryViewCellDataSource::SubviewType otherSubviewType = m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input ? HistoryViewCellDataSource::SubviewType::Output : HistoryViewCellDataSource::SubviewType::Input;
    m_dataSource->setSelectedSubviewType(otherSubviewType, this);
    CalculationSelectableTableView * tableView = (CalculationSelectableTableView *)parentResponder();
    tableView->scrollToSubviewOfTypeOfCellAtLocation(otherSubviewType, tableView->selectedColumn(), tableView->selectedRow());
    app()->setFirstResponder(this);
    return true;
  }
  return false;
}

}
