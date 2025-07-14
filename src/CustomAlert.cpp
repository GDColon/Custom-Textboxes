#include "utils.hpp"
#include "Callback.hpp"

#include "CustomAlert.hpp"

void CustomAlert::showPopup(std::string id)
{
    auto data = getTextboxByID(CustomAlert::jsonFilePath, CustomAlert::jsonFileName, id, "Popup");
    if (data == nullptr) return;

    std::string alertType = getStr(data, "type", "alert");

    std::string title = getStr(data, "title", "");

    bool hasContent = data.contains("content") || data.contains("text");
    matjson::Value content = data.contains("content") ? data["content"] : data["text"];

    bool hasButton = data.contains("button1") || data.contains("button");
    matjson::Value button1 = hasButton ? (data.contains("button1") ? data["button1"] : data["button"]) : nullptr;

    bool instant = getBool(data, "instant");

    if (data.contains("sound")) playCustomSound(data["sound"]);

    if (alertType == "item" || alertType == "icon" || alertType == "unlock") {
        IconData icon = getIcon(data, "icon");

        ItemInfoPopup* popup = ItemInfoPopup::create(
            icon.id,
            static_cast<UnlockType>(icon.type)
        );
        
        CCLayer* iconLayer = popup->m_mainLayer;

        if (hasContent) {
            TextArea* unlockText = iconLayer->getChildByType<TextArea>(0);
            customizeText(unlockText, content);
        }

        if (data.contains("title")) {
            CCLabelBMFont* unlockHeader = iconLayer->getChildByType<CCLabelBMFont>(0);
            customizeText(unlockHeader, data["title"]);
        }

        if (data.contains("subtitle")) {
            CCLabelBMFont* unlockSubtitle = iconLayer->getChildByType<CCLabelBMFont>(1);
            customizeText(unlockSubtitle, data["subtitle"]);
        }

        if (hasButton) {
            auto okParent = popup->m_buttonMenu->getChildByType<CCMenuItemSpriteExtra>(0);
            customizeButton(okParent, popup, button1, "OK");
        }

        if (data.contains("customTexture")) setupCustomTexture(iconLayer, data);

        createCloseButton(popup, data, {-145, 195});

        setBorder(popup->m_mainLayer, data);

        setupCustomButtons(popup, data);

        customizeIcon(iconLayer->getChildByType<GJItemIcon>(0)->m_player, data);

        popup->m_noElasticity = instant;
        popup->setID("custom_iconpopup"_spr);
        popup->show();
    }

    else if (alertType == "purchase" || alertType == "shop") {
        IconData icon = getIcon(data, "icon");

        int iconType = icon.type;
        GJStoreItem* item = GJStoreItem::create(
            -1, icon.id,
            iconType, getInt(data, "price", 100),
            static_cast<ShopType>(getInt(data, "shopType", 0))
        );

        PurchaseItemPopup* popup = PurchaseItemPopup::create(item);
        
        CCLayer* purchaseLayer = popup->m_mainLayer;

        if (data.contains("content")) {
            TextArea* priceText = purchaseLayer->getChildByType<TextArea>(0);
            customizeText(priceText, content);
        }

        if (data.contains("title")) {
            CCLabelBMFont* buyHeader = purchaseLayer->getChildByType<CCLabelBMFont>(0);
            buyHeader->setString(title.c_str());
        }

        setBorder(popup->m_mainLayer, data, 2);


        // Buttons
        CCMenu* buttonMenu = popup->m_buttonMenu;
        if (hasButton) customizeButton(buttonMenu->getChildByType<CCMenuItemSpriteExtra>(1), popup, button1, "Buy");
        if (data.contains("button2")) customizeButton(buttonMenu->getChildByType<CCMenuItemSpriteExtra>(0), popup, data["button2"], "Cancel");

        if (data.contains("customTexture")) setupCustomTexture(purchaseLayer, data);

        createCloseButton(popup, data, {-124, 87});

        setupCustomButtons(popup, data);

        customizeIcon(purchaseLayer->getChildByType<GJItemIcon>(0)->m_player, data);

        popup->m_noElasticity = instant;
        popup->setID("custom_shoppopup"_spr);
        popup->show();
    }

    else if (alertType == "mod") {
        RateStarsLayer* popup = RateStarsLayer::create(
            12107595, // jajaja
            getBool(data, "platformer", false), // is platformer
            true  // is mod
        );
        CCLayer* modLayer = popup->m_mainLayer;

        if (data.contains("title")) {
            CCLabelBMFont* modHeader = modLayer->getChildByType<CCLabelBMFont>(0);
            customizeText(modHeader, data["title"]);
        }

        setBorder(popup->m_mainLayer, data);

        popup->m_noElasticity = instant;
        popup->setID("custom_modpopup"_spr);
        popup->show();

        popup->m_submitButton->setTarget(popup, menu_selector(RateStarsLayer::onClose));

        if (hasButton) customizeButton(popup->m_submitButton, popup, button1, "Submit");

        setupCustomButtons(popup, data);
    }


    else if (alertType == "comment") {
        ShareCommentLayer* popup = ShareCommentLayer::create(
            title,
            getInt(data, "charLimit", 100),
            static_cast<CommentType>(getKey(data, "commentType", COMMENT_TYPES, 0)), 1,
            hasContent ? getText(content, "") : ""
        );
        
        popup->m_percent = getInt(data, "percent", 0);

        popup->m_noElasticity = instant;
        popup->setID("custom_commentpopup"_spr);
        popup->show();

        setBorder(popup->m_mainLayer, data);

        if (data.contains("title")) {
            customizeText(popup->m_mainLayer->getChildByType<CCLabelBMFont>(0), data["title"]);
        }

        if (hasButton) customizeButton(popup->m_buttonMenu->getChildByType<CCMenuItemSpriteExtra>(-1), popup, button1, "");
        if (data.contains("button2")) customizeButton(popup->m_buttonMenu->getChildByType<CCMenuItemSpriteExtra>(-2), popup, data["button2"], "");

        setupCustomButtons(popup, data);
    }


    // Normal popup
    else {
        auto width = getNum(data, "width", 350.0f);
        if (width < 100) width = 100.0f;

        FLAlertLayer* popup = FLAlertLayer::create(
            nullptr,
            title.c_str(),
            hasContent ? getText(content, "").c_str() : "",
            hasButton ? getText(button1, "OK").c_str() : "OK",
            data.contains("button2") ? getText(data["button2"], "Cancel").c_str() : nullptr,
            width
        );

        popup->m_noElasticity = instant;
        popup->setID("custom_alertpopup"_spr);
        popup->show();

        if (hasContent) {
            TextArea* popupMsg = popup->m_mainLayer->getChildByType<TextArea>(0);
            customizeText(popupMsg, content);
        }

        if (data.contains("title")) {
            CCLabelBMFont* alertTitle = popup->m_mainLayer->getChildByType<CCLabelBMFont>(0);
            customizeText(alertTitle, data["title"]);
        }

        setBorder(popup->m_mainLayer, data, 2);

        if (hasButton) customizeButton(popup->m_buttonMenu->getChildByType<CCMenuItemSpriteExtra>(0), popup, button1, "OK");
        if (data.contains("button2")) customizeButton(popup->m_buttonMenu->getChildByType<CCMenuItemSpriteExtra>(1), popup, data["button2"], "Cancel");

        setupCustomButtons(popup, data);
    }
}

