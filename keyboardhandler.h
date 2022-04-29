//
// Created by malek on 4/24/22.
//
#include <xkbcommon/xkbcommon.h>
#include <string>
#include <vector>
#include <map>
#include <stardustxr/fusion/types/drawable/text.hpp>
#include <stardustxr/fusion/types/data/receiver.hpp>

#pragma once

class KeyboardHandler {
private:
    bool runFunctions();
public:
    std::vector<int> keyboardStates;
    std::map<std::vector<int>, std::function<void(struct xkb_state*)>> keyboardFunctionMap;
    std::function<void(char*)> keyboardOnUTFKeyTyping;
    std::function<void(void)> everyPress;
    struct xkb_state *state;
    struct xkb_context *context;
    struct xkb_keymap *keymap;
    KeyboardHandler();
    void setKeymap(const char *keymap);

    void updateStateMask(xkb_mod_mask_t depressed_mods, xkb_mod_mask_t latched_mods, xkb_mod_mask_t locked_mods,
                         xkb_layout_index_t group) const;
    void updateKeyPress(unsigned int scancode, bool removeKey);
    void setPulseReceiver(StardustXRFusion::PulseReceiver &receiver);
};
