#include "Light.h"


Light::Light() {
  m_cPosition = argos::CVector3(0,0,-0.4);
}

/****************************************/
/****************************************/

Light::~Light() {}

/****************************************/
/****************************************/

std::string Light::GetARGoSDescription() {
  std::ostringstream cStream;
  cStream << "<light ";
  cStream << "id=\"light_0\" ";
  cStream << "position=\"" << m_cPosition.GetX() << "," << m_cPosition.GetY() << "," <<  m_cPosition.GetZ() << "\" ";
  cStream << "orientation=\"0,0,0\" ";
  cStream << "color=\"yellow\" ";
  cStream << "intensity=\"" << m_unIntensity << "\" ";
  cStream << "medium=\"leds\"/>";
  return cStream.str();
}

/****************************************/
/****************************************/

void Light::SetStatus(std::string str_status) {
  m_strStatus = str_status;
  if (m_strStatus == "'on'") {
    m_unIntensity = 5;
  } else {
    m_unIntensity = 0;
  }
}

/****************************************/
/****************************************/

std::string Light::GetStatus() {
  return m_strStatus;
}

/****************************************/
/****************************************/

unsigned int Light::GetIntensity() {
  return m_unIntensity;
}

/****************************************/
/****************************************/

void Light::SetPosition(argos::CVector3 c_position) {
  m_cPosition = c_position;
}

/****************************************/
/****************************************/

argos::CVector3 Light::GetPosition() {
    return m_cPosition;
}