std::string getText(matjson::Value data, std::string fallback) {
    if (data == nullptr) return fallback;
    else if (data.isString()) return data.asString().unwrapOr(fallback);
    else if (data.isObject()) return getText(data.contains("content") ? data["content"] : data.contains("text") ? data["text"] : nullptr, fallback);
    else return fallback;
}

void customizeText(CCLabelBMFont* text, matjson::Value data) {

    if (!text) return;
    std::string str = getText(data, "");
    if (data.isString()) return text->setString(str.c_str());
    else if (!data.isObject()) return;

    // ====== //

    if (data.contains("text")) {
        text->setString(str.c_str());
    }

    if (data.contains("font")) {
        text->setFntFile(getStr(data, "font").c_str());
    }

    if (data.contains("color")) {
        text->setColor(getColor(data, "color", text->getColor()));
    }

    if (data.contains("scale")) {
        text->setScale(getNum(data, "scale", 1.0f));
    }
}

void customizeText(TextArea* text, matjson::Value data) {
    if (!text) return;
    std::string str = getText(data, "");
    if (data.isString()) return text->setString(str.c_str());
    else if (!data.isObject()) return;

    // ====== //

    if (data.contains("text")) {
        text->setString(str.c_str());
    }

    if (data.contains("color")) {
        text->setColor(getColor(data));
    }

    if (data.contains("scale")) {
        text->setScale(getNum(data, "scale", 1.0f));
    }
}

