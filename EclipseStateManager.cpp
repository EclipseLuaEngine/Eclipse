#include "EclipseCache.hpp"
#include "EclipseCompiler.hpp"
#include "EclipseStateManager.hpp"
#include "EclipseScriptLoader.hpp"
#include "EclipseIncludes.hpp"
#include "EclipseConfig.hpp"
#include "EclipseLogger.hpp"
#include <chrono>

/**
 *
 */
EclipseStateManager& EclipseStateManager::GetInstance()
{
    static EclipseStateManager instance;
    return instance;
}

/**
 *
 */
EclipseSolState* EclipseStateManager::CreateState(Map* map)
{
    int32 mapId = map ? map->GetId() : -1;
    auto [it, inserted] = _states.try_emplace(mapId, nullptr);

    if(!inserted)
        return it->second.get();

    auto engine = std::make_unique<EclipseSolState>(map);
    ECLIPSE_LOG_DEBUG("Creating new Lua state for map " + std::to_string(mapId));

    if(engine->IsInitialized())
    {
        EclipseSolState* enginePtr = engine.get();
        it->second = std::move(engine);
        return enginePtr;
    }

    _states.erase(it);
    return nullptr;
}