/*
 * @file <src/GenericLoopFunctions.cpp>
 *
 * @author Antoine Ligot - <aligot@ulb.ac.be>
 *
 * @package ARGoS3-MissionGenerator
 *
 * @license MIT License
 */


 #include "GenericLoopFunctions.h"

/****************************************/
/****************************************/

void GenericLoopFunctions::Init(argos::TConfigurationNode& t_tree) {
  m_pcRng = CRandom::CreateRNG("argos");
  TConfigurationNode cParametersNode;
  try {
    cParametersNode = GetNode(t_tree, "params");
    GetNodeAttributeOrDefault(cParametersNode, "dist_radius", m_fDistributionRadius, (Real) 0);
    GetNodeAttribute(cParametersNode, "mission_description", m_strMissionDescription);
  } catch(std::exception e) {
    LOGERR << "Problem with Attributes in node params" << std::endl;
  }

  m_vecMissionDescription = SplitDescriptionString(m_strMissionDescription);

  std::vector<std::string>::iterator it;

  try {
    // Setting up arena
    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--a");
    m_pcArena = HandleArena((*(it+1)).c_str());
    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--as");
    m_pcArena->SetSideLength(strtod((*(it+1)).c_str(), NULL));

    // Setting up number of robots
    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--r");
    m_unNumberRobots = atoi((*(it+1)).c_str());

    // Setting up placement of robots
    it = std::find(m_vecMissionDescription.begin(), m_vecMissionDescription.end(), "--ip");
    m_strPositionDistributionType = (*(it+1)).c_str();
    if (m_strPositionDistributionType == "'oneside'") {
      m_bSpawnSide = atoi((*(it+2)).c_str());
    }

  } catch (std::exception e) {
    LOGERR << "Error while parsing mission description in generic loop func: " << e.what() << std::endl;
  }
}

/****************************************/
/****************************************/

void GenericLoopFunctions::Reset() {
  MoveRobots();
}

/****************************************/
/****************************************/

GenericLoopFunctions::~GenericLoopFunctions() {
  delete m_pcArena;
  if (m_pcLight != NULL) {
    delete m_pcLight;
  }
}

/****************************************/
/****************************************/

void GenericLoopFunctions::MoveRobots() {
  CEPuckEntity* pcEpuck;
  bool bPlaced = false;
  UInt32 unTrials;
  CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
  for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
    pcEpuck = any_cast<CEPuckEntity*>(it->second);
    // Choose position at random
    unTrials = 0;
    do {
       ++unTrials;
       CVector3 cEpuckPosition = GetRandomPosition();
       bPlaced = MoveEntity(pcEpuck->GetEmbodiedEntity(),
                            cEpuckPosition,
                            CQuaternion().FromEulerAngles(m_pcRng->Uniform(CRange<CRadians>(CRadians::ZERO,CRadians::TWO_PI)),
                            CRadians::ZERO,CRadians::ZERO),false);
    } while(!bPlaced && unTrials < 1000);
    if(!bPlaced) {
       THROW_ARGOSEXCEPTION("Can't place robot");
    }
  }
}

/****************************************/
/****************************************/

void GenericLoopFunctions::RemoveRobots() {
  for(UInt32 i = 1; i < m_unNumberRobots + 1; ++i) {
    std::ostringstream id;
    id << "epuck" << i;
    RemoveEntity(id.str().c_str());
  }
}

/****************************************/
/****************************************/

argos::CColor GenericLoopFunctions::GetFloorColor(const argos::CVector2& c_position_on_plane) {
  CVector2 vCurrentPoint(c_position_on_plane.GetX(), c_position_on_plane.GetY());
  std::vector<Patch*>::iterator it;
  for (it = m_vecAllPatches.begin(); it != m_vecAllPatches.end(); ++it) {
    if ((*it)->IsOnPatch(vCurrentPoint) and m_pcArena->IsInArena(vCurrentPoint)) {
      return (*it)->GetColor();
    }
  }

  return CColor::GRAY50;
}

/****************************************/
/****************************************/

