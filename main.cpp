#include <stardustxr/fusion/fusion.hpp>
#include <stardustxr/fusion/types/drawable/model.hpp>
#include <stardustxr/fusion/types/spatial/spatial.hpp>
#include <stardustxr/fusion/types/drawable/text.hpp>
#include <unistd.h>
#include <stardustxr/fusion/types/data/receiver.hpp>
#include <stardustxr/fusion/types/fields/boxfield.hpp>
#include <stardustxr/fusion/types/input/inputactionhandler.hpp>
#include <iostream>
#include "keyboardhandler.h"
#include "main.h"
#include "Option.h"
#include "ApplicationPathHandler.h"
#include <stardustxr/fusion/types/drawable/text.h>
#include <stardustxr/fusion/types/spatial/spatial.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>

using namespace StardustXRFusion;
using namespace SKMath;
using namespace std;
StardustXRFusion::Text *mainText;
size_t selectedOption = -1;
vector<std::string> getMatchingFiles(const string& strToCmp, vector<string>& files) {
    vector<string> matching;
    for(string &file : files) {
        if(strncmp(strToCmp.c_str(), file.c_str(), strToCmp.length()) == 0) {
            matching.push_back(file);
        }
    }
    return matching;
}
void adjustList(vector<Option*>& options, string &app) {
    for(auto &option : options) {
        delete option;
    }
    options.clear();
    selectedOption = -1;
    if(app.length() <= 1) {
        return;
    }
    vector<string> matchingPaths = getMatchingFiles(app, ApplicationPathHandler::getAppsOnPath());
    for(size_t i = 0; i < matchingPaths.size(); i++) {
        auto *option = new Option(mainText, matchingPaths[i], vec3{0, -0.1f*(float)(i+1), 0}, vec3{0.1f, -0.1f*(float)(i+1), 0});
        if(i == 0) {
            option->setSelected();
            selectedOption = 0;
        }
        options.push_back(option);
    }
}
int main() {
    if(!StardustXRFusion::Setup())
        return -1;
    Spatial root = Spatial(Root());
    std::string mainString = "hi";
    vector<Option*> options;
    mainText = TextBuilder()
            .text(mainString)
            .characterHeight(0.1f)
            .origin(-0.1, 0, -0.1)
            .orientation(0, 180, 0)
            .parent(&root)
            .buildRef();
    autoCompleteTextBuilder.parent(mainText);
    Spatial screenCenter = SpatialBuilder()
            .parent(&root)
            .origin(vec3{0, 0, -0.1})
            .build();
    BoxField inputField = BoxFieldBuilder()
            .parent(&screenCenter)
            .origin(vec3_zero)
            .orientation(quat_identity)
            .size(vec3{0.4f, 0.312254f, 0.022191f})
            .build();
    PulseReceiver keyReceiver(&screenCenter, inputField);
    keyReceiver.setMask([&](flexbuffers::Builder &fbb) {
        fbb.String("type", "keyboard");
    });
    auto *keyboardHandler = new KeyboardHandler();
    bool isHighlighted = false;
    keyboardHandler->everyPress = [&isHighlighted, &mainString]() {
        if(!isHighlighted) {
            mainText->setColor(SKMath::color_from_hsva(0, 0, 100, 1));
        } else {
            mainText->setColor(SKMath::color_from_hsva(240, 100, 100, 1));
        }
        mainText->setText(mainString);
    };
    keyboardHandler->keyboardFunctionMap[{XKB_KEY_Down}] = [&options](xkb_state* state) {
        options[selectedOption]->setUnselected();
        if(selectedOption < options.size()) {
            selectedOption++;
        }
        options[selectedOption]->setSelected();
    };
    keyboardHandler->keyboardFunctionMap[{XKB_KEY_Up}] = [&options](xkb_state* state) {
        options[selectedOption]->setUnselected();
        if(selectedOption != 0) {
            selectedOption--;
        }
        options[selectedOption]->setSelected();
    };
    keyboardHandler->keyboardFunctionMap[{65293}] = [&options](xkb_state* state) {
        enum { MAX_ARGS = 64 };
        char *args[MAX_ARGS];
        char **next = args;
        options[selectedOption]->setUnselected();
        char *temp = strtok(strdup(options[selectedOption]->getString().c_str()), " ");
        while (temp != nullptr)
        {
            *next++ = temp;
            printf("%s\n", temp);
            temp = strtok(nullptr, " ");
        }
        *next = nullptr;
        setenv("GDK_BACKEND", "wayland", true);
        setenv("QT_QPA_PLATFORM", "wayland", true);
        setenv("DISPLAY", ":1", true);
        StardustXRFusion::Shutdown();
        execvp(args[0], args);
        exit(0);
    };
    keyboardHandler->keyboardFunctionMap[{XKB_KEY_Control_R, XKB_KEY_a}] = [&isHighlighted](xkb_state* state){
        isHighlighted = true;
    };
    keyboardHandler->keyboardFunctionMap[{XKB_KEY_BackSpace}] = [&options, &mainString](xkb_state* state) {
        if(mainString.length() != 0) {
            mainString.pop_back();
        }
        adjustList(options, mainString);
    };
    keyboardHandler->keyboardOnUTFKeyTyping = [&mainString, &options](char *buf) {
       mainString.append(buf);
        adjustList(options, mainString);
    };
    keyboardHandler->setPulseReceiver(keyReceiver);

    StardustXRFusion::RunEventLoop();
    return 0;
}
