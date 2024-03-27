//EventMaanger.cpp

#include "SurvivantCore/Events/EventManager.h"

namespace SvCore::Events
{
    EventManager& EventManager::GetInstance()
    {
        static EventManager s_instance;

        return s_instance;
    }
}
