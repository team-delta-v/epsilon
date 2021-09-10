#include "homogeneity_data_source.h"

#include <apps/i18n.h>
#include <string.h>

using namespace Probability;

HomogeneityTableDataSource::HomogeneityTableDataSource(
    TableViewDataSource * contentTable,
    Escher::SelectableTableViewDelegate * tableDelegate,
    I18n::Message headerPrefix) :
      ChainedSelectableTableViewDelegate(tableDelegate),
      m_innerDataSource(contentTable),
      m_headerPrefix(headerPrefix),
      m_topLeftCell(Escher::Palette::WallScreenDark) {
  // First row
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    m_colHeader[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    m_colHeader[i].setFont(KDFont::SmallFont);
    m_colHeader[i].setEven(true);
  }
  // First column
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_rowHeader[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    m_rowHeader[i].setFont(KDFont::SmallFont);
    m_rowHeader[i].setEven(i % 2 == 0);
  }
}

HighlightCell * HomogeneityTableDataSource::reusableCell(int i, int type) {
  if (type == k_typeOfTopLeftCell) {
    assert(i == 0);
    return &m_topLeftCell;
  }
  if (type == k_typeOfColumnHeader) {
    return &m_colHeader[i];
  }
  if (type == k_typeOfRowHeader) {
    return &m_rowHeader[i];
  }
  return m_innerDataSource->reusableCell(i, type);
}

int HomogeneityTableDataSource::typeAtLocation(int i, int j) {
  if (i == 0 && j == 0) {
    return k_typeOfTopLeftCell;
  }
  if (j == 0) {
    return k_typeOfColumnHeader;
  }
  if (i == 0) {
    return k_typeOfRowHeader;
  }
  return m_innerDataSource->typeAtLocation(i - 1, j - 1);
}

void Probability::HomogeneityTableDataSource::willDisplayCellAtLocation(
    Escher::HighlightCell * cell,
    int column,
    int row) {
  if (row == 0 && column == 0) {
    return;  // Top left
  }

  // Headers
  if (row == 0 || column == 0) {
    Escher::EvenOddBufferTextCell * myCell = static_cast<Escher::EvenOddBufferTextCell *>(cell);
    constexpr int bufferSize = k_headerTranslationBuffer;
    char txt[bufferSize];
    strlcpy(txt, I18n::translate(m_headerPrefix), bufferSize);
    int length = strlen(txt);
    char digit;
    if (row == 0) {
      assert(column - 1 <= '9' - '1');
      digit = '1' + (column - 1);
    } else {
      assert(row - 1 <= 'Z' - 'A');
      digit = 'A' + (row - 1);
    }

    txt[length] = digit;
    assert(length + 1 < bufferSize);
    txt[length + 1] = 0;
    myCell->setText(txt);
    KDColor textColor = KDColorBlack;
    if ((row == 0 && column == numberOfColumns() - 1) ||
        (column == 0 && row == numberOfRows() - 1)) {
      textColor = Palette::GrayDark;
    }
    myCell->setTextColor(textColor);
  }

  else {
    m_innerDataSource->willDisplayCellAtLocation(cell, column - 1, row - 1);
  }
}

void Probability::HomogeneityTableDataSource::tableViewDidChangeSelection(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  // Prevent top left selection
  if (t->selectedRow() == 0 && t->selectedColumn() == 0) {
    t->selectRow(previousSelectedCellY);
    t->selectColumn(previousSelectedCellX);
  }
}