void customizeButton(CCMenuItemSpriteExtra* buttonBase, CCObject* parent, matjson::Value data, std::string fallback) {
    if (!buttonBase) return;

    ButtonSprite* button = buttonBase->getChildByType<ButtonSprite>(0);
    if (!button) return;

    CCLabelBMFont* label = button->getChildByType<CCLabelBMFont>(0);
    if (!label) return;


    if (fallback == "") fallback = label->m_sInitialStringUTF8;
    std::string str = getText(data, fallback);

    if (data.isString()) return label->setString(str.c_str());
    else if (!data.isObject()) return;
    
    // ====== //

    CCScale9Sprite* slice = button->getChildByType<CCScale9Sprite>(0);
    if (slice && data.contains("texture")) {
        auto customTexture = getCustomTexture(getStr(data, "texture").c_str(), "GJ_button_06.png", true);
        set9SpriteFrame(slice, customTexture);  // button->updateBGImage() is very prone to crashes
    }

    else if (data.contains("background")) {
        std::string bgName = getKey(data, "background", EXTRA_BUTTON_COLORS);
        if (bgName.empty()) {
            int col = getKey(data, "background", BUTTON_COLORS, 1);
            bgName = fmt::format("GJ_button_0{}.png", std::clamp(col, 1, 6));
        }
        button->updateBGImage(bgName.c_str());
    }

    if (data.contains("content") || data.contains("text")) {
        label->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        customizeText(label, data.contains("content") ? data["content"] : data["text"]);
    }

    if (data.contains("scale")) {
        button->setScale(getNum(data, "scale", 1.0f));
    }

    auto size = button->getContentSize();
    float w = getNum(data, "width", std::clamp(label->getContentWidth() + 14.0f, size.width, 150.0f));
    float h = getNum(data, "height", size.height);

    slice = button->getChildByType<CCScale9Sprite>(0); // need to get this again for some reason
    if (slice && (w != size.width || h != size.height)) {
        buttonBase->setContentSize({w, h});
        label->setPositionX(w / 2.0f);
        if (size.height != h) label->setPositionY((h / 2.0f) + 3.0f);  

        button->setContentSize({w, h});
        button->setPosition({w / 2.0f, h / 2.0f});

        slice->setContentSize({w, h});
        slice->setPosition({w / 2.0f, h / 2.0f});
    }

    if (data.contains("x") && data["x"].isNumber()) buttonBase->setPositionX(getNum(data, "x", 0.0f));
    if (data.contains("y") && data["y"].isNumber()) buttonBase->setPositionY(getNum(data, "y", 0.0f));
    if (data.contains("z") && data["z"].isNumber()) buttonBase->setZOrder(getInt(data, "z", 0));

    if (data.contains("pulse")) {
        auto pulseData = data["pulse"];

        float pulseSize = 1.1f;
        float pulseSpeed = 1.0f;

        if (pulseData.isObject()) {
            pulseSize = getNum(pulseData, "size", pulseSize);
            pulseSpeed = getNum(pulseData, "speed", pulseSpeed);
        }

        float scale = button->getScale();
        float newScale = scale * pulseSize;
        float duration = pulseSpeed;

        button->runAction(CCRepeatForever::create(
            CCSequence::create(
                CCEaseSineInOut::create(CCScaleTo::create(duration, newScale)),
                CCEaseSineInOut::create(CCScaleTo::create(duration, scale)),
                nullptr
            )
        ));
    }

    if (data.contains("onClick")) {
        setCustomButtonCallback(buttonBase, parent, data["onClick"]);
    }
}

