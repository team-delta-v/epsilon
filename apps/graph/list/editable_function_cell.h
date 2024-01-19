#ifndef ESCHER_EDITABLE_FUNCTION_CELL_H
#define ESCHER_EDITABLE_FUNCTION_CELL_H

#include <escher/button_cell.h>
#include <escher/editable_expression_model_cell.h>
#include <escher/stack_view_controller.h>

#include "function_cell.h"

namespace Graph {

class EditableFunctionCell
    : public TemplatedFunctionCell<Shared::WithEditableExpressionCell> {
 public:
  EditableFunctionCell(Escher::Responder* parentResponder,
                       Escher::LayoutFieldDelegate* layoutFieldDelegate,
                       Escher::StackViewController* modelsStackController);

  void updateSubviewsBackgroundAfterChangingState() override {
    m_expressionBackground = backgroundColor();
  }

  void selectTemplateButton() {
    assert(isEmpty());
    m_buttonCell.setHighlighted(true);
  }

  bool isEmpty() { return expressionCell()->layoutField()->isEmpty(); }
  bool isEditing() { return expressionCell()->layoutField()->isEditing(); }

  void setHighlighted(bool highlighted) override {
    if (!highlighted) {
      m_buttonCell.setHighlighted(false);
    }
    AbstractFunctionCell::setHighlighted(highlighted);
  }

  void updateButton() {
    bool empty = isEmpty();
    bool wasEmpty = m_buttonCell.isVisible();

    if (wasEmpty && !empty) {
      m_buttonCell.setHighlighted(false);
    }
    if (wasEmpty != empty) {
      m_buttonCell.setVisible(empty);
    }
  }

  Escher::ButtonCell* buttonCell() { return &m_buttonCell; }

 private:
  int numberOfSubviews() const override {
    return AbstractFunctionCell::numberOfSubviews() + m_buttonCell.isVisible();
  }
  void layoutSubviews(bool force = false) override;
  Escher::View* subviewAtIndex(int index) override;

  class ButtonCell : public Escher::ButtonCell {
   public:
    ButtonCell(Escher::LayoutField* parentResponder,
               Escher::Invocation invocation)
        : Escher::ButtonCell(parentResponder, I18n::Message::UseFunctionModel,
                             invocation, Escher::Palette::WallScreen, 0,
                             KDFont::Size::Small) {}
    bool handleEvent(Ion::Events::Event event) override;

    void deselect();

    void didEnterResponderChain(Responder* previousFirstResponder) override {
      // This is called when pressing Back from the template menu.
      layoutField()->setEditing(true);
    }

   private:
    Escher::LayoutField* layoutField() const {
      return static_cast<Escher::LayoutField*>(parentResponder());
    }
  };

  static constexpr KDCoordinate k_expressionMargin = 5;
  static constexpr KDCoordinate k_templateButtonMargin = 5;
  ButtonCell m_buttonCell;
};

}  // namespace Graph

#endif
