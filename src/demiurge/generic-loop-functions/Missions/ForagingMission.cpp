#include "ForagingMission.h"

/****************************************/
/****************************************/

ForagingMission::ForagingMission() {
  m_strMissionType = "foraging";
  m_unNumberItemsCollected = 0;
  m_vecAllPatches = {};
  m_pcLight = NULL;
  m_bInitializedByARGoS = false;
}

/****************************************/
/****************************************/

ForagingMission::ForagingMission(const ForagingMission& orig) {
}

/****************************************/
/****************************************/

void ForagingMission::Init(TConfigurationNode& t_tree) {
  m_bInitializedByARGoS = true;

  GenericLoopFunctions::Init(t_tree);

  std::vector<std::string>::iterator it;

  for (it = m_vecMissionDescription.begin(); it != m_vecMissionDescription.end(); it++) {
    std::cout << *it << std::endl;
  }

  try {
    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--colnest");
    m_cColorNests.Set((*(it+1)).c_str());

    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--colfs");
    m_cColorFoodSources.Set((*(it+1)).c_str());

    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--np");
    UInt16 unNumberPatches = atoi((*(it+1)).c_str());
    std::vector<std::string>::iterator vecFirstPatch;
    std::vector<std::string>::iterator vecSecondPatch;
    for (size_t i = 0; i < unNumberPatches; i++) {
      Patch* pcCurrentPatch;
      std::ostringstream oss;
      oss << "--p" << i;
      vecFirstPatch = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), oss.str());
      if (i+1 < unNumberPatches) {
        std::ostringstream oss;
        oss << "--p" << i+1;
        vecSecondPatch = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), oss.str());
      } else {
        vecSecondPatch = m_vecMissionDescription.end();
      }
      std::vector<std::string> vecPatchConfig(vecFirstPatch, vecSecondPatch);
      pcCurrentPatch = HandlePatch(vecPatchConfig, i);
      m_vecAllPatches.push_back(pcCurrentPatch);
      if (pcCurrentPatch->GetColor() == m_cColorNests) {
        m_vecNests.push_back(pcCurrentPatch);
      } else {
        m_vecFoodSources.push_back(pcCurrentPatch);
      }
    }
  } catch (std::exception e) {
    LOGERR << "Error while parsing mission Foraging description: " << e.what() << std::endl;
  }

  MoveRobots();
}

/****************************************/
/****************************************/

ForagingMission::~ForagingMission() {
  if (m_bInitializedByARGoS) {
    for (size_t i = 0; i < m_vecNests.size(); i++) {
      delete m_vecNests[i];
    }
    for (size_t i = 0; i < m_vecFoodSources.size(); i++) {
      delete m_vecFoodSources[i];
    }
  }
}

/****************************************/
/****************************************/

void ForagingMission::Destroy() {}

/****************************************/
/****************************************/

argos::CColor ForagingMission::GetFloorColor(const argos::CVector2& c_position_on_plane) {
  return GenericLoopFunctions::GetFloorColor(c_position_on_plane);
}


/****************************************/
/****************************************/

void ForagingMission::Reset() {
  GenericLoopFunctions::Reset();
  m_mapFoodData.clear();
  m_unNumberItemsCollected = 0;
}

/****************************************/
/****************************************/

void ForagingMission::PostStep() {
  UInt32 unScoreTemp = m_unNumberItemsCollected;

  CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
  CVector2 cEpuckPosition(0,0);
  for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
    CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);

    std::string strRobotId = pcEpuck->GetId();
    cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                       pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

    // Check if robot on any of the food sources
    for (std::vector<Patch*>::iterator itFS = m_vecFoodSources.begin(); itFS != m_vecFoodSources.end(); ++itFS) {
      if ((*itFS)->IsOnPatch(cEpuckPosition)) {
        m_mapFoodData[strRobotId] = 1;
      }
    }

    // Check if robot on any of the nests
    for (std::vector<Patch*>::iterator itN = m_vecNests.begin(); itN != m_vecNests.end(); ++itN) {
      if ((*itN)->IsOnPatch(cEpuckPosition)) {
        std::map<std::string, UInt32>::iterator itFood = m_mapFoodData.find(strRobotId);
        if (itFood != m_mapFoodData.end()) {
          m_unNumberItemsCollected += itFood->second;
        }
        m_mapFoodData[strRobotId] = 0;
      }
    }
  }

  if (unScoreTemp != m_unNumberItemsCollected) {
     LOGERR << "Obj " << m_unNumberItemsCollected << std::endl;
  }
}

