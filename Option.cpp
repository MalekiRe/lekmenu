//
// Created by malek on 4/28/22.
//

#include "Option.h"

#include <utility>
void Option::doSomethingToText(const std::function<void(std::string&)>& func) {
    func(textString);
    text.setText(textString);
}

Option::Option(Spatial* parent, std::string str, SKMath::vec3 pos, SKMath::vec3 offsetPos) : text(TextBuilder().parent(parent).text(str).characterHeight(0.05f).build()){
    this->textString = str;
    this->origin = pos;
    this->offsetOrigin = offsetPos;
    text.setOrigin(origin);
}

void Option::setUnselected() {
    selected = false;
    doSomethingToText([](std::string &str){
        str = str.substr(1, str.size()-1);
    });
    text.setOrigin(origin);
}

bool Option::isSelected() const {
    return selected;
}

void Option::setSelected() {
    selected = true;
    doSomethingToText([](std::string &str){
        str.insert(0, ">");
    });
    text.setOrigin(offsetOrigin);
}

Text *Option::getText() {
    return &text;
}

std::string Option::getString() {
    return textString;
}
