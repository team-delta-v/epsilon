#include "elements_data_base.h"
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/vertical_offset_layout.h>
#include <assert.h>

using namespace Poincare;

namespace Periodic {

Layout ElementsDataBase::ElectronConfiguration(AtomicNumber z) {
  constexpr int k_nMax = 7;
  constexpr int k_lMax = 4;
  constexpr char k_lSymbols[k_lMax] = { 's', 'p', 'd', 'f' };
  constexpr AtomicNumber k_nobles[] = { 10, 18, 36, 54, 86 };
  constexpr size_t k_numberOfNobles = sizeof(k_nobles) / sizeof(k_nobles[0]);

  int conf[k_nMax * k_lMax];
  for (int i = 0; i < k_nMax * k_lMax; i++) {
    conf[i] = 0;
  }

  int previousNoble = -1;
  for (int g = k_numberOfNobles - 1; g >= 0; g--) {
    if (k_nobles[g] < z) {
      previousNoble = g;
      break;
    }
  }

  HorizontalLayout res = HorizontalLayout::Builder();
  int electrons = z;
  int n = 0, l = 0;
  if (previousNoble >= 0) {
    electrons -= k_nobles[previousNoble];
    l = 0;
    n = previousNoble + 2;
    res.addOrMergeChildAtIndex(HorizontalLayout::Builder(CodePointLayout::Builder('['), LayoutHelper::String(Symbol(k_nobles[previousNoble])) ,CodePointLayout::Builder(']')), 0, false);
  }
  while (electrons > 0) {
    int index = n * k_lMax + l;
    int maxPop = 2 * (2 * l + 1);
    int pop = maxPop > electrons ? electrons : maxPop;
    electrons -= pop;
    conf[index] = pop;
    /* Subshells are filled in order of increasing n+l then increasing n. */
    if (l > 0) {
      l--;
      n++;
    } else {
      l = (n + 1) / 2;
      n += 1 - l;
    }
  }

  /* Subshells are displayed in order of increasing n, then increasing l. */
  for (n = 0; n < k_nMax; n++) {
    for (l = 0; l < k_lMax; l++) {
      int index = n * k_lMax + l;
      if (l > n || conf[index] == 0) {
        continue;
      }
      Layout term = HorizontalLayout::Builder(Integer(n + 1).createLayout(), CodePointLayout::Builder(k_lSymbols[l]), VerticalOffsetLayout::Builder(Integer(conf[index]).createLayout(), VerticalOffsetLayoutNode::Position::Superscript));
      res.addOrMergeChildAtIndex(term, res.numberOfChildren(), false);
    }
  }

  return std::move(res);
}

/* Shortucts some types and constants to make the table slightly readable. */
static constexpr double k_unknown = ElementData::k_unknown;
static constexpr double k_AUnknown = ElementData::k_AUnknown;
static constexpr double k_affinityUnstable = ElementData::k_affinityUnstable;
typedef ElementData::Group Group;
typedef ElementData::PhysicalState PhysicalState;
typedef ElementData::Block Block;

constexpr static ElementData k_elementsData[ElementsDataBase::k_numberOfElements] = {
  ElementData("H", I18n::Message::HName, 1, 1.008, 2.2, 32, -259.16, -252.879, 0.000088, 72.7700191, 1312.04602, Group::Nonmetal, PhysicalState::Gas, Block::S),
  ElementData("He", I18n::Message::HeName, 4, 4.003, k_unknown, 37, k_unknown, -268.928, 0.000176, k_affinityUnstable, 2372.323237, Group::NobleGas, PhysicalState::Gas, Block::S),
  ElementData("Li", I18n::Message::LiName, 7, 6.94, 0.98, 130, 180.5, 1342, 0.534, 59.63265756, 520.2199175, Group::Alkali, PhysicalState::Solid, Block::S),
  ElementData("Be", I18n::Message::BeName, 9, 9.012, 1.57, 99, 1287, 2468, 1.8477, k_affinityUnstable, 899.5037233, Group::AlkalineEarth, PhysicalState::Solid, Block::S),
  ElementData("B", I18n::Message::BName, 11, 10.81, 2.04, 84, 2077, 4000, 2.34, 26.98916408, 800.6352125, Group::Metalloid, PhysicalState::Solid, Block::P),
  ElementData("C", I18n::Message::CName, 12, 12.011, 2.55, 75, 3642, 3642, 3.513, 121.7759597, 1086.453686, Group::Nonmetal, PhysicalState::Solid, Block::P),
  ElementData("N", I18n::Message::NName, 14, 14.007, 3.04, 71, -210, -195.795, 0.001165, k_affinityUnstable, 1402.327337, Group::Nonmetal, PhysicalState::Gas, Block::P),
  ElementData("O", I18n::Message::OName, 16, 15.999, 3.44, 64, -218.79, -182.962, 0.001331, 140.9760084, 1313.946781, Group::Nonmetal, PhysicalState::Gas, Block::P),
  ElementData("F", I18n::Message::FName, 19, 18.998, 3.98, 60, -219.67, -188.11, 0.001667, 328.1648877, 1681.04449, Group::Halogen, PhysicalState::Gas, Block::P),
  ElementData("Ne", I18n::Message::NeName, 20, 20.18, k_unknown, 62, -248.59, -246.046, 0.0008999, 0, 2080.657754, Group::NobleGas, PhysicalState::Gas, Block::P),
  ElementData("Na", I18n::Message::NaName, 23, 22.99, 0.93, 160, 97.794, 882.94, 0.968, 52.86681724, 495.8477248, Group::Alkali, PhysicalState::Solid, Block::S),
  ElementData("Mg", I18n::Message::MgName, 24, 24.305, 1.31, 140, 650, 1090, 1.738, k_affinityUnstable, 737.7460788, Group::AlkalineEarth, PhysicalState::Solid, Block::S),
  ElementData("Al", I18n::Message::AlName, 27, 26.982, 1.61, 124, 660.323, 2519, 2.7, 41.76174247, 577.541848, Group::PostTransitionMetal, PhysicalState::Solid, Block::P),
  ElementData("Si", I18n::Message::SiName, 28, 28.085, 1.9, 114, 1414, 3265, 2.33, 134.0683925, 786.5194097, Group::Metalloid, PhysicalState::Solid, Block::P),
  ElementData("P", I18n::Message::PName, 31, 30.974, 2.19, 109, 44.15, 280.5, 1.823, 72.03661775, 1011.81264, Group::Nonmetal, PhysicalState::Solid, Block::P),
  ElementData("S", I18n::Message::SName, 32, 32.06, 2.58, 104, 115.21, 444.61, 1.92, 200.4100538, 999.5879491, Group::Nonmetal, PhysicalState::Solid, Block::P),
  ElementData("Cl", I18n::Message::ClName, 35, 35.45, 3.16, 100, -101.5, -34.04, 0.00298, 348.5749395, 1251.183078, Group::Halogen, PhysicalState::Gas, Block::P),
  ElementData("Ar", I18n::Message::ArName, 40, 39.948, k_unknown, 101, -139.34, -185.848, 0.0017824, 0, 1520.570101, Group::NobleGas, PhysicalState::Gas, Block::P),
  ElementData("K", I18n::Message::KName, 39, 39.098, 0.82, 200, 63.5, 759, 0.89, 48.38449506, 418.8138427, Group::Alkali, PhysicalState::Solid, Block::S),
  ElementData("Ca", I18n::Message::CaName, 40, 40.078, 1, 174, 842, 1484, 1.55, 2.368714686, 589.8340782, Group::AlkalineEarth, PhysicalState::Solid, Block::S),
  ElementData("Sc", I18n::Message::ScName, 45, 44.956, 1.36, 159, 1541, 2836, 2.895, 18.13924077, 633.0884486, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Ti", I18n::Message::TiName, 48, 47.867, 1.54, 148, 1670, 3287, 4.506, 7.622340538, 658.8114358, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("V", I18n::Message::VName, 51, 50.942, 1.63, 144, 1910, 3407, 6.11, 50.65479472, 650.9092878, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Cr", I18n::Message::CrName, 52, 51.996, 1.66, 130, 1907, 2671, 7.15, 64.2592253, 652.8679399, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Mn", I18n::Message::MnName, 55, 54.938, 1.55, 129, 1246, 2061, 7.21, k_affinityUnstable, 717.2718932, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Fe", I18n::Message::FeName, 56, 55.845, 1.83, 124, 1538, 2861, 7.86, 14.56928381, 762.4656186, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Co", I18n::Message::CoName, 59, 58.933, 1.88, 118, 1495, 2927, 8.9, 63.87328401, 760.4008327, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Ni", I18n::Message::NiName, 58, 58.693, 1.91, 117, 1455, 2913, 8.908, 111.5370337, 737.1382213, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Cu", I18n::Message::CuName, 63, 63.546, 1.9, 122, 1084.62, 2560, 8.96, 119.1593742, 745.4842017, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Zn", I18n::Message::ZnName, 64, 65.38, 1.65, 120, 419.527, 907, 7.14, k_affinityUnstable, 906.4024239, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Ga", I18n::Message::GaName, 69, 69.723, 1.81, 123, 29.7666, 2229, 5.904, 41.48868901, 578.8443999, Group::PostTransitionMetal, PhysicalState::Solid, Block::P),
  ElementData("Ge", I18n::Message::GeName, 74, 72.63, 2.01, 120, 938.25, 2833, 5.323, 118.9386158, 762.1761626, Group::Metalloid, PhysicalState::Solid, Block::P),
  ElementData("As", I18n::Message::AsName, 75, 74.922, 2.01, 120, 817, 616, 5.727, 77.57419991, 944.4562354, Group::Metalloid, PhysicalState::Solid, Block::P),
  ElementData("Se", I18n::Message::SeName, 80, 78.971, 2.55, 118, 220.8, 685, 4.81, 194.9649982, 940.9634667, Group::Nonmetal, PhysicalState::Solid, Block::P),
  ElementData("Br", I18n::Message::BrName, 79, 79.904, 2.96, 117, -7.2, 58.8, 3.1022, 324.5368948, 1139.858312, Group::Halogen, PhysicalState::Liquid, Block::P),
  ElementData("Kr", I18n::Message::KrName, 84, 83.798, k_unknown, 116, -157.37, -153.415, 0.0037493, 0, 1350.755932, Group::NobleGas, PhysicalState::Gas, Block::P),
  ElementData("Rb", I18n::Message::RbName, 85, 85.468, 0.82, 215, 39.3, 688, 1.532, 46.88414828, 403.0288438, Group::Alkali, PhysicalState::Solid, Block::S),
  ElementData("Sr", I18n::Message::SrName, 88, 87.62, 0.95, 190, 777, 1377, 2.64, 4.631295517, 549.4742675, Group::AlkalineEarth, PhysicalState::Solid, Block::S),
  ElementData("Y", I18n::Message::YName, 89, 88.906, 1.22, 176, 1522, 3345, 4.472, 29.62099424, 599.8782004, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Zr", I18n::Message::ZrName, 90, 91.224, 1.33, 164, 1854, 4406, 6.52, 41.10274771, 640.073986, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Nb", I18n::Message::NbName, 93, 92.906, 1.6, 156, 2477, 4741, 8.57, 88.38055612, 652.1346515, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Mo", I18n::Message::MoName, 98, 95.95, 2.16, 146, 2622, 4639, 10.28, 72.17102181, 684.3125068, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Tc", I18n::Message::TcName, 98, k_unknown, 2.1, 138, 2157, 4262, 11, 53.0669278, 702.4131534, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Ru", I18n::Message::RuName, 102, 101.07, 2.2, 121, 2333, 4147, 12.45, 101.3095894, 710.1802219, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Rh", I18n::Message::RhName, 103, 102.91, 2.28, 134, 1963, 3695, 12.41, 109.7038126, 719.6743777, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Pd", I18n::Message::PdName, 106, 106.42, 2.2, 130, 1554.8, 2963, 12.023, 54.22475168, 804.3884916, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Ag", I18n::Message::AgName, 107, 107.87, 1.93, 136, 961.78, 2162, 10.49, 125.6238909, 730.9921062, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Cd", I18n::Message::CdName, 114, 112.41, 1.69, 140, 321.78, 767, 8.65, k_affinityUnstable, 867.7697004, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("In", I18n::Message::InName, 115, 114.82, 1.78, 142, 156.5985, 2027, 7.31, 28.94559698, 558.3026746, Group::PostTransitionMetal, PhysicalState::Solid, Block::P),
  ElementData("Sn", I18n::Message::SnName, 120, 118.71, 1.96, 140, 231.928, 2586, 7.265, 107.298144, 708.5785656, Group::PostTransitionMetal, PhysicalState::Solid, Block::P),
  ElementData("Sb", I18n::Message::SbName, 121, 121.76, 2.05, 140, 630, 1587, 6.697, 100.9236481, 830.5842568, Group::Metalloid, PhysicalState::Solid, Block::P),
  ElementData("Te", I18n::Message::TeName, 130, 127.6, 2.1, 137, 449.51, 988, 6.24, 190.160608, 869.2941685, Group::Metalloid, PhysicalState::Solid, Block::P),
  ElementData("I", I18n::Message::IName, 127, 126.9, 2.66, 136, 113.7, 184.4, 4.63, 295.1521545, 1008.397059, Group::Halogen, PhysicalState::Solid, Block::P),
  ElementData("Xe", I18n::Message::XeName, 132, 131.29, 2.6, 136, -111.75, -108.099, 0.005761, 0, 1170.347674, Group::NobleGas, PhysicalState::Gas, Block::P),
  ElementData("Cs", I18n::Message::CsName, 133, 132.91, 0.79, 238, 28.5, 671, 1.8785, 45.50498707, 375.7042003, Group::Alkali, PhysicalState::Solid, Block::S),
  ElementData("Ba", I18n::Message::BaName, 138, 137.33, 0.89, 206, 727, 1845, 3.51, 13.95370745, 502.8525593, Group::AlkalineEarth, PhysicalState::Solid, Block::S),
  ElementData("La", I18n::Message::LaName, 139, 138.91, 1.1, 194, 920, 3464, 6.162, 45.34810194, 538.0889993, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Ce", I18n::Message::CeName, 140, 140.12, 1.12, 184, 799, 3443, 6.773, 62.71546013, 534.40326, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Pr", I18n::Message::PrName, 141, 140.91, 1.13, 190, 931, 3520, 6.475, 92.81888099, 528.0641743, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Nd", I18n::Message::NdName, 142, 144.24, 1.14, 188, 1016, 3074, 7.01, k_unknown, 533.0814111, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Pm", I18n::Message::PmName, 145, k_unknown, k_unknown, 186, 1042, 3000, 7.22, k_unknown, 538.5810745, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Sm", I18n::Message::SmName, 152, 150.36, 1.17, 185, 1072, 1794, 7.52, k_unknown, 544.5342189, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Eu", I18n::Message::EuName, 153, 151.96, k_unknown, 183, 822, 1529, 5.244, 83.36331931, 547.1103771, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Gd", I18n::Message::GdName, 158, 157.25, 1.2, 182, 1313, 3273, 7.9, k_unknown, 593.3654411, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Tb", I18n::Message::TbName, 159, 158.93, k_unknown, 181, 1359, 3230, 8.23, k_unknown, 565.7706386, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Dy", I18n::Message::DyName, 164, 162.5, 1.22, 180, 1412, 2567, 8.54, k_unknown, 573.0166864, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Ho", I18n::Message::HoName, 165, 164.93, 1.23, 179, 1472, 2700, 8.79, k_unknown, 580.9863741, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Er", I18n::Message::ErName, 166, 167.26, 1.24, 177, 1529, 2868, 9.066, k_unknown, 589.3034089, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Tm", I18n::Message::TmName, 169, 168.93, 1.25, 177, 1545, 1950, 9.32, 99.28339765, 596.6941847, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Yb", I18n::Message::YbName, 174, 173.05, k_unknown, 178, 824, 1196, 6.9, -1.929706465, 603.4385088, Group::Lanthanide, PhysicalState::Solid, Block::F),
  ElementData("Lu", I18n::Message::LuName, 175, 174.97, 1, 174, 1663, 3402, 9.841, 32.80500991, 523.5197155, Group::Lanthanide, PhysicalState::Solid, Block::D),
  ElementData("Hf", I18n::Message::HfName, 180, 178.49, 1.3, 164, 2233, 4600, 13.31, 1.350794526, 658.5219799, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Ta", I18n::Message::TaName, 181, 180.95, 1.5, 158, 3017, 5455, 16.69, 31.06827409, 728.4255966, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("W", I18n::Message::WName, 184, 183.84, 1.7, 150, 3414, 5555, 19.25, 78.75710997, 758.7605822, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Re", I18n::Message::ReName, 187, 186.21, 1.9, 141, 3185, 5590, 21.02, 14.47279849, 755.8177798, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Os", I18n::Message::OsName, 192, 190.23, 2.2, 136, 3033, 5008, 22.61, 106.1338556, 814.1624548, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Ir", I18n::Message::IrName, 193, 192.22, 2.2, 132, 2446, 4428, 22.65, 150.8837485, 865.1838938, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Pt", I18n::Message::PtName, 195, 195.08, 2.2, 130, 1768.2, 3825, 21.09, 205.3207679, 864.3927141, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Au", I18n::Message::AuName, 197, 196.97, 2.4, 130, 1064.18, 2836, 19.3, 222.7489119, 890.1253498, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Hg", I18n::Message::HgName, 202, 200.59, 1.9, 132, -38.829, 356.619, 13.534, k_affinityUnstable, 1007.065562, Group::TransitionMetal, PhysicalState::Liquid, Block::D),
  ElementData("Tl", I18n::Message::TlName, 205, 204.38, 1.8, 144, 304, 1473, 11.85, 36.37496687, 589.3516516, Group::PostTransitionMetal, PhysicalState::Solid, Block::P),
  ElementData("Pb", I18n::Message::PbName, 208, 207.2, 1.8, 145, 327.462, 1749, 11.34, 35.12065767, 715.6026971, Group::PostTransitionMetal, PhysicalState::Solid, Block::P),
  ElementData("Bi", I18n::Message::BiName, 209, 208.98, 1.9, 150, 271.462, 1564, 9.78, 90.92410221, 702.9438227, Group::PostTransitionMetal, PhysicalState::Solid, Block::P),
  ElementData("Po", I18n::Message::PoName, 209, k_unknown, 2, 142, 254, 962, 9.196, 183.3221142, 811.82751, Group::Metalloid, PhysicalState::Solid, Block::P),
  ElementData("At", I18n::Message::AtName, 210, k_unknown, 2.2, 148, 302, k_unknown, k_unknown, 270.1589052, k_unknown, Group::Metalloid, PhysicalState::Solid, Block::P),
  ElementData("Rn", I18n::Message::RnName, 222, k_unknown, k_unknown, 146, -71, -61.00973, 0.00973, k_affinityUnstable, 1037.072497, Group::NobleGas, PhysicalState::Gas, Block::P),
  ElementData("Fr", I18n::Message::FrName, 223, k_unknown, 0.7, 242, 21, k_unknown, k_unknown, 44.3832487, 392.9557761, Group::Alkali, PhysicalState::Solid, Block::S),
  ElementData("Ra", I18n::Message::RaName, 226, k_unknown, 0.9, 211, 696, k_unknown, 5.5, k_unknown, 509.2881304, Group::AlkalineEarth, PhysicalState::Solid, Block::S),
  ElementData("Ac", I18n::Message::AcName, 227, k_unknown, 1.1, 201, 1050, 3200, 10.07, 33.76986314, 519.1585789, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("Th", I18n::Message::ThName, 232, 232.04, 1.3, 190, 1750, 4785, 11.7, k_unknown, 608.5039883, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("Pa", I18n::Message::PaName, 231, 231.04, 1.5, 184, 1572, k_unknown, 15.37, k_unknown, 568.2985541, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("U", I18n::Message::UName, 238, 238.03, 1.7, 183, 1135, 4131, 19.1, k_unknown, 597.6204438, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("Np", I18n::Message::NpName, 237, k_unknown, 1.3, 180, 644, 3902, 20.2, k_unknown, 604.54809, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("Pu", I18n::Message::PuName, 244, k_unknown, 1.3, 180, 640, 3228, 19.816, k_unknown, 581.420558, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("Am", I18n::Message::AmName, 243, k_unknown, 1.3, 173, 1176, 2011, 12, k_unknown, 576.3840242, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("Cm", I18n::Message::CmName, 247, k_unknown, 1.3, 168, 1345, k_unknown, 13.51, k_unknown, 578.0821658, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("Bk", I18n::Message::BkName, 247, k_unknown, 1.3, 168, 986, k_unknown, 14.78, k_unknown, 598.0063851, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("Cf", I18n::Message::CfName, 251, k_unknown, 1.3, 168, 900, k_unknown, k_unknown, k_unknown, 606.0918552, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("Es", I18n::Message::EsName, k_AUnknown, k_unknown, 1.3, 165, 860, k_unknown, 8.84, k_unknown, 614.3799445, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("Fm", I18n::Message::FmName, 257, k_unknown, 1.3, 167, 1527, k_unknown, k_unknown, k_unknown, 627.1546013, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("Md", I18n::Message::MdName, 258, k_unknown, 1.3, 173, 827, k_unknown, k_unknown, k_unknown, 634.8734271, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("No", I18n::Message::NoName, 259, k_unknown, 1.3, 176, 827, k_unknown, k_unknown, k_unknown, 641.6273998, Group::Actinide, PhysicalState::Solid, Block::F),
  ElementData("Lr", I18n::Message::LrName, 262, k_unknown, k_unknown, 161, 1627, k_unknown, k_unknown, k_unknown, 478.5672034, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Rf", I18n::Message::RfName, 261, k_unknown, k_unknown, 157, k_unknown, k_unknown, k_unknown, k_unknown, 578.9119396, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Db", I18n::Message::DbName, 262, k_unknown, k_unknown, 149, k_unknown, k_unknown, k_unknown, k_unknown, k_unknown, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Sg", I18n::Message::SgName, 263, k_unknown, k_unknown, 143, k_unknown, k_unknown, k_unknown, k_unknown, k_unknown, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Bh", I18n::Message::BhName, 264, k_unknown, k_unknown, 141, k_unknown, k_unknown, k_unknown, k_unknown, k_unknown, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Hs", I18n::Message::HsName, 265, k_unknown, k_unknown, 134, k_unknown, k_unknown, k_unknown, k_unknown, k_unknown, Group::TransitionMetal, PhysicalState::Solid, Block::D),
  ElementData("Mt", I18n::Message::MtName, 268, k_unknown, k_unknown, 129, k_unknown, k_unknown, k_unknown, k_unknown, k_unknown, Group::Unidentified, PhysicalState::Solid, Block::D),
  ElementData("Ds", I18n::Message::DsName, 271, k_unknown, k_unknown, 128, k_unknown, k_unknown, k_unknown, k_unknown, k_unknown, Group::Unidentified, PhysicalState::Solid, Block::D),
  ElementData("Rg", I18n::Message::RgName, 272, k_unknown, k_unknown, 121, k_unknown, k_unknown, k_unknown, k_unknown, k_unknown, Group::Unidentified, PhysicalState::Solid, Block::D),
  ElementData("Cn", I18n::Message::CnName, k_AUnknown, k_unknown, k_unknown, 122, k_unknown, k_unknown, k_unknown, k_unknown, k_unknown, Group::Unidentified, PhysicalState::Solid, Block::D),
  ElementData("Nh", I18n::Message::NhName, k_AUnknown, k_unknown, k_unknown, 136, k_unknown, k_unknown, k_unknown, k_unknown, k_unknown, Group::Unidentified, PhysicalState::Solid, Block::P),
  ElementData("Fl", I18n::Message::FlName, k_AUnknown, k_unknown, k_unknown, 143, k_unknown, k_unknown, k_unknown, k_affinityUnstable, k_unknown, Group::Unidentified, PhysicalState::Solid, Block::P),
  ElementData("Mc", I18n::Message::McName, k_AUnknown, k_unknown, k_unknown, 162, k_unknown, k_unknown, k_unknown, k_unknown, k_unknown, Group::Unidentified, PhysicalState::Solid, Block::P),
  ElementData("Lv", I18n::Message::LvName, k_AUnknown, k_unknown, k_unknown, 175, k_unknown, k_unknown, k_unknown, k_unknown, k_unknown, Group::Unidentified, PhysicalState::Solid, Block::P),
  ElementData("Ts", I18n::Message::TsName, k_AUnknown, k_unknown, k_unknown, 165, k_unknown, k_unknown, k_unknown, k_unknown, k_unknown, Group::Unidentified, PhysicalState::Solid, Block::P),
  ElementData("Og", I18n::Message::OgName, k_AUnknown, k_unknown, k_unknown, 157, k_unknown, k_unknown, k_unknown, 5.403178103, k_unknown, Group::Unidentified, PhysicalState::Solid, Block::P),
};

const ElementData * ElementsDataBase::DataForElement(AtomicNumber z) {
  assert(IsElement(z));
  return k_elementsData + (z - 1);
}

}