/****************************************/
/****************************************/

Real ForagingMission::GetScore() {
  return m_unNumberItemsCollected;
}

/****************************************/
/****************************************/

void ForagingMission::PostExperiment() {
  LOG << "Score " << GetScore() << std::endl;
}

/****************************************/
/****************************************/

void ForagingMission::Initialize() {
  GenericLoopFunctions::Initialize();

  std::map<std::string, argos::CAny>::iterator it;

  it = GetVariableFromDictionary("colorNest");
  std::string strColorNests = argos::any_cast<std::string>(it->second);
  if (strColorNests == "'white'") {
    m_cColorNests = argos::CColor::WHITE;
    m_cColorFoodSources = argos::CColor::BLACK;
  } else if (strColorNests == "'black'") {
    m_cColorNests = argos::CColor::BLACK;
    m_cColorFoodSources = argos::CColor::WHITE;
  } else {
    std::cout << "\033[1;31m" << "Error: unknown color nest \"" << strColorNests << "\" ." << "\033[0m" << std::endl;
    std::exit(1);
  }

  it = GetVariableFromDictionary("numberNests");
  unsigned int unNumberNests = stoi(argos::any_cast<std::string>(it->second));
  unsigned int iNest = 0;
  Patch* pcNewPatch;
  std::vector<Patch*> vecPossibleNests = m_pcArena->GetPossibleNestPatches();
  while (iNest < unNumberNests) {
    unsigned int unRandomIndex = rand() % vecPossibleNests.size();
    pcNewPatch = vecPossibleNests[unRandomIndex];
    pcNewPatch->SetColor(m_cColorNests);
    // Set index of patch
    pcNewPatch->SetIndex(m_vecAllPatches.size());

    // Add patch to list of patches if no overlap with existing ones
    if (!(m_pcArena->ArePatchesOverlapping(m_vecAllPatches, pcNewPatch))) {
       m_vecAllPatches.push_back(pcNewPatch);
       m_vecFoodSources.push_back(pcNewPatch);
       vecPossibleNests.erase(vecPossibleNests.begin() + unRandomIndex);
       iNest += 1;
     }
  }

  it = GetVariableFromDictionary("numberFoodSource");
  unsigned int unNumberFoodSources = stoi(argos::any_cast<std::string>(it->second));
  unsigned int iFoodSource = 0;
  std::vector<Patch*> vecPossibleFoodSources = m_pcArena->GetPossibleFoodSourcePatches();
  while (iFoodSource < unNumberFoodSources) {
    unsigned int unRandomIndex = rand() % vecPossibleFoodSources.size();
    pcNewPatch = vecPossibleFoodSources[unRandomIndex];
    pcNewPatch->SetColor(m_cColorFoodSources);
    // Set index of patch
    pcNewPatch->SetIndex(m_vecAllPatches.size());

    // Add patch to list of patches if no overlap with existing ones
    if (!(m_pcArena->ArePatchesOverlapping(m_vecAllPatches, pcNewPatch))) {
       m_vecAllPatches.push_back(pcNewPatch);
       m_vecFoodSources.push_back(pcNewPatch);
       vecPossibleFoodSources.erase(vecPossibleFoodSources.begin() + unRandomIndex);
       iFoodSource += 1;
     }
  }
}

/****************************************/
/****************************************/

std::string ForagingMission::GetLowLevelDescription() {
  std::string strGenericPart = GenericLoopFunctions::GetLowLevelDescription();
  std::ostringstream cStream;
  cStream << strGenericPart;
  cStream << "--colnest " << m_cColorNests << " ";
  cStream << "--colfs " << m_cColorFoodSources << " ";
  cStream << "--np " << m_vecAllPatches.size() << " ";
  for (size_t i = 0; i < m_vecAllPatches.size(); ++i) {
    cStream << m_vecAllPatches[i]->GetLowLevelDescription();
  }
  return cStream.str();
}

REGISTER_LOOP_FUNCTIONS(ForagingMission, "generic_foraging");
