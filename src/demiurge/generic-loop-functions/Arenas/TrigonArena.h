#ifndef TRIGON_ARENA
#define TRIGON_ARENA

#include "BaseArena.h"

class TrigonArena : public BaseArena {

  public:

    TrigonArena();

    virtual ~TrigonArena();

  protected:

    virtual void Initialize();

};

#endif
