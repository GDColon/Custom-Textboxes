// custom keybinds mod doesn't support ignoring modifier keys like shift and alt
// so i have to code an entirely new key picker

#include <Geode/loader/SettingV3.hpp>
#include <Geode/loader/Mod.hpp>

#include "Keybind.hpp"
#include "KeyPickerPopup.hpp"

using namespace geode::prelude;

class KeybindNode : public SettingValueNodeV3<KeyPicker> {
protected:
    ButtonSprite* m_buttonSprite;
    CCMenuItemSpriteExtra* m_button;
    CCLabelBMFont* m_label;

    bool init(std::shared_ptr<KeyPicker> setting, float width) {
        if (!SettingValueNodeV3::init(setting, width)) return false;
            
        m_buttonSprite = ButtonSprite::create("", 120, 120, 0.8f, false, "goldFont.fnt", "geode.loader/GE_button_05.png", 30);
        m_buttonSprite->setScale(0.6f);
        m_button = CCMenuItemSpriteExtra::create(
            m_buttonSprite, this, menu_selector(KeybindNode::onChooseKey)
        );
        m_label = m_buttonSprite->getChildByType<CCLabelBMFont>(0);

        auto menu = CCMenu::create();
        menu->addChild(m_button);

        this->getButtonMenu()->addChildAtPosition(menu, Anchor::Right, {-42, 0});
        this->getButtonMenu()->setContentWidth(120);
        this->getButtonMenu()->updateLayout();

        updateButtonLabel();

        this->updateState(nullptr);
        
        return true;
    }

    enumKeyCodes getSelectedKey() {
        return static_cast<enumKeyCodes>(this->getSetting()->getValue());
    }

    void onChooseKey(CCObject*) {
        KeyPickerPopup::create(getSelectedKey(), [this](enumKeyCodes selected) {
            this->getSetting()->setValue(int(selected));
            updateButtonLabel();
        })->show();
    }

    void updateButtonLabel() {
        m_label->setString(keyToString(getSelectedKey()).c_str());
        m_label->limitLabelWidth(100, 1.0f, 0.1f);
        this->updateState(nullptr);

        // gotta do this manually because i dont even know
        auto resetButton = this->getNameMenu()->getChildByType<CCMenuItemSpriteExtra>(1);
        if (resetButton) resetButton->setVisible(!this->getSetting()->isDefaultValue());
        if (this->getNameMenu()->getLayout()) this->getNameMenu()->updateLayout();
    }

    void onCommit() override {}

    void onResetToDefault() override {
        SettingValueNodeV3::onResetToDefault();
        updateButtonLabel();
    }

public:
    static KeybindNode* create(std::shared_ptr<KeyPicker> setting, float width) {
        auto ret = new KeybindNode();
        if (ret && ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    std::shared_ptr<KeyPicker> getSetting() const {
        return std::static_pointer_cast<KeyPicker>(SettingNodeV3::getSetting());
    }

    bool hasUncommittedChanges() const override {
        return false;
    }
};

SettingNodeV3* KeyPicker::createNode(float width) {
    return KeybindNode::create(
        std::static_pointer_cast<KeyPicker>(shared_from_this()),
        width
    );
}

$execute {
    (void)Mod::get()->registerCustomSettingType("keybind", &KeyPicker::parse);
}