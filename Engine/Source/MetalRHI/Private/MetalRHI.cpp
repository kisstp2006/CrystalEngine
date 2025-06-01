
#include "MetalRHI.h"

#include "MetalRHI.private.h"


namespace CE::Metal
{
    class MetalRHIModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {

        }

        virtual void ShutdownModule() override
        {

        }

        virtual void RegisterTypes() override
        {

        }
    };
}

CE_IMPLEMENT_MODULE(MetalRHI, CE::Metal::MetalRHIModule)