CVector3 GenericLoopFunctions::GetRandomPosition() {
  CVector3 cPosition3D;
  CVector2 cPosition2D;

  Real fCircumradius = m_pcArena->GetCircumradius();

  Real fDistanceFromOrigin;
  CRadians cRotationAroundZ;

  bool bPlaced = false;
  UInt32 unNumberTries = 0;

  if (m_strPositionDistributionType == "'uniform'") {
    while (!bPlaced && unNumberTries < 1000) {
      unNumberTries += 1;
      fDistanceFromOrigin = m_pcRng->Uniform(CRange<Real>(0.0, fCircumradius));
      cRotationAroundZ = m_pcRng->Uniform(CRange<CRadians>(CRadians::ZERO, CRadians::TWO_PI));
      cPosition3D.FromSphericalCoords(fDistanceFromOrigin, CRadians::PI_OVER_TWO, cRotationAroundZ);
      if (m_pcArena->IsInArena(CVector2(cPosition3D.GetX(), cPosition3D.GetY()))) {
        bPlaced = true;
      }
    }
  } else if (m_strPositionDistributionType == "'oneside'") {
    while (!bPlaced && unNumberTries < 1000) {
      unNumberTries += 1;
      fDistanceFromOrigin = m_pcRng->Uniform(CRange<Real>(0.0, fCircumradius));
      if (m_pcArena->GetArenaType() != "trigon") {
        cRotationAroundZ = m_pcRng->Uniform(CRange<CRadians>(CRadians::ZERO, CRadians::PI));
        if (m_bSpawnSide) {
          cRotationAroundZ += CRadians::PI;
        }
      } else {
        cRotationAroundZ = m_pcRng->Uniform(CRange<CRadians>(CRadians::PI_OVER_TWO, CRadians::TWO_PI - CRadians::PI_OVER_TWO));
        if (m_bSpawnSide) {
          cRotationAroundZ += CRadians::PI;
        }
      }
      cPosition3D.FromSphericalCoords(fDistanceFromOrigin, CRadians::PI_OVER_TWO, cRotationAroundZ);
      if (m_pcArena->IsInArena(CVector2(cPosition3D.GetX(), cPosition3D.GetY()))) {
        bPlaced = true;
      }
    }
  } else if (m_strPositionDistributionType == "'notpatches'") {
    while (!bPlaced && unNumberTries < 1000) {
      unNumberTries += 1;
      fDistanceFromOrigin = m_pcRng->Uniform(CRange<Real>(0.0, fCircumradius));
      cRotationAroundZ = m_pcRng->Uniform(CRange<CRadians>(CRadians::ZERO, CRadians::TWO_PI));
      cPosition3D.FromSphericalCoords(fDistanceFromOrigin, CRadians::PI_OVER_TWO, cRotationAroundZ);
      cPosition2D = CVector2(cPosition3D.GetX(), cPosition3D.GetY());
      if (m_pcArena->IsInArena(cPosition2D) && (!IsOnAnyPatch(cPosition2D))) {
        bPlaced = true;
      }
    }
  }

  if (bPlaced) {
    return cPosition3D;
  } else {
    THROW_ARGOSEXCEPTION("Can't place robot");
  }
}


/****************************************/
/****************************************/

std::vector<std::string> GenericLoopFunctions::SplitDescriptionString(std::string& str_string_to_split) {
  std::stringstream cStringStream(str_string_to_split);
  std::istream_iterator<std::string> cStringBegin(cStringStream);
  std::istream_iterator<std::string> cStringEnd;
  std::vector<std::string> vStrings(cStringBegin, cStringEnd);

  return vStrings;
}

/****************************************/
/****************************************/

Patch* GenericLoopFunctions::HandlePatch(std::vector<std::string>& vec_patch_config, UInt32 un_index_patch) {
  Patch* pcNewPatch = new Patch();
  pcNewPatch->SetIndex(un_index_patch);

  std::vector<std::string>::iterator it;
  try {
    std::ostringstream oss;
    // Extracting type
    oss << "--p" << un_index_patch;
    it = std::find(vec_patch_config.begin(), vec_patch_config.end(), oss.str());
    pcNewPatch->SetShape((*(it+1)).c_str());
    // Extracting size
    oss.str("");
    oss << "--ps" << un_index_patch;
    it = std::find(vec_patch_config.begin(), vec_patch_config.end(), oss.str());
    pcNewPatch->SetSize(strtod((*(it+1)).c_str(), NULL));
    // Extracting coordinate X of center
    oss.str("");
    oss << "--pcx" << un_index_patch;
    it = std::find(vec_patch_config.begin(), vec_patch_config.end(), oss.str());
    float fCenterPatchX = strtod((*(it+1)).c_str(), NULL);
    // Extracting coordinate Y of center
    oss.str("");
    oss << "--pcy" << un_index_patch;
    it = std::find(vec_patch_config.begin(), vec_patch_config.end(), oss.str());
    float fCenterPatchY = strtod((*(it+1)).c_str(), NULL);
    pcNewPatch->SetPosition(CVector2(fCenterPatchX, fCenterPatchY));
    // Extracting color
    oss.str("");
    oss << "--pc" << un_index_patch;
    it = std::find(vec_patch_config.begin(), vec_patch_config.end(), oss.str());
    pcNewPatch->SetColor((*(it+1)).c_str());
  } catch (std::exception e) {
    THROW_ARGOSEXCEPTION("Error while parsing patch description.");
  }
  return pcNewPatch;
}

