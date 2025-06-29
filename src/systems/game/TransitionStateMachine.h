//
// Created by ivanr on 18.6.2025 г..
//

#ifndef TRANSITIONSTATEMACHINE_H
#define TRANSITIONSTATEMACHINE_H

#include <unordered_map>

#include "PlayerComponent.h"


class TransitionStateMachine {
public:
    using StateHandler = std::function<void(entt::entity, entt::registry&, float)>;

    TransitionStateMachine();
    void RegisterStateHandler(TransitionState state, StateHandler handler);
    void ProcessEntity(entt::entity entity, entt::registry& registry, float deltaTime);

private:
    std::unordered_map<TransitionState, StateHandler> m_stateHandlers;
};

#endif //TRANSITIONSTATEMACHINE_H
