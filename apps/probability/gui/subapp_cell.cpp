#include "subapp_cell.h"

#include <kandinsky/font.h>

namespace Probability {

TitleAndSubtitleView::TitleAndSubtitleView() {
  m_title.setFont(KDFont::LargeFont);
  m_subTitle.setFont(KDFont::SmallFont);
  m_subTitle.setTextColor(Palette::GrayDark);
}

Escher::HighlightCell * TitleAndSubtitleView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  return i == 0 ? &m_title : &m_subTitle;
}

void TitleAndSubtitleView::setMessages(I18n::Message title, I18n::Message subTitle) {
  m_title.setMessage(title);
  m_subTitle.setMessage(subTitle);
}

void SubappCell::setHighlighted(bool highlighted) {
  m_icon.setHighlighted(highlighted);
  m_titleAndSubtitleView.titleView()->setHighlighted(highlighted);
  m_titleAndSubtitleView.subTitleView()->setHighlighted(highlighted);
  HighlightCell::setHighlighted(highlighted);
}

Escher::View * SubappCell::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  switch (i) {
    case 0:
      return &m_icon;
    case 1:
      return &m_titleAndSubtitleView;
    case 2:
      return &m_chevron;
  }
  return nullptr;
}

void SubappCell::setImage(const Escher::Image * image) {
  m_icon.setImage(image);
}

void SubappCell::setMessages(I18n::Message title, I18n::Message subTitle) {
  m_titleAndSubtitleView.setMessages(title, subTitle);
}

}  // namespace Probability
