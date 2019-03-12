#ifndef GRAPH_CALCULATION_GRAPH_CONTROLLER_H
#define GRAPH_CALCULATION_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "banner_view.h"
#include "../../shared/simple_interactive_curve_view_controller.h"
#include "../../shared/function_banner_delegate.h"
#include "../cartesian_function_store.h"

namespace Graph {

class App;

class CalculationGraphController : public Shared::SimpleInteractiveCurveViewController, public Shared::FunctionBannerDelegate {
public:
  CalculationGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor, I18n::Message defaultMessage);
  void viewWillAppear() override;
  void setRecord(Ion::Storage::Record record);
protected:
  constexpr static float k_cursorTopMarginRatio = 0.07f;   // (cursorHeight/2)/graphViewHeight
  constexpr static float k_cursorBottomMarginRatio = 0.15f; // (cursorHeight/2+bannerHeigh)/graphViewHeight
  BannerView * bannerView() override { return m_bannerView; }
  void reloadBannerView() override;
  Poincare::Expression::Coordinate2D computeNewPointOfInteresetFromAbscissa(double start, int direction);
  CartesianFunctionStore * functionStore() const;
  virtual Poincare::Expression::Coordinate2D computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) = 0;
  GraphView * m_graphView;
  BannerView * m_bannerView;
  Shared::InteractiveCurveViewRange * m_graphRange;
  Ion::Storage::Record m_record;
  MessageTextView m_defaultBannerView;
  bool m_isActive;
private:
  bool handleZoom(Ion::Events::Event event) override { return false; }
  bool handleLeftRightEvent(Ion::Events::Event event) override;
  bool handleEnter() override;
  bool moveCursorHorizontally(int direction) override;
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override { return m_graphRange; }
  Shared::CurveView * curveView() override { return m_graphView; }
};

}

#endif
