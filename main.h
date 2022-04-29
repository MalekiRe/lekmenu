//
// Created by malek on 4/26/22.
//
#include <stardustxr/fusion/types/drawable/text.hpp>
#pragma once
static StardustXRFusion::TextBuilder autoCompleteTextBuilder = StardustXRFusion::TextBuilder()
        .characterHeight(0.05f)
        .origin(0, -0.1, 0);
static std::vector<StardustXRFusion::Text *> autocompleteObjects;

