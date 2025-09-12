#ifndef ECLIPSE_STATE_MANAGER_HPP
#define ECLIPSE_STATE_MANAGER_HPP

#include "EclipseIncludes.hpp"
#include "EclipseSolState.hpp"

#include <unordered_map>
#include <memory>

class EclipseStateManager
{
    public:
        static EclipseStateManager& GetInstance();

        EclipseSolState* CreateState(Map* map);

        EclipseSolState* GetGlobalState() { return _states[-1].get(); };
        EclipseSolState* GetStateByMap(Map* map) { return _states[map->GetId()].get();  }
        EclipseSolState* GetStateByMapId(int32 mapId) { return _states[mapId].get(); }

    private:
        EclipseStateManager() = default;
        ~EclipseStateManager() = default;
        EclipseStateManager(const EclipseStateManager&) = delete;
        EclipseStateManager& operator=(const EclipseStateManager&) = delete;

        std::unordered_map<int32, std::unique_ptr<EclipseSolState>> _states;

    protected:
        bool RunFromCache(sol::state& solState, const std::string& filePath, sol::table& modules, const std::string& filename, uint32& compiledCount, uint32& cachedCount);
        bool RunFromFile(sol::state& solState, const std::string& filePath, sol::table& modules, const std::string& filename);
};


#endif // ECLIPSE_STATE_MANAGER_HPP