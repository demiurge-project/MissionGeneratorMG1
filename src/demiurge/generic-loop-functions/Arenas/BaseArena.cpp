#include "BaseArena.h"


BaseArena::~BaseArena() {
  for (size_t i = 0; i < m_vecWalls.size(); ++i) {
    delete m_vecWalls[i];
  }
  for (size_t i = 0; i < m_vecAllPossiblePatches.size(); ++i) {
    delete m_vecAllPossiblePatches[i];
  }
  for (size_t i = 0; i < m_vecPossibleObstacles.size(); i++) {
    delete m_vecPossibleObstacles[i];
  }
}

/****************************************/
/****************************************/

void BaseArena::SetSideLength(float f_side_length) {
  m_fSideLength = f_side_length;
  Initialize();
}

/****************************************/
/****************************************/

float BaseArena::GetCircumradius() {
  return m_fCircumradius;
}

/****************************************/
/****************************************/

std::string BaseArena::GetArenaType() {
  return m_strArenaType;
}

/****************************************/
/****************************************/

void BaseArena::CreateWall(unsigned short n_index, argos::CVector2 c_position, float f_orientation, float f_length) {
  Box* pcWall = new Box();
  pcWall->SetType("wall");
  pcWall->SetIndex(n_index);
  pcWall->SetLength(f_length);
  pcWall->SetWidth(m_fWallWidth);
  pcWall->SetHeight(0.055);
  pcWall->SetPosition(c_position);
  pcWall->SetOrientation(f_orientation);
  m_vecWalls.push_back(pcWall);
}

/****************************************/
/****************************************/

void BaseArena::InstantiateObstacles(unsigned short n_number_obstacles) {
  std::vector<Box*> possibleObstacles = m_vecPossibleObstacles;
  size_t i = 0;
  while (i < n_number_obstacles) {
    Box* pcNewObstacle;
    unsigned short unRandomIndex = rand() % possibleObstacles.size();
    pcNewObstacle = possibleObstacles[unRandomIndex];
    pcNewObstacle->SetType("obst");
    pcNewObstacle->SetIndex(m_vecObstacles.size());
    pcNewObstacle->GetExtremityPoints();
    m_vecObstacles.push_back(pcNewObstacle);
    possibleObstacles.erase(possibleObstacles.begin() + unRandomIndex);
    i += 1;
  }
}

/****************************************/
/****************************************/

std::string BaseArena::GetARGoSDescription() {
  std::ostringstream cStream;
  cStream << "<!-- Walls -->" << std::endl;
  for (size_t i = 0; i < m_vecWalls.size(); ++i) {
    cStream << m_vecWalls[i]->GetARGoSDescription() << "\n";
  }
  cStream << std::endl << "<!-- Obstacles -->" << std::endl;
  for (size_t i = 0; i < m_vecObstacles.size(); ++i) {
    cStream << m_vecObstacles[i]->GetARGoSDescription() << "\n";
  }
  return cStream.str();
}

/****************************************/
/****************************************/

std::vector<argos::CVector3> BaseArena::GetPossibleLightPositions() {
  return m_vecPossibleLightPositions;
}

/****************************************/
/****************************************/

std::vector<Patch*> BaseArena::GetAllPossiblePatches() {
  return m_vecAllPossiblePatches;
}

/****************************************/
/****************************************/

std::vector<Patch*> BaseArena::GetPossibleNestPatches() {
  return m_vecPossibleNestPatches;
}

/****************************************/
/****************************************/

std::vector<Patch*> BaseArena::GetPossibleFoodSourcePatches() {
  return m_vecPossibleFoodSourcePatches;
}

/****************************************/
/****************************************/

std::vector<Patch*> BaseArena::GetPossibleAggregationPatches() {
  return m_vecPossibleAggregationPatches;
}

/****************************************/
/****************************************/

bool BaseArena::ArePatchesOverlapping(std::vector<Patch*> vec_patches, Patch* pc_new_patch) {
  bool bOverlapping = false;
  for (size_t i = 0; i < vec_patches.size(); ++i) {
    float fMinimalDistanceToAvoidOverlap = ComputeMinimalDistanceToAvoidOverlap(vec_patches[i], pc_new_patch);
    if (!(bOverlapping) && (argos::Distance(vec_patches[i]->GetPosition(), pc_new_patch->GetPosition()) < fMinimalDistanceToAvoidOverlap)) {
      bOverlapping = true;
    }
  }
  return bOverlapping;
}

/****************************************/
/****************************************/

std::string BaseArena::GetLowLevelDescription() {
  std::ostringstream cStream;
  cStream << "--a " << m_strArenaType << " ";
  cStream << "--as " << m_fSideLength << " ";
  return cStream.str();
}

/****************************************/
/****************************************/

bool BaseArena::IsInArena(argos::CVector2 c_point) {
  bool bInArena = false;
  std::vector<STriangle>::iterator it;
  for (it = m_vecEmbodiedTriangles.begin(); it != m_vecEmbodiedTriangles.end(); ++it) {
    bInArena = bInArena || IsInTriangle((*it), c_point);
  }
  return bInArena;
}

/****************************************/
/****************************************/

bool BaseArena::IsInTriangle(STriangle s_triangle, argos::CVector2 c_point) {
  float fAreaTriangle = ComputeAreaTriangle(s_triangle);
  float fAreaABP = ComputeAreaTriangle(s_triangle.A, s_triangle.B, c_point);
  float fAreaBCP = ComputeAreaTriangle(s_triangle.B, s_triangle.C, c_point);
  float fAreaACP = ComputeAreaTriangle(s_triangle.A, s_triangle.C, c_point);

  if (argos::Abs(fAreaTriangle - (fAreaABP + fAreaACP + fAreaBCP)) < 0.0001) {
    return true;
  } else {
    return false;
  }
}

/****************************************/
/****************************************/

float BaseArena::ComputeAreaTriangle(STriangle s_triangle) {
  return ComputeAreaTriangle(s_triangle.A, s_triangle.B, s_triangle.C);
}

/****************************************/
/****************************************/

float BaseArena::ComputeAreaTriangle(argos::CVector2 c_A, argos::CVector2 c_B, argos::CVector2 c_C) {
  float fArea = argos::Abs(c_A.GetX()*(c_B.GetY() - c_C.GetY()) + c_B.GetX()*(c_C.GetY() - c_A.GetY()) + c_C.GetX()*(c_A.GetY() - c_B.GetY()))/2;
  return fArea;
}


/****************************************/
/****************************************/

float BaseArena::ComputeMinimalDistanceToAvoidOverlap(Patch* pc_patch1, Patch* pc_patch2) {
  float fMinimalDistance = 0.0f;
  if (pc_patch1->GetShape() == "Circle") {
    fMinimalDistance += pc_patch1->GetSize();
  } else if (pc_patch1->GetShape() == "Square") {
    fMinimalDistance += pc_patch1->GetSize()/2;
  }
  if (pc_patch2->GetShape() == "Circle") {
    fMinimalDistance += pc_patch2->GetSize();
  } else if (pc_patch2->GetShape() == "Square") {
    fMinimalDistance += pc_patch2->GetSize()/2;
  }

  return fMinimalDistance;
}
