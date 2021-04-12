#include <algorithm>
#include <random>
#include <vector>
#include <iostream>

#include "ConfigurationParser.h"
#include <argos3/demiurge/generic-loop-functions/Missions/GenericLoopFunctions.h>
#include <argos3/demiurge/generic-loop-functions/Missions/ForagingMission.h>
#include <argos3/demiurge/generic-loop-functions/Missions/HomingMission.h>
#include <argos3/demiurge/generic-loop-functions/Missions/AggregationXORMission.h>
#include <argos3/demiurge/generic-loop-functions/Arenas/BaseArena.h>
#include <argos3/demiurge/generic-loop-functions/Arenas/TetragonArena.h>

#include <argos3/core/utility/datatypes/any.h>

using namespace argos;

const std::string ExplainParameters() {
	std::string strExplanation = "Missing configuration file. The possible parameters are: \n\n"
    " --conf CONF \t The configuration file [MANDATORY];\n"
    " --seed \t The seed for the generator [OPTIONAL];\n"
		" --nb \t The numbers of missions to generate [OPTIONAL].\n";
	return strExplanation;
}

const argos::CAny SampleVariable(ConfigurationParser::SConfigVariable s_variable) {
	argos::CAny cSampledValue;
	// If no Distribution values, sample uniformly
	if (s_variable.Distribution.size() == 0) {
		int unIndex = rand() % s_variable.Range.size();
		cSampledValue = s_variable.Range[unIndex];
	} else { // Else, sample according to weights specified in Distribution values
		float fFloat = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		bool bSampled = false;
		for (size_t i = 0; i < s_variable.Range.size(); i++) {
			if ((fFloat < s_variable.Distribution[i]) && (not(bSampled))) {
				cSampledValue = s_variable.Range[i];
				bSampled = true;
			}
			fFloat -= s_variable.Distribution[i];
		}
	}
	return cSampledValue;
}


int main(int n_argc, char** ppch_argv) {

  bool bConfigurationFileGiven = false;
  std::string strConfigurationFile;

  bool bSeedGiven = false;
  int intSeed;

  unsigned int intNumberMissionsToGenerate = 1;

  int nCurrentArgument = 1;
  while(nCurrentArgument < n_argc) {
    if(strcmp(ppch_argv[nCurrentArgument], "--conf") == 0) {
      bConfigurationFileGiven = true;
      strConfigurationFile = std::string(ppch_argv[nCurrentArgument+1]);
    } else if (strcmp(ppch_argv[nCurrentArgument], "--seed") == 0) {
      bSeedGiven = true;
      intSeed = atoi(ppch_argv[nCurrentArgument+1]);
    } else if (strcmp(ppch_argv[nCurrentArgument], "--nb") == 0) {
      intNumberMissionsToGenerate = atoi(ppch_argv[nCurrentArgument+1]);
    }
		nCurrentArgument++;
  }

	if (bSeedGiven) {
		srand(intSeed);
	} else {
		intSeed = time(0);
		srand(intSeed);
	}
	std::cout << "Using seed " << intSeed << std::endl;

	/* Configuration file not given, terminate */
	if (!bConfigurationFileGiven) {
		std::cout << ExplainParameters() << std::endl;
		return 1;
	}

	ConfigurationParser* pcConfigurationParser = new ConfigurationParser();
	ConfigurationParser::TConfiguration cMissionConfiguration =	pcConfigurationParser->Parse(strConfigurationFile);

	std::ofstream ofOutputFile("generated_missions/all_missions.txt");
	if (ofOutputFile.is_open()) {
		for (unsigned int i = 0; i < intNumberMissionsToGenerate; i++) {
			std::cout << " *** Mission " << i << " *** " << std::endl;
			/* Handling mission */
			GenericLoopFunctions* pcMission;
			if (cMissionConfiguration[0].Name == "missionClass") {
				std::string strMission =  argos::any_cast<std::string>(SampleVariable(cMissionConfiguration[0]));
				if (strMission == "'foraging'") {
					pcMission = new ForagingMission();
				} else if (strMission == "'homing'") {
					pcMission = new HomingMission();
				} else if (strMission == "'aggXOR'") {
					pcMission = new AggregationXORMission();
				}
				if (pcMission->IsConditionRespected(cMissionConfiguration[0])) {
					pcMission->AddVariable("mission", strMission);
				}
			} else {
				std::cout << "Error: expected \'mission\' as first variable" << std::endl;
				return 1;
			}


			argos::CAny anySampledParameter;
			std::string strVariableName;
			for (size_t i = 1; i < cMissionConfiguration.size(); ++i) {
				if (pcMission->IsConditionRespected(cMissionConfiguration[i])) {
					anySampledParameter = SampleVariable(cMissionConfiguration[i]);
					strVariableName = cMissionConfiguration[i].Name;
					pcMission->AddVariable(strVariableName, anySampledParameter);
				}
			}


			pcMission->Initialize();

			// Can print out the description of the mission in terminal
			// pcMission->PrintDictionaryContent();
			ofOutputFile << pcMission->GetLowLevelDescription() << std::endl;
			
			/* Writting ARGOS configuration file from template */
			pcMission->WriteARGoSConfigurationFile("src/mission_template.argos", "generated_missions/mission_" + std::to_string(i) + ".argos");

			delete pcMission;
		}
		ofOutputFile.close();
	}

  /* Everything's ok, exit */
  return 0;

}
