//
// Created by malek on 4/24/22.
//

#include "keyboardhandler.h"
#include <xkbcommon/xkbcommon.h>
#include <cstring>
#include <csignal>
#include <utility>
#include <vector>
#include <iostream>
#include <stardustxr/fusion/fusion.hpp>
#include "ApplicationPathHandler.h"
#include "main.h"
#include <algorithm>
using namespace std;
KeyboardHandler::KeyboardHandler() {
    this->context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
}

void KeyboardHandler::setKeymap(const char *key_map) {
    this->keymap = xkb_keymap_new_from_string(context, key_map, XKB_KEYMAP_FORMAT_TEXT_V1,XKB_KEYMAP_COMPILE_NO_FLAGS);
    this->state = xkb_state_new(keymap);
}

void KeyboardHandler::updateStateMask(xkb_mod_mask_t depressed_mods,
                                      xkb_mod_mask_t latched_mods,
                                      xkb_mod_mask_t locked_mods, xkb_layout_index_t group) const {
    xkb_state_update_mask(state, depressed_mods, latched_mods, locked_mods, 0, 0, group);
}
void permutation(const vector<int> &v, vector<int> v2, const std::function<void(vector<int>)>& function) {
    vector<int> vCopy(v.begin(), v.end());
    if(v.empty()) {
        function(std::move(v2));
    } else {
        int item = vCopy.back();
        vCopy.pop_back();
        permutation(vCopy, v2, function);
        v2.push_back(item);
        permutation(vCopy, v2, function);
    }
}
bool KeyboardHandler::runFunctions() {
    bool matched = false;
    permutation(keyboardStates, vector<int>(), [&, this](const vector<int>& v2){
        if(this->keyboardFunctionMap[v2] != nullptr) {
            matched = true;
            this->keyboardFunctionMap[v2](this->state);
        }
    });
    return matched;
}


void KeyboardHandler::updateKeyPress(unsigned int scancode, bool removeKey) {
    const int *mybuf = static_cast<const int *>(malloc(128*sizeof(int)));
    int number = xkb_state_key_get_syms(state, scancode, reinterpret_cast<const xkb_keysym_t **>(&mybuf));
    if(removeKey) {
        for(int i = 0; i < number; i++) {
            keyboardStates.erase(std::remove(keyboardStates.begin(), keyboardStates.end(), mybuf[i]), keyboardStates.end());
        }
    } else {
        for (int i = 0; i < number; i++) {
            this->keyboardStates.push_back(mybuf[i]);
        }
        if(!runFunctions()) {
            char buf[129];
            buf[128] = '\0';
            xkb_state_key_get_utf8(state, scancode, buf, sizeof(buf)-1);
            keyboardOnUTFKeyTyping(buf);
        }
    }
    everyPress();

}

void KeyboardHandler::setPulseReceiver(StardustXRFusion::PulseReceiver &receiver) {
    receiver.onDataReceived =  [this](const std::string& senderUUID, flexbuffers::Reference ref) {
        if (ref.IsMap()) {
            flexbuffers::Map map = ref.AsMap();
            if(map["keymap"].IsString()) {
                this->setKeymap(map["keymap"].AsString().c_str());
            }
            if (map["pressed"].IsUInt()) {
                this->updateKeyPress(map["pressed"].AsUInt32(), false);
            }
            if (map["released"].IsUInt()) {
                this->updateKeyPress(map["released"].AsUInt32(), true);
            }
            if(map["modifiers"].IsMap()) {
                flexbuffers::Map modMap = map["modifiers"].AsMap();
                this->updateStateMask(
                        modMap["depressed"].AsUInt32(),
                        modMap["latched"].AsUInt32(),
                        modMap["locked"].AsUInt32(),
                        modMap["group"].AsUInt32());
            }
        }
    };
}
