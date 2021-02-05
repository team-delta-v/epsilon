#ifndef SETTINGS_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_WITH_SEPARATOR_H
#define SETTINGS_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_WITH_SEPARATOR_H

#include <escher/message_table_cell_with_editable_text.h>
#include "cell_with_separator.h"

namespace Settings {

class MessageTableCellWithEditableTextWithSeparator : public CellWithSeparator {
public:
  MessageTableCellWithEditableTextWithSeparator(Escher::Responder * parentResponder = nullptr, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, Escher::TextFieldDelegate * textFieldDelegate = nullptr, I18n::Message message = (I18n::Message)0) :
    CellWithSeparator(),
    m_cell(parentResponder, inputEventHandlerDelegate, textFieldDelegate, message) {}
  const char * text() const override { return m_cell.text(); }
  Poincare::Layout layout() const override { return m_cell.layout(); }
  Escher::MessageTableCellWithEditableText * messageTableCellWithEditableText() { return &m_cell; }
  /* Being const, we cannot set m_cell width to the expected width. It must then
   * be handled here. Additionally, subLabelView is ignored. See comment in
   * MessageTableCellWithEditableText::minimalSizeForOptimalDisplay */
  KDSize minimalSizeForOptimalDisplay() const override {
    // Available width is necessary to compute it minimal height.
    KDCoordinate expectedWidth = m_frame.width();
    assert(expectedWidth > 0);
    return KDSize(expectedWidth, Escher::TableCell::minimalHeightForOptimalDisplay(m_cell.labelView(), nullptr, m_cell.accessoryView(), expectedWidth) + k_margin + Escher::Metric::CellSeparatorThickness);
  }
private:
  Escher::HighlightCell * cell() override { return &m_cell; }
  Escher::MessageTableCellWithEditableText m_cell;
};

}

#endif
