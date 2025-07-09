#include <Geode/Geode.hpp>
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/DialogLayer.hpp>

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
	std::error_code ec;
    if (!fs::exists(CustomAlert::jsonFilePath))		fs::copy_file(Mod::get()->getResourcesDir() / CustomAlert::jsonFileName, CustomAlert::jsonFilePath, ec);
	if (!fs::exists(CustomTextbox::jsonFilePath))	fs::copy_file(Mod::get()->getResourcesDir() / CustomTextbox::jsonFileName, CustomTextbox::jsonFilePath, ec);
	if (!fs::exists(CustomChest::jsonFilePath))		fs::copy_file(Mod::get()->getResourcesDir() / CustomChest::jsonFileName, CustomChest::jsonFilePath, ec);
	if (ec) { log::error("Filesystem error: {}", ec.message()); }
}

void clearQueue() {
	queuedPopup = "";
	queuedTextbox = "";
	queuedChest = "";
}

void playQueueSound() {
	if (!Mod::get()->getSettingValue<bool>("shiftSound")) return;
	FMODAudioEngine::sharedEngine()->playEffect("counter003.ogg");
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

enumKeyCodes getPopupKey()   { return Mod::get()->getSettingValue<Keybind>("key_popup").getKey(); };
enumKeyCodes getTextboxKey() { return Mod::get()->getSettingValue<Keybind>("key_dialogue").getKey(); };
enumKeyCodes getChestKey()   { return Mod::get()->getSettingValue<Keybind>("key_chest").getKey(); };

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

bool getShift() { return CCKeyboardDispatcher::get()->getShiftKeyPressed(); }
bool getAlt() { 
	if (Mod::get()->getSettingValue<bool>("ctrl")) {
		return CCKeyboardDispatcher::get()->getControlKeyPressed();
	}
	else {
		return CCKeyboardDispatcher::get()->getAltKeyPressed();
	}
}

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
		if (n != nullptr && n->getID().starts_with(""_spr)) {
			n->removeMeAndCleanup();
			killed = true;
		}
	} 
	return killed;
}

// Button queueing
class $modify(CCMenuItemSpriteExtra) {
	void activate() {
		if (queuedPopup != "") {
			runPopup(queuedPopup);
		}
		else if (queuedTextbox != "") {
			runTextbox(queuedTextbox);
		}
		else if (queuedChest != "") {
			runChest(queuedChest);
		}
		else {
			CCMenuItemSpriteExtra::activate();
		}
	}
};

class $modify(CCKeyboardDispatcher) {
	bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool repeat) {
		if (repeat || !down || key == KEY_None || key == KEY_Unknown) return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat);

		else if (key == KEY_Escape && getShift()) {
			if (killAllAlerts()) return false;
		}

		else if (key == getPopupKey()) prepPopup();
		else if (key == getTextboxKey()) prepTextbox();
		else if (key == getChestKey()) prepChest();
		
		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat);
	}
};

$on_mod(Loaded) {
	checkMissingFiles();
}