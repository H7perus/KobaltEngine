#include "Engine/Entity/EntityWorldSystem.h"

#include "SDL3/SDL.h"

namespace KE
{
class IRenderer : public EntityWorldSystem
{
    virtual void Init(SDL_Window*& windowPtr) = 0;

    virtual void Draw() = 0;
};
}