/****************************************/
/****************************************/

bool GenericLoopFunctions::IsOnAnyPatch(const CVector2& c_point) {
  bool bOnPatch = false;
  std::vector<Patch*>::iterator it;
  for (it = m_vecAllPatches.begin(); it != m_vecAllPatches.end(); it++) {
    bOnPatch = bOnPatch || (*it)->IsOnPatch(c_point);
  }
  return bOnPatch;
}

/****************************************/
/****************************************/

BaseArena* GenericLoopFunctions::HandleArena(const std::string& str_arena_type) {
  BaseArena* pcArena;
  if (str_arena_type == "tetragon") {
    pcArena = new TetragonArena();
  } else if (str_arena_type == "trigon") {
    pcArena = new TrigonArena();
  } else if (str_arena_type == "hexagon") {
    pcArena = new HexagonArena();
  } else if (str_arena_type == "dodecagon") {
    pcArena = new DodecagonArena();
  } else if (str_arena_type == "heart") {
    pcArena = new HeartArena();
  }
  return pcArena;
}

/****************************************/
/****************************************/

void GenericLoopFunctions::AddVariable(std::string str_key, argos::CAny any_value) {
  m_dicVariables[str_key] = any_value;
  m_vecVariables.push_back(str_key);
}

/****************************************/
/****************************************/

bool GenericLoopFunctions::IsInstanciated(std::string str_key, argos::CAny any_value) {
  bool bInstanciated = false;
  std::map<std::string, argos::CAny>::iterator it;
  it = m_dicVariables.find(str_key);
  if (it != m_dicVariables.end()) {
    if (any_value.m_pcRef->GetType() == typeid(std::string)) {
      bInstanciated = (argos::any_cast<std::string>(any_value) == argos::any_cast<std::string>(it->second));
    }
  }
  return bInstanciated;
}

/****************************************/
/****************************************/

bool GenericLoopFunctions::IsConditionRespected(const ConfigurationParser::SConfigVariable s_variable) {
  if (s_variable.Condition.size() == 0) {
    return true;
  } else {
    bool bRespected = false;
    std::string strCurrentCondition;
    for (size_t i = 0; i < s_variable.Condition.size(); i++) {
      strCurrentCondition = s_variable.Condition[i];
      std::vector<std::string> vecSplittedCondition;
      vecSplittedCondition = SplitString(strCurrentCondition, ' ');
      if (strCurrentCondition.find("==") != std::string::npos) {
        bRespected = IsInstanciated(vecSplittedCondition[0], vecSplittedCondition[2]);
      } else {
        std::cout << "\033[1;31m" << "Error: condition \"" << strCurrentCondition << "\" not properly defined." << "\033[0m" << std::endl;
        std::exit(1);
      }
    }
    return bRespected;
  }
}

/****************************************/
/****************************************/

void GenericLoopFunctions::PrintDictionaryContent() {
  for (std::vector<std::string>::const_iterator itv = m_vecVariables.begin(); itv != m_vecVariables.end(); ++itv) {
    std::map<std::string, argos::CAny>::iterator it;
    it = m_dicVariables.find(*itv);
    if (it != m_dicVariables.end()) {
      if (it->second.m_pcRef->GetType() == typeid(std::string)) {
        std::cout << "Key: " << it->first << ", Value: " << argos::any_cast<std::string>(it->second) << std::endl;
      } else if (it->second.m_pcRef->GetType() == typeid(int)) {
        std::cout << "Key: " << it->first << ", Value: " << argos::any_cast<int>(it->second) << std::endl;
      } else if (it->second.m_pcRef->GetType() == typeid(float)) {
        std::cout << "Key: " << it->first << ", Value: " << argos::any_cast<float>(it->second) << std::endl;
      }
    } else {
      std::cout << "\033[1;31m" << "Error: variable \"" << *itv << "\" disappeared from dictionary." << "\033[0m" << std::endl;
      std::exit(1);
    }
  }
}

/****************************************/
/****************************************/

std::map<std::string, argos::CAny>::iterator GenericLoopFunctions::GetVariableFromDictionary(const std::string& str_key) {
  std::map<std::string, argos::CAny>::iterator it;
  it = m_dicVariables.find(str_key);
  if (it == m_dicVariables.end()) {
    std::cout << "\033[1;31m" << "Error: variable \"" << str_key << "\" not in dictionary." << "\033[0m" << std::endl;
    std::exit(1);
  }
  return it;
}

/****************************************/
/****************************************/

