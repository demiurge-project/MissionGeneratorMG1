#ifndef BOX
#define BOX

#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/utility/math/angles.h>

class Box {

  public:

    Box();

    Box(argos::CVector2 c_position, float f_orientation, float f_length);

    virtual ~Box();

    std::string GetARGoSDescription();

    /**
     * Redefine the "<<" operator for Box.
     */
    friend std::ostream& operator<<(std::ostream& c_os, const Box& c_box);

    void SetType(std::string s_type);

    void SetIndex(unsigned int un_index);

    void SetLength(float f_length);

    void SetWidth(float f_width);

    void SetHeight(float f_height);

    void SetPosition(argos::CVector2 c_position);

    void SetOrientation(float f_orientation);

    std::string GetType();

    unsigned int GetIndex();

    float GetLength();

    float GetHeight();

    float GetWidth();

    argos::CVector2 GetPosition();

    /*
     * Orientation of box, in degrees
     */
    float GetOrientation();

    std::pair<argos::CVector2, argos::CVector2> GetExtremityPoints();


  private:

    std::string m_sType;

    unsigned int m_unIndex;

    float m_fWidth;

    float m_fHeight;

    float m_fLength;

    argos::CVector2 m_cPosition;

    argos::CDegrees m_cOrientation;

};

#endif
