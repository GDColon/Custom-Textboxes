#pragma once
#include <Geode/Geode.hpp>
#include <matjson.hpp>
#include <regex>

#include "CustomAlert.hpp"
#include "CustomChest.hpp"
#include "CustomTextbox.hpp"

using namespace geode::prelude;

struct IconData {
    int id;
    int type;
};

struct SoundData {
    std::string name;
    float volume = 1.0f;
    float speed = 1.0f;
};

enum Toggler {
    Default, Off, On
};

inline std::map<std::string, int> TEXTBOX_COLORS = {
    { "brown", 1 },
    { "blue", 2 },
    { "green", 3 },
    { "purple", 4 },
    { "grey", 5 },
    { "gray", 5 },
    { "lightgrey", 6 },
    { "lightgray", 6 },
    { "transparent", 7 }
};

inline std::map<std::string, std::string> EXTRA_TEXTBOXES = {
    { "fancy", "square01_001.png" },
    { "black", "square02_001.png" },
    { "white", "square02b_001.png" },
    { "whiteoutline", "square02c_001.png" },
    { "roundgrey", "GJ_squareB_01.png" },
    { "roundgray", "GJ_squareB_01.png" },
    { "cyan", "GJ_button_02.png" },
    { "red", "GJ_button_06.png" },
    { "red", "GJ_button_06.png" },
    { "darkpurple", "geode.loader/GE_square01.png" },
    { "darkblue", "geode.loader/GE_square02.png" },
    { "geode", "geode.loader/GE_square03.png" }
};

inline std::map<std::string, int> ICON_TYPES = {
    { "cube", 1 },
    { "color", 2 },
    { "colour", 2 },
    { "color1", 2 },
    { "colour1", 2 },
    { "color2", 3 },
    { "colour2", 3 },
    { "ship", 4 },
    { "ball", 5 },
    { "ufo", 6 },
    { "UFO", 6 },
    { "bird", 6 },
    { "wave", 7 },
    { "dart", 7 },
    { "robot", 8 },
    { "spider", 9 },
    { "trail", 10 },
    { "streak", 10 },
    { "deathEffect", 11 },
    { "deatheffect", 11 },
    { "death", 11 },
    { "item", 12 },
    { "swing", 13 },
    { "jetpack", 14 },
    { "shipFire", 15 },
    { "shipfire", 15 }
};

inline std::map<std::string, int> ITEM_TYPES = {
    { "blueKey", 1 },
    { "greenKey", 2 },
    { "orangeKey", 3 },
    { "masterEmblem", 4 },
    { "purpleKey", 5 },
    { "firePath", 6 },
    { "icePath", 7 },
    { "poisonPath", 7 },
    { "shadowPath", 8 },
    { "lavaPath", 9 },
    { "earthPath", 10 },
    { "bloodPath", 11 },
    { "metalPath", 12 },
    { "lightPath", 13 },
    { "soulPath", 14 },
    { "musicCustomizer", 15 },
    { "musicUnlocker", 16 },
    { "robotAnimationSlow", 17 },
    { "robotAnimationFast", 18 },
    { "spiderAnimationFast", 19 },
};

inline std::regex errorPattern(R"(>:(\d+):(\d+)$)");

inline matjson::Value readJSON(std::filesystem::path path, std::string& err) {
	std::ifstream jsonFile(path);
    auto parsed = matjson::parse(jsonFile);
    if (!parsed) {
        err = parsed.unwrapErr();
        geode::log::error("{}", err);
        return nullptr;
    }
    else return parsed.unwrap();
}

// Read JSON or display an error popup
inline matjson::Value getTextboxByID(std::filesystem::path path, std::filesystem::path shortPath, std::string id, std::string typeStr) {
    std::string err;
    auto rawJSON = readJSON(path, err);
    if (rawJSON == nullptr) {
        err = std::regex_replace(err, errorPattern, "line $1, position $2");
        FLAlertLayer::create("Custom Textboxes", fmt::format("Failed to load <cy>{}</c>\n\nError: <cr>{}</c>", shortPath, err), "OK")->show();
        return nullptr;
    }
    
    if (!rawJSON.contains(id)) {
        if (Mod::get()->getSettingValue<bool>("missingIDWarn")) FLAlertLayer::create("Custom Textboxes", fmt::format("{} ID not found!\nID: <cy>{}</c> ", typeStr, id), "OK")->show();
        return nullptr;
    };
    
    return rawJSON[id];
}

inline std::string getStr(matjson::Value data, char const* key, char const* fallback) {
    if (!data.contains(key)) return fallback;
    else return data[key].asString().unwrapOr(fallback);
}

inline std::string getStr(matjson::Value data, char const* key) {
    return data[key].asString().unwrap();
}

inline double getNum(matjson::Value data, char const* key, double fallback) {
    if (!data.contains(key)) return fallback;
    else return data[key].asDouble().unwrapOr(fallback);
}

inline int getInt(matjson::Value data, char const* key, int fallback) {
    if (!data.contains(key)) return fallback;
    else return data[key].asInt().unwrapOr(fallback);
}

inline int getInt(matjson::Value data, char const* key) {
    return data[key].asInt().unwrap();
}

