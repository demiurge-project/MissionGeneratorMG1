#include "HomingMission.h"

/****************************************/
/****************************************/

HomingMission::HomingMission() {
  m_strMissionType = "homing";
  m_strTypeObjectiveFunction = "";
  m_unRobotsAtHome = 0;
  m_unAnyTimeScore = 0;
  m_pcLight = NULL;
  m_bInitializedByARGoS = false;
}

/****************************************/
/****************************************/

HomingMission::HomingMission(const HomingMission& orig) {
}

/****************************************/
/****************************************/

void HomingMission::Init(TConfigurationNode& t_tree) {
  m_bInitializedByARGoS = true;

  GenericLoopFunctions::Init(t_tree);

  std::vector<std::string>::iterator it;

  try {
    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--colhome");
    m_cColorHome.Set((*(it+1)).c_str());

    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--coldistration");
    m_cColorDistractions.Set((*(it+1)).c_str());

    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--tof");
    m_strTypeObjectiveFunction = (*(it+1)).c_str();

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
      if (pcCurrentPatch->GetColor() == m_cColorHome) {
        m_pcHome = pcCurrentPatch;
      } else {
        m_vecDistractionPatches.push_back(pcCurrentPatch);
      }
    }
  } catch (std::exception e) {
    LOGERR << "Error while parsing mission description: " << e.what() << std::endl;
  }

  MoveRobots();
}

/****************************************/
/****************************************/

HomingMission::~HomingMission() {
  if (m_bInitializedByARGoS) {
    delete m_pcHome;
    for (size_t i = 0; i < m_vecDistractionPatches.size(); i++) {
      delete m_vecDistractionPatches[i];
    }
  }
}

/****************************************/
/****************************************/

void HomingMission::Destroy() {}

/****************************************/
/****************************************/

argos::CColor HomingMission::GetFloorColor(const argos::CVector2& c_position_on_plane) {
  return GenericLoopFunctions::GetFloorColor(c_position_on_plane);
}


/****************************************/
/****************************************/

void HomingMission::Reset() {
  GenericLoopFunctions::Reset();
  m_unRobotsAtHome = 0;
  m_unAnyTimeScore = 0;
}

/****************************************/
/****************************************/

void HomingMission::PostStep() {
  m_unRobotsAtHome = 0;

  CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
  CVector2 cEpuckPosition(0,0);
  for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
    CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);

    std::string strRobotId = pcEpuck->GetId();
    cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                       pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

    // Check if robot on home patch
    if (m_pcHome->IsOnPatch(cEpuckPosition)) {
      m_unRobotsAtHome += 1;
    }
  }

  m_unAnyTimeScore += m_unRobotsAtHome;
}

/****************************************/
/****************************************/

Real HomingMission::GetScore() {
  UInt32 unScore = 0;
  if (m_strTypeObjectiveFunction == "'anytime'") {
    unScore = m_unAnyTimeScore;
  } else if (m_strTypeObjectiveFunction == "'endtime'") {
    unScore = m_unRobotsAtHome;
  }
  return unScore;
}

/****************************************/
/****************************************/

void HomingMission::PostExperiment() {
  LOG << "Score " << GetScore() << std::endl;
}

/****************************************/
/****************************************/

void HomingMission::Initialize() {
  GenericLoopFunctions::Initialize();

  std::map<std::string, argos::CAny>::iterator it;

  it = GetVariableFromDictionary("typeObjFunc");
  m_strTypeObjectiveFunction = argos::any_cast<std::string>(it->second);

  it = GetVariableFromDictionary("colorHome");
  std::string strColorHome = argos::any_cast<std::string>(it->second);
  if (strColorHome == "'white'") {
    m_cColorHome = argos::CColor::WHITE;
    m_cColorDistractions = argos::CColor::BLACK;
  } else if (strColorHome == "'black'") {
    m_cColorHome = argos::CColor::BLACK;
    m_cColorDistractions = argos::CColor::WHITE;
  } else {
    std::cout << "\033[1;31m" << "Error: unknown color nest \"" << strColorHome << "\" ." << "\033[0m" << std::endl;
    std::exit(1);
  }

  // Setting up patches. The first one is the Home, the others are distractions
  it = GetVariableFromDictionary("numberPatches");
  unsigned int unNumberPatches = stoi(argos::any_cast<std::string>(it->second));
  std::vector<Patch*> vecPossibleAggregationPatches = m_pcArena->GetPossibleAggregationPatches();
  std::vector<Patch*> vecAllPossiblePatches = m_pcArena->GetAllPossiblePatches();
  unsigned int iPatch = 0;
  unsigned int unRandomIndex;
  while (iPatch < unNumberPatches) {
    Patch* pcNewPatch;
    if (iPatch == 0) {
      unRandomIndex = rand() % vecPossibleAggregationPatches.size();
      pcNewPatch = vecPossibleAggregationPatches[unRandomIndex];
      pcNewPatch->SetIndex(m_vecAllPatches.size());
      pcNewPatch->SetColor(m_cColorHome);
    } else {
      unRandomIndex = rand() % vecAllPossiblePatches.size();
      pcNewPatch = vecAllPossiblePatches[unRandomIndex];
      pcNewPatch->SetIndex(m_vecAllPatches.size());
      pcNewPatch->SetColor(m_cColorDistractions);
    }

    // Add patch to list of patches if no overlap with existing ones
    if (!(m_pcArena->ArePatchesOverlapping(m_vecAllPatches, pcNewPatch))) {
      m_vecAllPatches.push_back(pcNewPatch);
      if (iPatch == 0) {
        m_pcHome = pcNewPatch;
        vecPossibleAggregationPatches.erase(vecPossibleAggregationPatches.begin() + unRandomIndex);
      } else {
        m_vecDistractionPatches.push_back(pcNewPatch);
        vecAllPossiblePatches.erase(vecAllPossiblePatches.begin() + unRandomIndex);
      }
      iPatch += 1;
    }
  }
}

/****************************************/
/****************************************/

std::string HomingMission::GetLowLevelDescription() {
  std::string strGenericPart = GenericLoopFunctions::GetLowLevelDescription();
  std::ostringstream cStream;
  cStream << strGenericPart;
  cStream << "--tof " << m_strTypeObjectiveFunction << " ";
  cStream << "--colhome " << m_cColorHome << " ";
  cStream << "--coldistration " << m_cColorDistractions << " ";
  cStream << "--np " << m_vecAllPatches.size() << " ";
  for (size_t i = 0; i < m_vecAllPatches.size(); ++i) {
    cStream << m_vecAllPatches[i]->GetLowLevelDescription();
  }
  return cStream.str();
}

/****************************************/
/****************************************/

float HomingMission::GetMaxValue(std::vector<float> vec_values) {
  float fLargestElement = vec_values[0];
  for (std::vector<float>::iterator it = vec_values.begin(); it < vec_values.end(); it++) {
    if (*it > fLargestElement) {
      fLargestElement = *it;
    }
  }
  return fLargestElement;
}

REGISTER_LOOP_FUNCTIONS(HomingMission, "generic_homing");
