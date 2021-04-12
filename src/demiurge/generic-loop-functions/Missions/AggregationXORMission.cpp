#include "AggregationXORMission.h"

/****************************************/
/****************************************/

AggregationXORMission::AggregationXORMission() {
  m_strMissionType = "aggXOR";
  m_fCurrentTimeScore = 0.0f;
  m_fAnyTimeScore = 0.0f;
  m_pcLight = NULL;
  m_bInitializedByARGoS = false;
}

/****************************************/
/****************************************/

AggregationXORMission::AggregationXORMission(const AggregationXORMission& orig) {
}

/****************************************/
/****************************************/

void AggregationXORMission::Init(TConfigurationNode& t_tree) {
  m_bInitializedByARGoS = true;

  GenericLoopFunctions::Init(t_tree);

  std::vector<std::string>::iterator it;

  try {
    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--colhome");
    m_cColorHome.Set((*(it+1)).c_str());

    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--coldistraction");
    m_cColorDistractions.Set((*(it+1)).c_str());

    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--tof");
    m_strTypeObjectiveFunction = (*(it+1)).c_str();

    // Initializing the map containing the number of robots on each home
    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--nh");
    m_unNumberHomes = atoi((*(it+1)).c_str());
    for (UInt16 i = 0; i < m_unNumberHomes; i++) {
      m_mapHomesData[i] = 0;
    }
    std::cout << "Ok" << std::endl;


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
        m_vecHomes.push_back(pcCurrentPatch);
      } else {
        m_vecDistractionPatches.push_back(pcCurrentPatch);
      }
      std::cout << "Ok" << i << std::endl;

    }

  } catch (std::exception e) {
    LOGERR << "Error while parsing mission description: " << e.what() << std::endl;
  }

  MoveRobots();
}

/****************************************/
/****************************************/

AggregationXORMission::~AggregationXORMission() {
  if (m_bInitializedByARGoS) {
    for (size_t i = 0; i < m_vecHomes.size(); i++) {
      delete m_vecHomes[i];
    }
    for (size_t i = 0; i < m_vecDistractionPatches.size(); i++) {
      delete m_vecDistractionPatches[i];
    }
  }
}

/****************************************/
/****************************************/

void AggregationXORMission::Destroy() {}

/****************************************/
/****************************************/

argos::CColor AggregationXORMission::GetFloorColor(const argos::CVector2& c_position_on_plane) {
  return GenericLoopFunctions::GetFloorColor(c_position_on_plane);
}


/****************************************/
/****************************************/

void AggregationXORMission::Reset() {
  GenericLoopFunctions::Reset();
  m_fCurrentTimeScore = 0.0f;
  m_fAnyTimeScore = 0.0f;
  for (UInt16 i = 0; i < m_unNumberHomes; i++) {
    m_mapHomesData[i] = 0;
  }
}

/****************************************/
/****************************************/

void AggregationXORMission::PostStep() {
  std::map<UInt16, UInt32> mapScoreTemp = m_mapHomesData;
  for (UInt16 i = 0; i < m_unNumberHomes; i++) {
    m_mapHomesData[i] = 0;
  }

  CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
  CVector2 cEpuckPosition(0,0);
  for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
    CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);

    std::string strRobotId = pcEpuck->GetId();
    cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                       pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

    // Check if robot on any of the home
    for (std::vector<Patch*>::iterator itH = m_vecHomes.begin(); itH != m_vecHomes.end(); ++itH) {
      if ((*itH)->IsOnPatch(cEpuckPosition)) {
        m_mapHomesData[(*itH)->GetIndex()] += 1;
      }
    }
  }

  m_fCurrentTimeScore = LargestGroupOfRobotsInOneHome() / Real(m_unNumberRobots);
  m_fAnyTimeScore += m_fCurrentTimeScore;

}

/****************************************/
/****************************************/

