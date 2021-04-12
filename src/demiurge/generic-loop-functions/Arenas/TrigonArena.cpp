#include "TrigonArena.h"


TrigonArena::TrigonArena() {
  m_strArenaType = "trigon";
}

/****************************************/
/****************************************/

TrigonArena::~TrigonArena() {}

/****************************************/
/****************************************/

void TrigonArena::Initialize() {
  std::cout << "TRIGON Initialization" << std::endl;
  float fCos60 = cos(M_PI / 3);
  float fSin60 = sin(M_PI / 3);
  float fInradius = (m_fSideLength / 2.0f * (fCos60 / fSin60));     // cos / sin = tan
  float fPosX = fCos60 * (fInradius + (m_fWallWidth/2)) ;
  float fPosY = fSin60 * (fInradius + (m_fWallWidth/2));

  // Generating walls
  CreateWall(0, argos::CVector2(fPosX, fPosY), 60, m_fSideLength);
  CreateWall(1, argos::CVector2(fPosX, -fPosY), -60, m_fSideLength);
  CreateWall(2, argos::CVector2(-fInradius - (m_fWallWidth/2), 0), 0, m_fSideLength);

  // Specifying embodied triangles
  m_fCircumradius = (m_fSideLength)  * (1/fSin60) / 2;
  float fBottomCornerY = fSin60 * (m_fCircumradius + (m_fWallWidth/2));
  m_vecEmbodiedTriangles.push_back(STriangle(argos::CVector2(-fInradius, fBottomCornerY), argos::CVector2(-fInradius, -fBottomCornerY), argos::CVector2(m_fCircumradius, 0)));

  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, 0), "Circle", 0.15));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, 0), "Circle", 0.30));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(m_fCircumradius/2, 0), "Circle", 0.15));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(m_fCircumradius/2, 0), "Circle", 0.30));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(m_fCircumradius, 0), "Circle", 0.40));

  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(0, 0), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(m_fCircumradius/2, 0), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(m_fCircumradius, 0), "Circle", 0.40));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(-fInradius/2, fBottomCornerY/2), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(-fInradius/2, -fBottomCornerY/2), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(-fInradius, fBottomCornerY), "Circle", 0.40));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(-fInradius, -fBottomCornerY), "Circle", 0.40));

  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(0, 0), "Circle", 0.3));
  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(m_fCircumradius/2, 0), "Circle", 0.3));
  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(-fInradius/2, fBottomCornerY/2), "Circle", 0.3));
  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(-fInradius/2, -fBottomCornerY/2), "Circle", 0.3));

  m_vecAllPossiblePatches.reserve(m_vecPossibleNestPatches.size() + m_vecPossibleFoodSourcePatches.size() + m_vecPossibleAggregationPatches.size());
  m_vecAllPossiblePatches.insert(m_vecAllPossiblePatches.end(), m_vecPossibleNestPatches.begin(), m_vecPossibleNestPatches.end());
  m_vecAllPossiblePatches.insert(m_vecAllPossiblePatches.end(), m_vecPossibleFoodSourcePatches.begin(), m_vecPossibleFoodSourcePatches.end());
  m_vecAllPossiblePatches.insert(m_vecAllPossiblePatches.end(), m_vecPossibleAggregationPatches.begin(), m_vecPossibleAggregationPatches.end());

  m_vecPossibleLightPositions.push_back(argos::CVector3(-fInradius - 0.2, 0, 0.4));

  m_vecPossibleObstacles.push_back(new Box(argos::CVector2(-fInradius/4, -fBottomCornerY/4), 60, 0.25));
  m_vecPossibleObstacles.push_back(new Box(argos::CVector2(-fInradius/4, fBottomCornerY/4), -60, 0.25));
  m_vecPossibleObstacles.push_back(new Box(argos::CVector2(m_fCircumradius/4, 0), 0, 0.25));


}
