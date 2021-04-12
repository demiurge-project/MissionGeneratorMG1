/*
 * @file <src/Base/GenericLoopFunctions.h>
 *
 * @author Antoine Ligot - <aligot@ulb.ac.be>
 *
 * @package ARGoS3-MissionGenerator
 *
 * @license MIT License
 */

#ifndef HOMING_MISSION
#define HOMING_MISSION

#include <map>

#include "GenericLoopFunctions.h"

#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

using namespace argos;

class HomingMission: public GenericLoopFunctions {
  public:
    HomingMission();
    HomingMission(const HomingMission& orig);
    virtual ~HomingMission();

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

    Patch* m_pcHome;
    std::vector<Patch*> m_vecDistractionPatches;
    argos::CColor m_cColorHome;
    argos::CColor m_cColorDistractions;

    std::string m_strTypeObjectiveFunction;
    UInt32 m_unRobotsAtHome;
    UInt32 m_unAnyTimeScore;


};

#endif
