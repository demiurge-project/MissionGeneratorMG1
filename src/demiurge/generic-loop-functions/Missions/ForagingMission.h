/*
 * @file <src/Base/GenericLoopFunctions.h>
 *
 * @author Antoine Ligot - <aligot@ulb.ac.be>
 *
 * @package ARGoS3-MissionGenerator
 *
 * @license MIT License
 */

#ifndef FORAGING_MISSION
#define FORAGING_MISSION

#include <map>

#include "GenericLoopFunctions.h"

#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

using namespace argos;

class ForagingMission: public GenericLoopFunctions {
  public:
    ForagingMission();
    ForagingMission(const ForagingMission& orig);
    virtual ~ForagingMission();

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

    std::vector<Patch*> m_vecNests;
    std::vector<Patch*> m_vecFoodSources;
    argos::CColor m_cColorNests;
    argos::CColor m_cColorFoodSources;

    UInt32 m_unNumberItemsCollected;
    std::map<std::string, UInt32> m_mapFoodData;


};

#endif