void GenericLoopFunctions::Initialize() {
  std::map<std::string, argos::CAny>::iterator it;

  // Setting up arena
  it = GetVariableFromDictionary("arenaShape");
  std::string strMissionType = argos::any_cast<std::string>(it->second);
  if (strMissionType == "'tetragon'") {
    m_pcArena = new TetragonArena();
  } else if (strMissionType == "'trigon'") {
    m_pcArena = new TrigonArena();
  } else if (strMissionType == "'hexagon'") {
    m_pcArena = new HexagonArena();
  } else if (strMissionType == "'dodecagon'") {
    m_pcArena = new DodecagonArena();
  } else if (strMissionType == "'heart'") {
    m_pcArena = new HeartArena();
  }

  it = GetVariableFromDictionary("arenaSide");
  m_pcArena->SetSideLength(stof(argos::any_cast<std::string>(it->second)));

  // Setting up light
  it = GetVariableFromDictionary("light");
  m_pcLight = new Light();
  m_pcLight->SetStatus(argos::any_cast<std::string>(it->second));
  int unIndex = rand() % m_pcArena->GetPossibleLightPositions().size();
  if (m_pcLight->GetStatus() == "'on'") {
    m_pcLight->SetPosition(m_pcArena->GetPossibleLightPositions()[unIndex]);
  }

  // Setting up obstacles
  it = m_dicVariables.find("numberObstacles");
  m_pcArena->InstantiateObstacles(stoi(argos::any_cast<std::string>(it->second)));

  // Setting up experiment length
  it = m_dicVariables.find("expDuration");
  m_unExperimentDuration = stoi(argos::any_cast<std::string>(it->second));

  // Setting up number of robots
  it = m_dicVariables.find("robots");
  m_unNumberRobots = stoi(argos::any_cast<std::string>(it->second));

  it = m_dicVariables.find("initialPosition");
  m_strPositionDistributionType = argos::any_cast<std::string>(it->second);
  if (m_strPositionDistributionType == "'oneside'") {
    std::random_device rd{}; // use to seed the rng
    std::mt19937 rng{rd()}; // rng
    std::bernoulli_distribution distribution(0.5);
    m_bSpawnSide = distribution(rng);
  }
}

/****************************************/
/****************************************/

void GenericLoopFunctions::WriteARGoSConfigurationFile(std::string str_template_file, std::string str_output_file) {
  std::ifstream ifTemplateFile(str_template_file);
  if (ifTemplateFile.is_open()) {
    std::string strTemplateContent((std::istreambuf_iterator<char>(ifTemplateFile)), (std::istreambuf_iterator<char>()));
    ifTemplateFile.close();
    std::string strOutput = std::regex_replace(strTemplateContent, std::regex("\\$arenaDescription"), m_pcArena->GetARGoSDescription());
    strOutput = std::regex_replace(strOutput, std::regex("\\$missionDescription"), GetLowLevelDescription());
    strOutput = std::regex_replace(strOutput, std::regex("\\$experimentDuration"), std::to_string(m_unExperimentDuration));
    strOutput = std::regex_replace(strOutput, std::regex("\\$missionType"), m_strMissionType);
    strOutput = std::regex_replace(strOutput, std::regex("\\$lightDescription"), m_pcLight->GetARGoSDescription());
    strOutput = std::regex_replace(strOutput, std::regex("\\$numberRobots"), std::to_string(m_unNumberRobots));
    std::ofstream ofOutputFile(str_output_file);
    if (ofOutputFile.is_open()) {
      ofOutputFile << strOutput << std::endl;
      ofOutputFile.close();
    } else {
      std::cout << "\033[1;31m" << "Error: unable to open file {"<< str_output_file << "} \033[0m" << std::endl;
      std::exit(1);
    }
  } else {
    std::cout << "\033[1;31m" << "Error: unable to open file {"<< str_template_file << "} \033[0m" << std::endl;
    std::exit(1);
  }
}

/****************************************/
/****************************************/

std::string GenericLoopFunctions::GetLowLevelDescription() {
  std::ostringstream cStream;
  cStream << "--m " << m_strMissionType << " ";
  cStream << m_pcArena->GetLowLevelDescription() << " ";
  cStream << "--el " << m_unExperimentDuration << " ";
  cStream << "--r " << m_unNumberRobots << " ";
  cStream << "--l " << m_pcLight->GetStatus() << " ";
  if (m_strPositionDistributionType == "'oneside'") {
    cStream << "--ip " << m_strPositionDistributionType << " " << m_bSpawnSide << " ";
  } else {
    cStream << "--ip " << m_strPositionDistributionType << " ";
  }
  return cStream.str();
}
