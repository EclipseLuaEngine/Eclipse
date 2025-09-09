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

        EclipseSolState* GetGlobalState();
        EclipseSolState* GetStateForMap(int32 mapId);

        void RunAllScripts();

    private:
        EclipseStateManager() = default;
        ~EclipseStateManager() = default;
        EclipseStateManager(const EclipseStateManager&) = delete;
        EclipseStateManager& operator=(const EclipseStateManager&) = delete;

        std::unordered_map<int32, std::unique_ptr<EclipseSolState>> _states;
};


#endif // ECLIPSE_STATE_MANAGER_HPP