#include <Geode/Geode.hpp>
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/DialogLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <asp/fs.hpp>

#include "utils.hpp"
#include "CustomAlert.hpp"
#include "CustomTextbox.hpp"
#include "CustomChest.hpp"

#include "KeyPicker/Keybind.hpp"
#include "KeyPicker/KeyPickerPopup.hpp"

using namespace geode::prelude;

std::string queuedPopup = "";
std::string queuedChest = "";
std::string queuedTextbox = "";

std::string showKey = "to-show";
std::string altShowKey = "to-show-alt";

// ===== //

// Create files if they don't exist
void checkMissingFiles() {
	Mod::get()->getConfigDir(); // create config folder if missing
    if (!fs::exists(CustomAlert::jsonFilePath))		(void)asp::fs::copy(Mod::get()->getResourcesDir() / CustomAlert::jsonFileName, CustomAlert::jsonFilePath);
	if (!fs::exists(CustomTextbox::jsonFilePath))	(void)asp::fs::copy(Mod::get()->getResourcesDir() / CustomTextbox::jsonFileName, CustomTextbox::jsonFilePath);
	if (!fs::exists(CustomChest::jsonFilePath))		(void)asp::fs::copy(Mod::get()->getResourcesDir() / CustomChest::jsonFileName, CustomChest::jsonFilePath);
}

void clearQueue() {
	queuedPopup = "";
	queuedTextbox = "";
	queuedChest = "";
}

void playQueueSound(bool lower = false) {
	if (!Mod::get()->getSettingValue<bool>("shiftSound")) return;
	FMODAudioEngine::sharedEngine()->playEffect("counter003.ogg", lower ? 0.7f : 1.0f, 1, 0.8f);
}

std::string getTargetID(std::string path, bool alt) {
    if (!fs::exists(path)) {
		checkMissingFiles();
    }

	std::string err = "";
	auto rawJSON = readJSON(path, err);
    if (rawJSON == nullptr) return "";

	auto k = alt ? altShowKey : showKey;
	auto toShow = rawJSON[k];

	if (!toShow.isString()) {
		FLAlertLayer::create("Custom Textboxes", fmt::format("The <cy>\"{}\"</c> key appears to be missing from your JSON!", k), "OK")->show();
		return "_";
	}

	return toShow.asString().unwrapOr("_");
}

// ===== //

enumKeyCodes getPopupKey()   { return Mod::get()->getSettingValue<CTKeybind>("key_popup").getKey(); };
enumKeyCodes getTextboxKey() { return Mod::get()->getSettingValue<CTKeybind>("key_dialogue").getKey(); };
enumKeyCodes getChestKey()   { return Mod::get()->getSettingValue<CTKeybind>("key_chest").getKey(); };

// ===== //

void runPopup(std::string id) {
	(new CustomAlert())->showPopup(id);
	clearQueue();
}

void runTextbox(std::string id) {
	(new CustomTextbox())->showTextbox(id);
	clearQueue();
}

void runChest(std::string id) {
	(new CustomChest())->showChest(id);
	clearQueue();
}

// ===== //

// If keybind selector is open, don't show
// Would be great if this worked with the custom keybinds menu too but it has no node ids
bool dontShowPopupHere() {
	return CCScene::get() == nullptr || CCScene::get()->getChildByType<KeyPickerPopup>(0) != nullptr;
}

bool getModifier(std::string key) {
	auto val = Mod::get()->getSettingValue<std::string>(key);
	if (val == "Shift") return CCKeyboardDispatcher::get()->getShiftKeyPressed();
	if (val == "Alt") return CCKeyboardDispatcher::get()->getAltKeyPressed();
	if (val == "Ctrl") return CCKeyboardDispatcher::get()->getControlKeyPressed();
	if (val == "Command") return CCKeyboardDispatcher::get()->getCommandKeyPressed();
	return false;
}

bool getShift() { return getModifier("shiftRebind"); }
bool getAlt() { return getModifier("altRebind"); }

void prepPopup() {
	if (dontShowPopupHere()) return;
	auto id = getTargetID(CustomAlert::jsonFilePath, getAlt());
	if (id == "_") return;
	if (getShift()) {
		queuedPopup = id;
		playQueueSound();
	}
	else runPopup(id);
}

void prepTextbox() {
	if (dontShowPopupHere()) return;
	auto id = getTargetID(CustomTextbox::jsonFilePath, getAlt());
	if (id == "_") return;
	if (getShift()) {
		queuedTextbox = id;
		playQueueSound();
	}
	else runTextbox(id);
}

void prepChest() {
	if (dontShowPopupHere()) return;
	auto id = getTargetID(CustomChest::jsonFilePath, getAlt());
	if (id == "_") return;
	if (getShift()) {
		queuedChest = id;
		playQueueSound();
	}
	else runChest(id);
}

// Get rid of all active alerts
bool killAllAlerts() {
	bool killed = false;
	auto scene = CCScene::get();
	if (!scene) return false;
	auto children = scene->getChildren();
	for (int i = children->count() - 1; i >= 0; i--) {
		CCNode* n = typeinfo_cast<CCNode*>(children->objectAtIndex(i));
		if (n != nullptr && std::string(n->getID()).starts_with(""_spr)) {
			n->removeMeAndCleanup();
			killed = true;
		}
	} 

	// Clear queue
	
	if (!killed && (!queuedChest.empty() || !queuedPopup.empty() || !queuedTextbox.empty())) {
		killed = true;
		playQueueSound(true);
	}
	clearQueue(); // to be safe
	
	return killed;
}


// Handle key presses
class $modify(CCKeyboardDispatcher) {
	bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool repeat, double time) {
		if (repeat || !down || key == KEY_None || key == KEY_Unknown) return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat, time);

		else if (key == KEY_Escape && CCKeyboardDispatcher::get()->getShiftKeyPressed()) {
			if (killAllAlerts()) return false;
		}

		else if (key == getPopupKey()) prepPopup();
		else if (key == getTextboxKey()) prepTextbox();
		else if (key == getChestKey()) prepChest();
		
		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat, time);
	}
};

bool playQueue() {
	if (queuedPopup != "") {
		runPopup(queuedPopup);
		return true;
	}
	else if (queuedTextbox != "") {
		runTextbox(queuedTextbox);
		return true;
	}
	else if (queuedChest != "") {
		runChest(queuedChest);
		return true;
	}
	return false;
}

// Button queueing
class $modify(CCMenuItemSpriteExtra) {
	void activate() {
		if (!playQueue()) CCMenuItemSpriteExtra::activate();
	}
};

// Run queued textbox on death (it's funny, why not)
class $modify(PlayLayer) {
	void destroyPlayer(PlayerObject* player, GameObject* obj) {
		PlayLayer::destroyPlayer(player, obj);
		if (obj != m_anticheatSpike && Mod::get()->getSettingValue<bool>("onDeath")) playQueue();
	}
};

$on_mod(Loaded) {
	checkMissingFiles();
}