Real AggregationXORMission::GetScore() {
  Real fScore = 0.0f;
  if (m_strTypeObjectiveFunction == "'anytime'") {
    fScore = m_fAnyTimeScore;
  } else if (m_strTypeObjectiveFunction == "'endtime'") {
    fScore = m_fCurrentTimeScore;
  }
  return fScore;
}

/****************************************/
/****************************************/

void AggregationXORMission::PostExperiment() {
  LOG << "Score " << GetScore() << std::endl;
}

/****************************************/
/****************************************/

void AggregationXORMission::Initialize() {
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
  it = GetVariableFromDictionary("numberHomes");
  m_unNumberHomes = stoi(argos::any_cast<std::string>(it->second));
  it = GetVariableFromDictionary("numberDistractionPatches");
  unsigned int unNumberDistractions = stoi(argos::any_cast<std::string>(it->second));
  unsigned int iPatch = 0;

  std::vector<Patch*> vecPossibleAggregationPatches = m_pcArena->GetPossibleAggregationPatches();
  std::vector<Patch*> vecAllPossiblePatches = m_pcArena->GetAllPossiblePatches();
  unsigned int unRandomIndex;

  while (iPatch < (m_unNumberHomes + unNumberDistractions)) {
    Patch* pcNewPatch;
    if (iPatch < m_unNumberHomes) {
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
      if (iPatch < m_unNumberHomes) {
        m_vecHomes.push_back(pcNewPatch);
        vecPossibleAggregationPatches.erase(vecPossibleAggregationPatches.begin() + unRandomIndex);
      } else {
        m_vecDistractionPatches.push_back(pcNewPatch);
        vecAllPossiblePatches.erase(vecAllPossiblePatches.begin() + unRandomIndex);
      }
      pcNewPatch = NULL;
      iPatch += 1;
    }
  }

}

/****************************************/
/****************************************/

std::string AggregationXORMission::GetLowLevelDescription() {
  std::string strGenericPart = GenericLoopFunctions::GetLowLevelDescription();
  std::ostringstream cStream;
  cStream << strGenericPart;
  cStream << "--tof " << m_strTypeObjectiveFunction << " ";
  cStream << "--colhome " << m_cColorHome << " ";
  cStream << "--coldistraction " << m_cColorDistractions << " ";
  cStream << "--nh " << m_unNumberHomes << " ";
  cStream << "--np " << m_vecAllPatches.size() << " ";
  for (size_t i = 0; i < m_vecAllPatches.size(); ++i) {
    cStream << m_vecAllPatches[i]->GetLowLevelDescription();
  }
  return cStream.str();
}

/****************************************/
/****************************************/

float AggregationXORMission::GetMaxValue(std::vector<float> vec_values) {
  float fLargestElement = vec_values[0];
  for (std::vector<float>::iterator it = vec_values.begin(); it < vec_values.end(); it++) {
    if (*it > fLargestElement) {
      fLargestElement = *it;
    }
  }
  return fLargestElement;
}

/****************************************/
/****************************************/

UInt32 AggregationXORMission::SmallestGroupOfRobotsInOneHome() {
  UInt32 unSmallestGroup = m_unNumberRobots;
  for (UInt16 i = 0; i < m_unNumberHomes; i++) {
    if (m_mapHomesData[i] < unSmallestGroup) {
      unSmallestGroup = m_mapHomesData[i];
    }
  }
  return unSmallestGroup;
}

/****************************************/
/****************************************/

UInt32 AggregationXORMission::LargestGroupOfRobotsInOneHome() {
  UInt32 unLargestGroup = 0;
  for (UInt16 i = 0; i < m_unNumberHomes; i++) {
    if (m_mapHomesData[i] > unLargestGroup) {
      unLargestGroup = m_mapHomesData[i];
    }
  }
  return unLargestGroup;
}

REGISTER_LOOP_FUNCTIONS(AggregationXORMission, "generic_aggXOR");
