/*
 * @file <src/Base/GenericLoopFunctions.h>
 *
 * @author Antoine Ligot - <aligot@ulb.ac.be>
 *
 * @package ARGoS3-MissionGenerator
 *
 * @license MIT License
 */

#ifndef GENERIC_LOOP_FUNCTIONS
#define GENERIC_LOOP_FUNCTIONS

#include <map>
#include <fstream>
#include <regex>
#include <random>

#include <argos3/demiurge/generic-loop-functions/Generator/ConfigurationParser.h>

#include <argos3/demiurge/generic-loop-functions/Arenas/BaseArena.h>
#include <argos3/demiurge/generic-loop-functions/Arenas/TetragonArena.h>
#include <argos3/demiurge/generic-loop-functions/Arenas/TrigonArena.h>
#include <argos3/demiurge/generic-loop-functions/Arenas/HexagonArena.h>
#include <argos3/demiurge/generic-loop-functions/Arenas/DodecagonArena.h>
#include <argos3/demiurge/generic-loop-functions/Arenas/HeartArena.h>

#include <argos3/demiurge/generic-loop-functions/Arenas/Objects/Light.h>
#include <argos3/demiurge/generic-loop-functions/Arenas/Objects/Patch.h>

#include <argos3/core/simulator/space/space.h>
#include <argos3/core/utility/datatypes/any.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

using namespace argos;

class GenericLoopFunctions: public CLoopFunctions {

  public:

    ~GenericLoopFunctions();

    /*
     * Initialization method where the parameters of the loop function
     * are fetched from the xml declaration.
     */
    virtual void Init(argos::TConfigurationNode& t_tree);

    /*
     * This method should return the performance of the control software.
     */
    virtual Real GetScore() = 0;

    argos::CColor GetFloorColor(const argos::CVector2& c_position_on_plane);

    void AddVariable(std::string str_key, argos::CAny any_value);

    bool IsConditionRespected(const ConfigurationParser::SConfigVariable s_variable);

    void PrintDictionaryContent();

    /*
     * Initialization method where the parameters are within dictionary.
     */
    virtual void Initialize();

    void WriteARGoSConfigurationFile(std::string str_template_file, std::string str_output_file);

    virtual std::string GetLowLevelDescription();

  protected:

    /*
     * Reset function. First remove the robots, then re-position them.
     */
    virtual void Reset();

    /*
     * Method used to remove the robots from the arena.
     */
    void RemoveRobots();

    /*
     * Method used to reallocate the robots.
     * The position is given by the method GetRandomPosition().
     */
    void MoveRobots();

    /*
     * Return a random position.
     */
    virtual CVector3 GetRandomPosition();

    /*
     * The number of robots to be placed for the experiment.
     */
    UInt32 m_unNumberRobots;

    std::string m_strPositionDistributionType;

    /*
     * Flag specifying the side on distribution of the robots used in case distribution is of type "oneside"
     */
    bool m_bSpawnSide;

    /*
     * Flag that should be set to true if loop function is initialized from ARGoS and not from the generator of missions
     */
    bool m_bInitializedByARGoS;

    /*
     * The radius of the circle in which the robots will be distributed.
     */
    Real m_fDistributionRadius;

    UInt32 m_unExperimentDuration;

    /*
     * The center of the circle in which the robots will be distributed.
     */
    CVector2 m_cCenterDistribution;

    /*
     * A string containing an identifier for the mission (example: foraging, aggregation, etc.)
     */
    std::string m_strMissionType;

    /*
     * A string the low level description of the mission (example: --m foraging --el 150 ... )
     */
    std::string m_strMissionDescription;

    std::vector<std::string> m_vecMissionDescription;

    /*
     * Dictionary countaining mission's variables
     */ 
    std::map<std::string, argos::CAny> m_dicVariables;

    /**
     * Vector used to keep track of the order of variables in dictionary
     */
    std::vector<std::string> m_vecVariables;

    CRandom::CRNG* m_pcRng;

    /*
     * Checks if variable str_key exists in dictionary m_dicVariables and that value any_value is assigned to it
     */
    bool IsInstanciated(std::string str_key, argos::CAny any_value);

    std::map<std::string, argos::CAny>::iterator GetVariableFromDictionary(const std::string& str_key);

    /*
     * Split low level description of mission (given as parameter of the loopfunctions in the ARGoS file)
     */
    std::vector<std::string> SplitDescriptionString(std::string& str_string_to_split);

    BaseArena* HandleArena(const std::string& str_arena_type);

    Patch* HandlePatch(std::vector<std::string>& vec_patch_config, UInt32 un_index_patch);

    BaseArena* m_pcArena;

    Light* m_pcLight;

    std::vector<Patch*> m_vecAllPatches;

    bool IsOnAnyPatch(const CVector2& c_point);

};

#endif
