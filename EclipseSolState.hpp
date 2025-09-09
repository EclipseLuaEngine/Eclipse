#ifndef ECLIPSE_SOL_STATE_HPP
#define ECLIPSE_SOL_STATE_HPP

#include "EclipseIncludes.hpp"

#include <memory>
#include <string>
#include <vector>

class EclipseSolState
{
    public:
        EclipseSolState();
        ~EclipseSolState() = default;

        bool Initialize(int32 mapId = -1);
        bool IsInitialized() const;

        int32 GetStateMapId() const { return _stateMapId; }
        sol::state& GetState() { return _solState; }
        const sol::state& GetState() const { return _solState; }

    private:
        int32 _stateMapId;
        sol::state _solState;
        bool _isInitialized;
};

#endif // ECLIPSE_SOL_STATE_HPP