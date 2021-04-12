/*
 * @file <src/Base/GenericLoopFunctions.h>
 *
 * @author Antoine Ligot - <aligot@ulb.ac.be>
 *
 * @package ARGoS3-MissionGenerator
 *
 * @license MIT License
 */

#ifndef AGGREGATION_XOR_MISSION
#define AGGREGATION_XOR_MISSION

#include <map>

#include "GenericLoopFunctions.h"

#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

using namespace argos;

class AggregationXORMission: public GenericLoopFunctions {
  public:
    AggregationXORMission();
    AggregationXORMission(const AggregationXORMission& orig);
    virtual ~AggregationXORMission();

    virtual void Destroy();
    virtual void Init(TConfigurationNode& t_tree);

    virtual argos::CColor GetFloorColor(const argos::CVector2& c_position_on_plane);
    virtual void PostStep();
    virtual void Reset();
    virtual void PostExperiment();

    /*
     * Initialization method where the parameters are within dictionary.
     */
    void Initialize();

    std::string GetLowLevelDescription();

    Real GetScore();

  private:

    float GetMaxValue(std::vector<float> vec_values);
    UInt32 SmallestGroupOfRobotsInOneHome();
    UInt32 LargestGroupOfRobotsInOneHome();

    UInt16 m_unNumberHomes;
    std::vector<Patch*> m_vecHomes;
    std::vector<Patch*> m_vecDistractionPatches;
    argos::CColor m_cColorHome;
    argos::CColor m_cColorDistractions;

    std::string m_strTypeObjectiveFunction;
    Real m_fCurrentTimeScore;
    Real m_fAnyTimeScore;

    std::map<UInt16, UInt32> m_mapHomesData;

};

#endif
