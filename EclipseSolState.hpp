#ifndef ECLIPSE_SOL_STATE_HPP
#define ECLIPSE_SOL_STATE_HPP

#include "EclipseIncludes.hpp"

#include <memory>
#include <string>
#include <vector>

class EclipseSolState
{
    public:
        EclipseSolState(Map* map);
        ~EclipseSolState() = default;

        bool Initialize();
        bool IsInitialized() const;

        void RunScripts();

        sol::state& GetState() { return _solState; }
        const sol::state& GetState() const { return _solState; }

        const Map* GetMap() const { return _map; }

    private:
        Map* const _map;
        sol::state _solState;
        bool _isInitialized;
};

#endif // ECLIPSE_SOL_STATE_HPP