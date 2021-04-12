#include "HexagonArena.h"


HexagonArena::HexagonArena() {
  m_vecObstacles = {};
  m_strArenaType = "hexagon";
}

/****************************************/
/****************************************/

HexagonArena::~HexagonArena() {}

/****************************************/
/****************************************/

void HexagonArena::Initialize() {
  float fCos30 = cos(M_PI / 6);
  float fSin30 = sin(M_PI / 6);
  float fInradius = (m_fSideLength / 2.0f * (fCos30 / fSin30));     // cos / sin = tan
  m_fCircumradius = (m_fSideLength)  * (1/fSin30) / 2;
  float fPosX = fSin30 * (fInradius + (m_fWallWidth/2)) ;
  float fPosY = fCos30 * (fInradius + (m_fWallWidth/2));

  // Generating walls
  CreateWall(0, argos::CVector2(fPosX, fPosY), 60, m_fSideLength);
  CreateWall(1, argos::CVector2(-fPosX, -fPosY), 60, m_fSideLength);
  CreateWall(2, argos::CVector2(fPosX, -fPosY), -60, m_fSideLength);
  CreateWall(3, argos::CVector2(-fPosX, fPosY), -60, m_fSideLength);
  CreateWall(4, argos::CVector2(fInradius + (m_fWallWidth/2), 0), 0, m_fSideLength);
  CreateWall(5, argos::CVector2(-fInradius - (m_fWallWidth/2), 0), 0, m_fSideLength);

  // Specifying embodied triangles
  float fCos60 = cos(M_PI / 3);
  float fSin60 = sin(M_PI / 3);
  fPosX = fSin60 * (m_fCircumradius + (m_fWallWidth/2)) ;
  fPosY = fCos60 * (m_fCircumradius + (m_fWallWidth/2));
  m_vecEmbodiedTriangles.push_back(STriangle(argos::CVector2(0, 0), argos::CVector2(0, -m_fCircumradius), argos::CVector2(fPosX, -fPosY)));
  m_vecEmbodiedTriangles.push_back(STriangle(argos::CVector2(0, 0), argos::CVector2(0, -m_fCircumradius), argos::CVector2(-fPosX, -fPosY)));
  m_vecEmbodiedTriangles.push_back(STriangle(argos::CVector2(0, 0), argos::CVector2(fPosX, fPosY), argos::CVector2(fPosX, -fPosY)));
  m_vecEmbodiedTriangles.push_back(STriangle(argos::CVector2(0, 0), argos::CVector2(-fPosX, fPosY), argos::CVector2(-fPosX, -fPosY)));
  m_vecEmbodiedTriangles.push_back(STriangle(argos::CVector2(0, 0), argos::CVector2(0, m_fCircumradius), argos::CVector2(fPosX, fPosY)));
  m_vecEmbodiedTriangles.push_back(STriangle(argos::CVector2(0, 0), argos::CVector2(0, m_fCircumradius), argos::CVector2(-fPosX, fPosY)));


  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, 0), "Circle", 0.15));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, 0), "Circle", 0.3));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, m_fCircumradius), "Circle", 0.4));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, -m_fCircumradius), "Circle", 0.4));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, m_fCircumradius * (2/3.0f)), "Circle", 0.3));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, m_fCircumradius * (2/3.0f)), "Circle", 0.15));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, -m_fCircumradius * (2/3.0f)), "Circle", 0.3));
  m_vecPossibleNestPatches.push_back(new Patch(argos::CVector2(0, -m_fCircumradius * (2/3.0f)), "Circle", 0.15));

  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(0, 0), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(0, m_fCircumradius), "Circle", 0.40));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(0, -m_fCircumradius), "Circle", 0.40));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(0, m_fCircumradius * (2/3.0f)), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(0, -m_fCircumradius * (2/3.0f)), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(fInradius * (2/3.0f), 0), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(-fInradius * (2/3.0f), 0), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(fPosX * (2/3.0f), fPosY * (2/3.0f)), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(fPosX * (2/3.0f), -fPosY * (2/3.0f)), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(-fPosX * (2/3.0f), fPosY * (2/3.0f)), "Circle", 0.15));
  m_vecPossibleFoodSourcePatches.push_back(new Patch(argos::CVector2(-fPosX * (2/3.0f), -fPosY * (2/3.0f)), "Circle", 0.15));

  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(0, 0), "Circle", 0.3));
  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(0, m_fCircumradius * (2/3.0f)), "Circle", 0.3));
  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(0, -m_fCircumradius * (2/3.0f)), "Circle", 0.3));
  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(fInradius * (2/3.0f), 0), "Circle", 0.3));
  m_vecPossibleAggregationPatches.push_back(new Patch(argos::CVector2(-fInradius * (2/3.0f), 0), "Circle", 0.3));

  m_vecAllPossiblePatches.reserve(m_vecPossibleNestPatches.size() + m_vecPossibleFoodSourcePatches.size() + m_vecPossibleAggregationPatches.size());
  m_vecAllPossiblePatches.insert(m_vecAllPossiblePatches.end(), m_vecPossibleNestPatches.begin(), m_vecPossibleNestPatches.end());
  m_vecAllPossiblePatches.insert(m_vecAllPossiblePatches.end(), m_vecPossibleFoodSourcePatches.begin(), m_vecPossibleFoodSourcePatches.end());
  m_vecAllPossiblePatches.insert(m_vecAllPossiblePatches.end(), m_vecPossibleAggregationPatches.begin(), m_vecPossibleAggregationPatches.end());


  m_vecPossibleLightPositions.push_back(argos::CVector3(0, -m_fCircumradius - 0.2, 0.4));

  m_vecPossibleObstacles.push_back(new Box(argos::CVector2(0, fInradius * (2/3.0f) / 2.0f), 90, 0.35));
  m_vecPossibleObstacles.push_back(new Box(argos::CVector2(0, -fInradius * (2/3.0f) / 2.0f), 90, 0.35));
  m_vecPossibleObstacles.push_back(new Box(argos::CVector2(fInradius * (2/3.0f) / 2.0f, 0), 0, 0.35));
  m_vecPossibleObstacles.push_back(new Box(argos::CVector2(-fInradius * (2/3.0f) / 2.0f, 0), 0, 0.35));

}