inline bool getBool(matjson::Value data, char const* key, bool fallback = false) {
    if (!data.contains(key)) return fallback;
    else return data[key].asBool().unwrapOr(fallback);
}

inline int getKey(matjson::Value data, char const* key, std::map<std::string, int> map, int fallback) {
    if (!data.contains(key)) return fallback;

    if (data[key].isString()) {
        auto k = data[key].asString().unwrap();
        if (map.contains(k)) return map[k];
        else return fallback;
    }

    return data[key].asInt().unwrapOr(fallback);
}

inline std::string getKey(matjson::Value data, char const* key, std::map<std::string, std::string> map, std::string fallback = "") {
    if (!data.contains(key) || !data[key].isString()) return fallback;

    auto k = data[key].asString().unwrap();
    if (map.contains(k)) return map[k];
    else return fallback;
}

inline ccColor3B getColor(matjson::Value data, char const* key = "color", ccColor3B fallback = ccWHITE) {
    ccColor3B col = fallback;
    auto arr = data[key].asArray();
    if (arr) {
        auto x = arr.unwrap();
        col.r = x[0].asInt().unwrapOr(0);
        col.g = x[1].asInt().unwrapOr(0);
        col.b = x[2].asInt().unwrapOr(0);
    }
    return col;
}

// Gets a custom texture from both spritesheets and the resources folder
inline CCSprite* getCustomTexture(char const* frame, char const* fallback, bool fallbackIsResource = false) {
    // Search spritesheets
    CCSprite* customTexture = CCSprite::createWithSpriteFrameName(frame);
    if (customTexture != nullptr && !customTexture->isUsingFallback()) return customTexture;

    // Search resources
    CCSprite* customResource = CCSprite::create(frame);
    if (customResource != nullptr && !customResource->isUsingFallback()) return customResource;

    // Return fallback for borders
    if (fallbackIsResource) return CCSprite::create(fallback);

    // Return texture loader null sprite if it exists
    if (customTexture != nullptr || customResource != nullptr) return (customTexture != nullptr) ? customTexture : customResource;

    // Return fallback
    return CCSprite::createWithSpriteFrameName(fallback);
}

inline void set9SpriteFrame(CCScale9Sprite* slice, CCSprite* sprite) {
    CCSize bgSize = slice->getContentSize();
    slice->setSpriteFrame(sprite->displayFrame());
    slice->setContentSize(bgSize);
}

inline void setBorder(CCLayer* layer, matjson::Value data, int fallback = 1, char const* key = "background") {
    if (data.contains(key) && layer) {
        auto slice = layer->getChildByType<CCScale9Sprite>(0);
        if (!slice) return;

        std::string bg = getKey(data, key, EXTRA_TEXTBOXES);

        if (bg.empty()) {
            int borderID = getKey(data, key, TEXTBOX_COLORS, -1);

            // Custom
            if (borderID == -1 && data[key].isString()) {
                bg = data[key].asString().unwrapOr("");
            }

            else bg = fmt::format("GJ_square0{}.png", std::clamp(borderID, 1, 7));
        }
    
        set9SpriteFrame(slice, getCustomTexture(bg.c_str(), fmt::format("GJ_square0{}.png", fallback).c_str(), true));
    }
}

// hack
inline void setBorderFromData(CCLayer* layer, matjson::Value data, int fallback = 1) {
    matjson::Value blank = matjson::Value();
    blank.set("background", data);
    setBorder(layer, blank, fallback, "background");
}

inline IconData getIcon(matjson::Value data, char const* key) {
    if (data.contains(key)) {

        matjson::Value icon = data[key];

        if (icon.isNumber()) {
            int id = data[key].asInt().unwrapOr(1);
            return { id, 1 };
        }

        else if (icon.isObject()) {
            int iconID = getKey(icon, "id", ITEM_TYPES, 1);
            int iconType = getKey(icon, "type", ICON_TYPES, 1);
            return { iconID, iconType };
        }

    }
    
    return {1, 1};
}

inline SoundData getCustomSound(matjson::Value data) {
    if (data.isArray()) {
        auto arr = data.asArray().unwrap();
        if (arr.size() == 1) return getCustomSound(arr[0]);
        else if (arr.size() > 1) {
            auto val = arr[rand() % arr.size()];
            if (val != nullptr && !val.isArray()) return getCustomSound(val);
        }
    }

    float vol = 1.0f;
    float speed = 0.0f;
    std::string sound;

    if (data.isString()) {
        sound = data.asString().unwrap();
    }

    else if (data.isObject()) {
        sound = getStr(data, "name", "");
        vol = getNum(data, "volume", vol);
        speed = getNum(data, "speed", speed);
        speed = std::pow(2.0f, speed / 12.0f); // Convert to semitones
    }

    return { sound, vol, speed };
}

inline void playCustomSound(matjson::Value data) {
    SoundData snd = getCustomSound(data);
    FMODAudioEngine::sharedEngine()->playEffect(snd.name, snd.speed, 1.0f, snd.volume);
}

inline void playCustomSound(SoundData snd) {
    FMODAudioEngine::sharedEngine()->playEffect(snd.name, snd.speed, 1.0f, snd.volume);
}