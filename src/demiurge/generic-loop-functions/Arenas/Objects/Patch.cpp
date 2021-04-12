#include "Patch.h"


Patch::Patch() {
  m_vecPossibleSquareSizes = {0.4,0.6};
  m_vecPossibleCircleSizes = {0.15,0.3};
  m_vecPossibleShapes = {"Circle"};
  m_vecPossibleColors = {argos::CColor::BLACK, argos::CColor::WHITE};
}

/****************************************/
/****************************************/

Patch::Patch(argos::CVector2 c_position, std::string str_shape, float f_size) {
  m_cPosition = c_position;
  m_strShape = str_shape;
  m_fSize = f_size;
  m_vecPossibleColors = {argos::CColor::BLACK, argos::CColor::WHITE};
}

/****************************************/
/****************************************/

Patch::~Patch() {}

/****************************************/
/****************************************/

std::string Patch::GetLowLevelDescription() {
  std::ostringstream cStream;
  cStream << "--p" << m_unIndex << " " << m_strShape << " ";
  cStream << "--ps" << m_unIndex << " " << m_fSize << " ";
  cStream << "--pcx" << m_unIndex << " " << m_cPosition.GetX() << " ";
  cStream << "--pcy" << m_unIndex << " " << m_cPosition.GetY() << " ";
  cStream << "--pc" << m_unIndex << " " << m_cColor << " ";
  return cStream.str();
}

/****************************************/
/****************************************/

void Patch::SetIndex(unsigned int un_index) {
  m_unIndex = un_index;
}

/****************************************/
/****************************************/

unsigned int Patch::GetIndex() {
  return m_unIndex;
}

/****************************************/
/****************************************/

void Patch::SetShape(std::string str_shape) {
  m_strShape = str_shape;
}

/****************************************/
/****************************************/

std::string Patch::GetShape() {
  return m_strShape;
}

/****************************************/
/****************************************/

void Patch::SetSize(float f_size) {
  m_fSize = f_size;
}

/****************************************/
/****************************************/

float Patch::GetSize() {
  return m_fSize;
}

/****************************************/
/****************************************/

void Patch::SetColor(argos::CColor c_color) {
  m_cColor = c_color;
}

/****************************************/
/****************************************/

void Patch::SetColor(std::string str_color) {
  m_cColor.Set(str_color);
}

/****************************************/
/****************************************/

argos::CColor Patch::GetColor() {
  return m_cColor;
}

/****************************************/
/****************************************/

void Patch::SetPosition(argos::CVector2 c_position) {
  m_cPosition = c_position;
}

/****************************************/
/****************************************/

argos::CVector2 Patch::GetPosition() {
    return m_cPosition;
}

/****************************************/
/****************************************/

bool Patch::IsOnPatch(argos::CVector2 c_point) {
  bool bOnPatch = false;
  if (m_strShape == "Square") {
    argos::CRange<argos::Real> cRangeX = argos::CRange<argos::Real>(m_cPosition.GetX() - m_fSize/2, m_cPosition.GetX() + m_fSize/2);
    argos::CRange<argos::Real> cRangeY = argos::CRange<argos::Real>(m_cPosition.GetY() - m_fSize/2, m_cPosition.GetY() + m_fSize/2);
    if (cRangeX.WithinMinBoundIncludedMaxBoundIncluded(c_point.GetX()) and cRangeY.WithinMinBoundIncludedMaxBoundIncluded(c_point.GetY())) {
      bOnPatch = true;
    }
  } else if (m_strShape == "Circle") {
    if (argos::Distance(m_cPosition, c_point) <= m_fSize) {
      bOnPatch = true;
    }
  }
  return bOnPatch;
}

/****************************************/
/****************************************/

std::vector<std::string> Patch::GetPossibleShapes() {
  return m_vecPossibleShapes;
}

/****************************************/
/****************************************/

std::vector<float> Patch::GetPossibleSquareSizes() {
  return m_vecPossibleSquareSizes;
}

/****************************************/
/****************************************/

std::vector<float> Patch::GetPossibleCircleSizes() {
  return m_vecPossibleCircleSizes;
}
