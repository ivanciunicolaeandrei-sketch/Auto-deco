#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

namespace {
    struct Preset {
        char const* name;
        char const* description;
        int defaultIntensity;
        bool defaultGlow;
    };

    constexpr Preset PRESETS[] = {
        {"Hell Theme", "Deep reds, black shadows and fiery accents.", 84, true},
        {"White Space", "Clean whites, cool blues and detailed atmosphere.", 75, true},
        {"Oceanic", "Ocean blues, aquatic accents and flowing scenery.", 78, true},
        {"Amethyst", "Purple crystal tones with luminous highlights.", 80, true},
        {"Monochrome", "Grayscale contrast and geometric minimalism.", 70, false},
        {"Nature", "Natural greens and organic visual accents.", 76, true},
        {"Nine Circles", "High-contrast neon colors and rhythmic patterns.", 82, true},
        {"Cyberpunk", "Electric neon, dark panels and futuristic accents.", 85, true},
        {"Industrial", "Metallic surfaces, machinery and structured detail.", 72, false},
        {"Sunset", "Warm amber, orange and silhouette-based atmosphere.", 74, true}
    };

    size_t currentPreset() {
        return Mod::get()->getSavedValue<size_t>("selected-preset", 0);
    }

    void showAutoDecoratorMenu() {
        auto index = currentPreset() % std::size(PRESETS);
        auto const& preset = PRESETS[index];

        auto body = fmt::format(
            "{}\n{}\n\nIntensity: {}%\nGlow: {}\n\n"
            "This is the first menu milestone. Preset application and the custom "
            "level scanner are being added next.",
            preset.name,
            preset.description,
            preset.defaultIntensity,
            preset.defaultGlow ? "Enabled" : "Disabled"
        );

        FLAlertLayer::create(
            "AUTO DECORATOR",
            body,
            "Close"
        )->show();
    }
}

class $modify(AutoDecoratorEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto icon = CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
        if (!icon) {
            icon = CCSprite::create();
            icon->setContentSize({32.f, 32.f});
        }

        auto button = CCMenuItemSpriteExtra::create(
            icon,
            this,
            menu_selector(AutoDecoratorEditorUI::onAutoDecorator)
        );

        auto menu = CCMenu::create();
        menu->setID("auto-decorator-floating-menu"_spr);
        menu->addChild(button);
        menu->setPosition({winSize.width - 28.f, winSize.height - 72.f});
        this->addChild(menu, 1000);

        return true;
    }

    void onAutoDecorator(CCObject*) {
        showAutoDecoratorMenu();
    }
};
