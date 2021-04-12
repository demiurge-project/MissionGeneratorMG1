#include "Box.h"

Box::Box() {
  m_fWidth = 0.026; // Fixed and to be checked with available wood
  m_fHeight = 0.07; // Fixed and to be checked with available wood
}

/****************************************/
/****************************************/

Box::Box(argos::CVector2 c_position, float f_orientation, float f_length) {
  m_fWidth = 0.026; // Fixed and to be checked with available wood
  m_fHeight = 0.07; // Fixed and to be checked with available wood
  m_cPosition = c_position;
  m_cOrientation = argos::CDegrees(f_orientation);
  m_fLength = f_length;
}

/****************************************/
/****************************************/

Box::~Box() {}

/****************************************/
/****************************************/

std::string Box::GetARGoSDescription() {
  std::ostringstream cStream;
  cStream << "<box ";
  cStream << "id=\"" << m_sType << "_" << m_unIndex << "\" ";
  cStream << "size=\"" << m_fWidth << "," << m_fLength << "," << m_fHeight << "\" ";
  cStream << "movable=\"false\"> ";
  cStream << "<body position=\"" << m_cPosition.GetX() << "," << m_cPosition.GetY() << ",0\" ";
  cStream << "orientation=\"" << m_cOrientation.GetValue() << ",0,0\" />";
  cStream << "</box>";
  return cStream.str();
}

/****************************************/
/****************************************/

void Box::SetType(std::string s_type) {
  m_sType = s_type;
}

/****************************************/
/****************************************/

void Box::SetIndex(unsigned int un_index) {
  m_unIndex = un_index;
}

/****************************************/
/****************************************/

void Box::SetLength(float f_length) {
  m_fLength = f_length;
}

/****************************************/
/****************************************/

void Box::SetWidth(float f_width) {
  m_fWidth = f_width;
}

/****************************************/
/****************************************/

void Box::SetHeight(float f_height) {
  m_fHeight = f_height;
}

/****************************************/
/****************************************/

void Box::SetPosition(argos::CVector2 c_position) {
  m_cPosition = c_position;
}

/****************************************/
/****************************************/

void Box::SetOrientation(float f_orientation) {
  m_cOrientation = argos::CDegrees(f_orientation);
}

/****************************************/
/****************************************/

std::string Box::GetType() {
  return m_sType;
}

/****************************************/
/****************************************/

unsigned int Box::GetIndex() {
  return m_unIndex;
}


/****************************************/
/****************************************/
float Box::GetLength() {
  return m_fLength;
}

/****************************************/
/****************************************/

float Box::GetHeight() {
  return m_fHeight;
}

/****************************************/
/****************************************/

float Box::GetWidth() {
  return m_fWidth;
}

/****************************************/
/****************************************/

argos::CVector2 Box::GetPosition() {
  return m_cPosition;
}

/****************************************/
/****************************************/

float Box::GetOrientation() {
  return m_cOrientation.GetValue();
}

/****************************************/
/****************************************/

std::pair<argos::CVector2, argos::CVector2> Box::GetExtremityPoints() {
  argos::CVector2 v = argos::CVector2(argos::Cos(argos::ToRadians(argos::CDegrees(90) + m_cOrientation)), argos::Sin(argos::ToRadians(argos::CDegrees(90) + m_cOrientation)));
  v = v * m_fLength/2;

  return std::make_pair(m_cPosition + v, m_cPosition - v);
}
