#include "TetragonArena.h"


TetragonArena::TetragonArena() {
  m_strArenaType = "tetragon";
}

/****************************************/
/****************************************/

TetragonArena::~TetragonArena() {}

/****************************************/
/****************************************/

void TetragonArena::Initialize() {
  float fInradius = m_fSideLength / 2.0f;
  float fCos45 = cos(M_PI / 4);
  float fSin45 = sin(M_PI / 4);
  float fPosX = fCos45 * (fInradius + m_fWallWidth/2);
  float fPosY = fSin45 * (fInradius + m_fWallWidth/2);

  // Generating walls
  CreateWall(0, argos::CVector2(fPosX, fPosY), 45, m_fSideLength);
  CreateWall(1, argos::CVector2(-fPosX, -fPosY), 45, m_fSideLength);
  CreateWall(2, argos::CVector2(fPosX, -fPosY), -45, m_fSideLength);
  CreateWall(3, argos::CVector2(-fPosX, fPosY), -45, m_fSideLength);

  // Specifying embodied triangles
  m_fCircumradius = m_fSideLength * std::sqrt(2) / 2;
  m_vecEmbodiedTriangles.push_back(STriangle(argos::CVector2(m_fCircumradius, 0), argos::CVector2(-m_fCircumradius, 0), argos::CVector2(0,m_fCircumradius)));
  m_vecEmbodiedTriangles.push_back(STriangle(argos::CVector2(m_fCircumradius, 0), argos::CVector2(-m_fCircumradius, 0), argos::CVector2(0,-m_fCircumradius)));


  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, 0), "Circle", 0.15));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, 0), "Circle", 0.30));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, m_fCircumradius), "Circle", 0.40));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, -m_fCircumradius), "Circle", 0.40));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, m_fCircumradius * (2/3.0f)), "Circle", 0.15));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, m_fCircumradius * (2/3.0f)), "Circle", 0.30));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, -m_fCircumradius * (2/3.0f)), "Circle", 0.15));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, -m_fCircumradius * (2/3.0f)), "Circle", 0.30));

  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(0, 0), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(0, m_fCircumradius), "Circle", 0.40));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(0, -m_fCircumradius), "Circle", 0.40));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(m_fCircumradius, 0), "Circle", 0.40));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(-m_fCircumradius, 0), "Circle", 0.40));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(0, m_fCircumradius * (2/3.0f)), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(0, -m_fCircumradius * (2/3.0f)), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(m_fCircumradius * (2/3.0f), 0), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(-m_fCircumradius * (2/3.0f), 0), "Circle", 0.15));

  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(0, 0), "Circle", 0.3));
  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(0, m_fCircumradius * (2/3.0f)), "Circle", 0.3));
  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(0, -m_fCircumradius * (2/3.0f)), "Circle", 0.3));
  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(m_fCircumradius * (2/3.0f), 0), "Circle", 0.3));
  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(-m_fCircumradius * (2/3.0f), 0), "Circle", 0.3));

  m_vecAllPossiblePatches.reserve(m_vecPossibleNestPatches.size() + m_vecPossibleFoodSourcePatches.size() + m_vecPossibleAggregationPatches.size());
  m_vecAllPossiblePatches.insert(m_vecAllPossiblePatches.end(), m_vecPossibleNestPatches.begin(), m_vecPossibleNestPatches.end());
  m_vecAllPossiblePatches.insert(m_vecAllPossiblePatches.end(), m_vecPossibleFoodSourcePatches.begin(), m_vecPossibleFoodSourcePatches.end());
  m_vecAllPossiblePatches.insert(m_vecAllPossiblePatches.end(), m_vecPossibleAggregationPatches.begin(), m_vecPossibleAggregationPatches.end());

  m_vecPossibleLightPositions.push_back(argos::CVector3(0, -1.61, 0.4));

  m_vecPossibleObstacles.push_back(new Box(argos::CVector2(0, m_fCircumradius * (2/3.0f) / 2.0f), 90, 0.45));
  m_vecPossibleObstacles.push_back(new Box(argos::CVector2(0, -m_fCircumradius * (2/3.0f) / 2.0f), 90, 0.45));
  m_vecPossibleObstacles.push_back(new Box(argos::CVector2(m_fCircumradius * (2/3.0f) / 2.0f, 0), 0, 0.45));
  m_vecPossibleObstacles.push_back(new Box(argos::CVector2(-m_fCircumradius * (2/3.0f) / 2.0f, 0), 0, 0.45));

}
