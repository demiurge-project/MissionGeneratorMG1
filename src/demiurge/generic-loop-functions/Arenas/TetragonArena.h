#ifndef TETRAGON_ARENA
#define TETRAGON_ARENA

#include "BaseArena.h"

class TetragonArena : public BaseArena {

  public:

    TetragonArena();

    virtual ~TetragonArena();

  protected:

    virtual void Initialize();

};

#endif
