#ifndef PATCH
#define PATCH

#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/utility/datatypes/color.h>

class Patch {

  public:

    Patch();

    Patch(argos::CVector2 c_position, std::string str_shape, float f_size);

    virtual ~Patch();

    std::string GetLowLevelDescription();

    void SetIndex(unsigned int un_index);

    unsigned int GetIndex();

    void SetShape(std::string str_shape);

    std::string GetShape();

    void SetSize(float f_size);

    float GetSize();

    void SetColor(argos::CColor c_color);

    void SetColor(std::string str_color);

    argos::CColor GetColor();

    void SetPosition(argos::CVector2 c_position);

    argos::CVector2 GetPosition();

    bool IsOnPatch(argos::CVector2 c_point);

    std::vector<std::string> GetPossibleShapes();

    /*
     * List of possible square sizes (in this case, length of sides)
     */
    std::vector<float> GetPossibleSquareSizes();

    /*
     * List of possible circle sizes (in this case, length of radius)
     */
    std::vector<float> GetPossibleCircleSizes();

  private:

    unsigned int m_unIndex;

    std::string m_strShape;

    float m_fSize;

    argos::CColor m_cColor;

    argos::CVector2 m_cPosition;

    std::vector<float> m_vecPossibleSquareSizes;

    std::vector<float> m_vecPossibleCircleSizes;

    std::vector<std::string> m_vecPossibleShapes;

    std::vector<argos::CColor> m_vecPossibleColors;


};

#endif
