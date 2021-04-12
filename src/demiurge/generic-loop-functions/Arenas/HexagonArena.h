#ifndef HEXAGON_ARENA
#define HEXAGON_ARENA

#include "BaseArena.h"

class HexagonArena : public BaseArena {

  public:

    HexagonArena();

    virtual ~HexagonArena();

  protected:

    virtual void Initialize();

};

#endif
