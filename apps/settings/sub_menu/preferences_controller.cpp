#include "preferences_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include <assert.h>
#include <cmath>
#include <poincare/layout_helper.h>
#include <poincare/code_point_layout.h>
#include <poincare/fraction_layout.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;

namespace Settings {

PreferencesController::PreferencesController(Responder * parentResponder) :
  GenericSubController(parentResponder)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
  }
}

void PreferencesController::didBecomeFirstResponder() {
  selectCellAtLocation(0, valueIndexForPreference(m_messageTreeModel->label()));
  app()->setFirstResponder(&m_selectableTableView);
}

bool PreferencesController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    /* Generic behaviour of preference menu*/
    assert(m_messageTreeModel->label() != I18n::Message::DisplayMode || selectedRow() != numberOfRows()-1); // In that case, events OK and EXE are handled by the cell
    setPreferenceWithValueIndex(m_messageTreeModel->label(), selectedRow());
    AppsContainer * myContainer = (AppsContainer * )app()->container();
    myContainer->refreshPreferences();
    StackViewController * stack = stackController();
    stack->pop();
    return true;
  }
  return GenericSubController::handleEvent(event);
}

HighlightCell * PreferencesController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_cells[index];
}

int PreferencesController::reusableCellCount(int type) {
  return k_totalNumberOfCell;
}

Layout PreferencesController::layoutForPreferences(I18n::Message message) {
  switch (message) {
    // Angle Unit
    case I18n::Message::Degres:
    {
      const char * degEx = "90°";
      return LayoutHelper::String(degEx, strlen(degEx), k_layoutFont);
    }
    case I18n::Message::Gradians:
    {
      const char * degEx = "100 gon";
      return LayoutHelper::String(degEx, strlen(degEx), k_layoutFont);
    }
    case I18n::Message::Radian:
      return FractionLayout::Builder(
          CodePointLayout::Builder(UCodePointGreekSmallLetterPi, k_layoutFont),
          CodePointLayout::Builder('2', k_layoutFont));

    // Display Mode format
    case I18n::Message::Decimal:
      return LayoutHelper::String("12.34", 5, k_layoutFont);
    case I18n::Message::Scientific:
    {
      const char * text = "1.234ᴇ1";
      return LayoutHelper::String(text, strlen(text), k_layoutFont);
    }

    // Edition mode
    case I18n::Message::Edition2D:
      return HorizontalLayout::Builder(
          LayoutHelper::String("1+", 2, k_layoutFont),
          FractionLayout::Builder(LayoutHelper::String("2", 1, k_layoutFont), LayoutHelper::String("3", 1, k_layoutFont))
        );
    case I18n::Message::EditionLinear:
      return LayoutHelper::String("1+2/3", 5, k_layoutFont);

    // Complex format
    case I18n::Message::Real:
      return CodePointLayout::Builder('x', k_layoutFont);
    case I18n::Message::Cartesian:
    {
      const char * text = "a+𝐢b";
      return LayoutHelper::String(text, strlen(text), k_layoutFont);
    }
    case I18n::Message::Polar:
    {
      const char * base = "rℯ";
      const char * superscript = "𝐢θ";
      return HorizontalLayout::Builder(
          LayoutHelper::String(base, strlen(base), k_layoutFont),
          VerticalOffsetLayout::Builder(LayoutHelper::String(superscript, strlen(superscript), k_layoutFont), VerticalOffsetLayoutNode::Position::Superscript)
        );
    }
    default:
      assert(false);
      return Layout();
  }
}

void PreferencesController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);
  MessageTableCellWithExpression * myCell = (MessageTableCellWithExpression *)cell;
  myCell->setLayout(layoutForPreferences(m_messageTreeModel->children(index)->label()));
}

KDCoordinate PreferencesController::rowHeight(int j) {
  /* We cheat for the Writing format subcontroller, because the Edition2D layout
   * needs more vertical space. */
  return GenericSubController::rowHeight(j) + (m_messageTreeModel->label() == I18n::Message::EditionMode ? 2 : 0);
}

void PreferencesController::setPreferenceWithValueIndex(I18n::Message message, int valueIndex) {
  Preferences * preferences = Preferences::sharedPreferences();
  if (message == I18n::Message::AngleUnit) {
    preferences->setAngleUnit((Preferences::AngleUnit)valueIndex);
  } else if (message == I18n::Message::DisplayMode) {
    preferences->setDisplayMode((Preferences::PrintFloatMode)valueIndex);
  } else if (message == I18n::Message::EditionMode) {
    preferences->setEditionMode((Preferences::EditionMode)valueIndex);
  } else if (message == I18n::Message::ComplexFormat) {
    preferences->setComplexFormat((Preferences::ComplexFormat)valueIndex);
  }
}

int PreferencesController::valueIndexForPreference(I18n::Message message) {
  Preferences * preferences = Preferences::sharedPreferences();
  if (message == I18n::Message::AngleUnit) {
    return (int)preferences->angleUnit();
  }
  if (message == I18n::Message::DisplayMode) {
    return (int)preferences->displayMode();
  }
  if (message == I18n::Message::EditionMode) {
    return (int)preferences->editionMode();
  }
  if (message == I18n::Message::ComplexFormat) {
    return (int)preferences->complexFormat();
  }
  return 0;
}

}
