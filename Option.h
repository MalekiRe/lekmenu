//
// Created by malek on 4/28/22.
//
#pragma once
#include <stardustxr/fusion/fusion.hpp>
#include <stardustxr/fusion/types/drawable/text.hpp>
#define OFFSET_ORIGIN 1.f
using namespace StardustXRFusion;
class Option {
private:

    Text text;
    std::string textString;
    bool selected = false;
    SKMath::vec3 origin{};
    SKMath::vec3 offsetOrigin{};
public:
    Option(Spatial *parent, std::string str, SKMath::vec3 pos, SKMath::vec3 offsetPos);
    void doSomethingToText(const std::function<void(std::string&)>& func);
    void setSelected();
    bool isSelected() const;
    void setUnselected();
    Text *getText();
    std::string getString();
};
