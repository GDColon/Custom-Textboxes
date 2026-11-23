#pragma once

#include <filesystem>
#include <functional>
#include <fstream>
#include <sstream>

#include <Geode/Geode.hpp>
#include <matjson.hpp>

namespace fs = std::filesystem;
using namespace geode::prelude;

class CustomAlert {
public:
    void showPopup(std::string id);

    inline static std::string jsonFileName = "custom_popups.json";
    inline static std::string jsonFilePath = fmt::format("{}/{}", Mod::get()->getConfigDir(), jsonFileName);

private:
    std::map<std::string, int> COMMENT_TYPES = {
        { "level", 0 },
        { "profile", 1 },
        { "account", 1 },
        { "friend", 2 },
        { "list", 4 },
        { "listname", 5 },
        { "note", 9 }
    };
};

std::string getText(matjson::Value data, std::string fallback);

void customizeText(CCLabelBMFont* text, matjson::Value data);
void customizeText(TextArea* text, matjson::Value data);

void createCloseButton(FLAlertLayer* parent, matjson::Value data, CCPoint position);
void customizeButton(CCMenuItemSpriteExtra* button, CCObject* parent, matjson::Value data, std::string fallback);
void customizeIcon(CCSprite* icon, matjson::Value data);

void setupCustomButtons(FLAlertLayer* layer, matjson::Value data);
void createExtraButton(FLAlertLayer* layer, matjson::Value data);

void setupCustomTexture(CCLayer* layer, matjson::Value data);

inline std::map<std::string, int> BUTTON_COLORS = {
    { "green", 1 },
    { "blue", 2 },
    { "cyan", 2 },
    { "aqua", 2 },
    { "pink", 3 },
    { "magenta", 3 },
    { "grey", 4 },
    { "gray", 4 },
    { "darkgrey", 5 },
    { "darkgray", 5 },
    { "red", 6 }
};

inline std::map<std::string, std::string> EXTRA_BUTTON_COLORS = {
    { "geodeblue", "geode.loader/GE_button_01.png" },
    { "geodepink", "geode.loader/GE_button_02.png" },
    { "geodepurple", "geode.loader/GE_button_03.png" },
    { "geodeblack", "geode.loader/GE_button_05.png" },
    { "trans", "geode.loader/GE_button_04.png" }
};  