void createCloseButton(FLAlertLayer* parent, matjson::Value data, CCPoint position) {
    if (!data.contains("closeButton")) return;

    auto close = data["closeButton"];
    if (close.isBool() && !close.asBool().unwrap()) return;

    auto closeBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"), parent, nullptr);
    closeBtn->setPosition(position);
    closeBtn->setID("close-button"_spr);
    parent->m_buttonMenu->addChild(closeBtn);

    if (close.isObject()) setCustomButtonCallback(closeBtn, parent, close);
}

void customizeIcon(CCSprite* iconSpr, matjson::Value data) {
    if (!data.contains("icon")) return;
    auto iconData = data["icon"];

    SimplePlayer* icon = typeinfo_cast<SimplePlayer*>(iconSpr);
    if (!icon) return;

    GameManager* gm = GameManager::get();

    if (iconData.contains("col1")) icon->setColor(gm->colorForIdx(getInt(iconData, "col1")));
    if (iconData.contains("col2")) icon->setSecondColor(gm->colorForIdx(getInt(iconData, "col2")));
    if (iconData.contains("colG")) icon->setGlowOutline(gm->colorForIdx(getInt(iconData, "colG")));
}

void setupCustomTexture(CCLayer* layer, matjson::Value data) {
    auto iconImg = layer->getChildByType<GJItemIcon>(0);
    if (!iconImg) return;

    if (iconImg) iconImg->setVisible(false);

    auto textureInfo = data["customTexture"];
    std::string fallback = "exMark_001.png";
    std::string texture = fallback;
    float scale = 1.0f;

    if (textureInfo.isString()) texture = textureInfo.asString().unwrapOr(fallback);
    else if (textureInfo.isObject()) {
        texture = getStr(textureInfo, "name", fallback.c_str());
        scale = getNum(textureInfo, "scale", 1);
    }

    auto customSprite = getCustomTexture(texture.c_str(), fallback.c_str());

    customSprite->setPosition(iconImg->getPosition());
    customSprite->setScale(scale);
    layer->addChild(customSprite);
}

void setupCustomButtons(FLAlertLayer* layer, matjson::Value data) {
    if (!data.contains("extraButtons")) return;
    auto dat = data["extraButtons"];

    if (dat.isObject()) createExtraButton(layer, dat);

    else if (dat.isArray()) {
        auto arr = dat.asArray().unwrap();
        for (auto b : arr) {
            createExtraButton(layer, b);
        }
    }
}

void createExtraButton(FLAlertLayer* layer, matjson::Value data) {
    // just create a default button sprite and change the properties in customizeButton()
    // can probably improve on this later but it works for now

    auto msg = getText(data, "OK");
    auto bSprite = ButtonSprite::create(msg.c_str());
    if (!bSprite) return;

    auto bItem = CCMenuItemExt::createSpriteExtra(bSprite, [layer](auto) { layer->keyBackClicked(); });
    bItem->setID("custom_button"_spr);

    customizeButton(bItem, layer, data, msg);
    layer->m_buttonMenu->addChild(bItem);
}