#ifndef LIGHT
#define LIGHT

#include <argos3/core/utility/math/vector3.h>

class Light {

  public:

    Light();

    virtual ~Light();

    std::string GetARGoSDescription();

    void SetStatus(std::string str_status);

    std::string GetStatus();

    unsigned int GetIntensity();

    void SetPosition(argos::CVector3 c_position);

    argos::CVector3 GetPosition();

  private:

    std::string m_strStatus;

    unsigned int m_unIntensity;

    argos::CVector3 m_cPosition;

};

#endif
