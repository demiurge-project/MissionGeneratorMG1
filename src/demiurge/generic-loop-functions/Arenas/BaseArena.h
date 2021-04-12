#ifndef BASE_ARENA
#define BASE_ARENA

#include <math.h>

#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/utility/math/vector3.h>

#include <argos3/demiurge/generic-loop-functions/Arenas/Objects/Box.h>
#include <argos3/demiurge/generic-loop-functions/Arenas/Objects/Patch.h>

class BaseArena {

  public:

    /*
     * STriangle are used to determine whether a point is in the arena:
     *       for each arena, a set of triangles is given;
     *       if the given point is inside at least one of these triangles, the point is within the arena.
     */
    struct STriangle {
      argos::CVector2 A;
      argos::CVector2 B;
      argos::CVector2 C;

    STriangle(argos::CVector2 c_A, argos::CVector2 c_B, argos::CVector2 c_C):
      A(c_A), B(c_B), C(c_C) {}

    };

    virtual ~BaseArena();

    std::string GetARGoSDescription();

    void CreateWall(unsigned short n_index, argos::CVector2 c_position, float f_orientation, float f_length);

    void CreateObstacle(argos::CVector2 c_position, float f_orientation);

    void SetSideLength(float f_side_length);

    float GetCircumradius();

    void InstantiateObstacles(unsigned short n_number_obstacles);

    std::vector<argos::CVector3> GetPossibleLightPositions();

    std::vector<Patch*> GetAllPossiblePatches();

    std::vector<Patch*> GetPossibleNestPatches();

    std::vector<Patch*> GetPossibleFoodSourcePatches();

    std::vector<Patch*> GetPossibleAggregationPatches();

    bool ArePatchesOverlapping(std::vector<Patch*> vec_patches, Patch* pc_new_patch);

    std::string GetArenaType();

    virtual std::string GetLowLevelDescription();

    virtual bool IsInArena(argos::CVector2 c_point);

  protected:

    virtual void Initialize() = 0;

    float ComputeMinimalDistanceToAvoidOverlap(Patch* pc_patch1, Patch* pc_patch2);

    /*
     * Checks if a point is within a triangle. Use the sum of the areas technique.
     */
    bool IsInTriangle(STriangle s_triangle, argos::CVector2 c_point);

    /*
     * Computes the area of a triangle struct
     */
    float ComputeAreaTriangle(STriangle s_triangle);

    /*
     * Computes the area of a triangle formed by three points
     */
    float ComputeAreaTriangle(argos::CVector2 c_A, argos::CVector2 c_B, argos::CVector2 c_C);

    /*
     * A string containing an identifier for the mission (example: foraging, aggregation, etc.)
     */
    std::string m_strArenaType;

    float m_fSideLength;

    float m_fWallWidth = 0.055;

    float m_fCircumradius;

    /*
     * List of walls;
     */
    std::vector<Box*> m_vecWalls;

    /*
     * List of obstacles;
     */
    std::vector<Box*> m_vecObstacles;

    /*
     * List of possible positions for lights;
     */
    std::vector<argos::CVector3> m_vecPossibleLightPositions;

    /*
     * List of possible positions and orienations for obstacles;
     */
    std::vector<Box*> m_vecPossibleObstacles;

    std::vector<Patch*> m_vecPossibleNestPatches;

    std::vector<Patch*> m_vecPossibleFoodSourcePatches;

    /*
     * List of possible positions for patches on which the robot must aggregate (for Aggregation missions);
     */
    std::vector<Patch*> m_vecPossibleAggregationPatches;

    std::vector<Patch*> m_vecAllPossiblePatches;

    /*
     * List of triangles that compose the arena. These triangles are used to check whether a point is within the arena or not.
     */
    std::vector<STriangle> m_vecEmbodiedTriangles;

};

#endif
