#include <Geode/Geode.hpp>
#include <algorithm>
#include <string>
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

    constexpr size_t PRESET_COUNT = sizeof(PRESETS) / sizeof(PRESETS[0]);

    size_t selectedPreset() {
        // Clamp saved data before converting to size_t so a corrupted or
        // negative setting can never produce an invalid PRESETS index.
        auto saved = Mod::get()->getSavedValue<int>("selected-preset", 0);
        return static_cast<size_t>(std::clamp(saved, 0, static_cast<int>(PRESET_COUNT) - 1));
    }

    int intensityFor(size_t index) {
        static constexpr char const* KEYS[] = {
            "intensity-hell", "intensity-white", "intensity-ocean",
            "intensity-amethyst", "intensity-mono", "intensity-nature",
            "intensity-nine", "intensity-cyber", "intensity-industrial",
            "intensity-sunset"
        };
        return Mod::get()->getSavedValue<int>(KEYS[index], PRESETS[index].defaultIntensity);
    }

    bool glowFor(size_t index) {
        static constexpr char const* KEYS[] = {
            "glow-hell", "glow-white", "glow-ocean",
            "glow-amethyst", "glow-mono", "glow-nature",
            "glow-nine", "glow-cyber", "glow-industrial",
            "glow-sunset"
        };
        return Mod::get()->getSavedValue<bool>(KEYS[index], PRESETS[index].defaultGlow);
    }

    void saveIntensity(size_t index, int value) {
        static constexpr char const* KEYS[] = {
            "intensity-hell", "intensity-white", "intensity-ocean",
            "intensity-amethyst", "intensity-mono", "intensity-nature",
            "intensity-nine", "intensity-cyber", "intensity-industrial",
            "intensity-sunset"
        };
        Mod::get()->setSavedValue<int>(KEYS[index], std::clamp(value, 0, 100));
    }

    void saveGlow(size_t index, bool value) {
        static constexpr char const* KEYS[] = {
            "glow-hell", "glow-white", "glow-ocean",
            "glow-amethyst", "glow-mono", "glow-nature",
            "glow-nine", "glow-cyber", "glow-industrial",
            "glow-sunset"
        };
        Mod::get()->setSavedValue<bool>(KEYS[index], value);
    }

    void showMainMenu();

    void showSensitivityMenu() {
        auto sensitivity = Mod::get()->getSavedValue<int>("sensitivity", 50);
        createQuickPopup(
            "DECORATION SENSITIVITY",
            fmt::format("Current sensitivity: {}%\n\nHigher values allow stronger style changes. Lower values keep changes subtle.", sensitivity),
            "Decrease",
            "Increase",
            [sensitivity](FLAlertLayer*, bool increase) {
                auto next = std::clamp(sensitivity + (increase ? 10 : -10), 0, 100);
                Mod::get()->setSavedValue<int>("sensitivity", next);
                createQuickPopup("SENSITIVITY SAVED", fmt::format("Sensitivity is now {}%.", next), "OK");
            }
        );
    }

    void showIntensityMenu() {
        auto index = selectedPreset();
        auto value = intensityFor(index);
        createQuickPopup(
            "PRESET INTENSITY",
            fmt::format("{} intensity is {}%. Adjust it in 5% steps.", PRESETS[index].name, value),
            "Decrease",
            "Increase",
            [index, value](FLAlertLayer*, bool increase) {
                auto next = std::clamp(value + (increase ? 5 : -5), 0, 100);
                saveIntensity(index, next);
                createQuickPopup("INTENSITY SAVED", fmt::format("{} intensity is now {}%.", PRESETS[index].name, next), "OK");
            }
        );
    }

    void showGlowMenu() {
        auto index = selectedPreset();
        auto enabled = glowFor(index);
        createQuickPopup(
            "PRESET GLOW",
            fmt::format("{} glow is currently {}.", PRESETS[index].name, enabled ? "enabled" : "disabled"),
            "Disable",
            "Enable",
            [index](FLAlertLayer*, bool enable) {
                saveGlow(index, enable);
                createQuickPopup("GLOW SAVED", fmt::format("Glow {} for {}.", enable ? "enabled" : "disabled", PRESETS[index].name), "OK");
            }
        );
    }

    void showCustomPresetMenu() {
        auto index = selectedPreset();
        auto intensity = intensityFor(index);
        auto glow = glowFor(index);
        auto slot = std::clamp(
            Mod::get()->getSavedValue<int>("custom-preset-count", 0) + 1,
            1, 999
        );
        Mod::get()->setSavedValue<int>("custom-preset-count", slot);
        Mod::get()->setSavedValue<int>("custom-" + std::to_string(slot) + "-intensity", intensity);
        Mod::get()->setSavedValue<bool>("custom-" + std::to_string(slot) + "-glow", glow);
        Mod::get()->setSavedValue<std::string>("custom-" + std::to_string(slot) + "-name", "Custom " + std::to_string(slot));
        createQuickPopup(
            "CUSTOM PRESET SAVED",
            fmt::format("Saved Custom {} from {} settings ({}% intensity, glow {}).\n\nCustom preset naming and editing will be added in a later update.", slot, PRESETS[index].name, intensity, glow ? "on" : "off"),
            "OK"
        );
    }

    void showPreviewMenu() {
        auto index = selectedPreset();
        auto const& preset = PRESETS[index];
        auto intensity = intensityFor(index);
        auto glow = glowFor(index);
        auto sensitivity = Mod::get()->getSavedValue<int>("sensitivity", 50);

        static constexpr char const* PALETTES[] = {
            "Crimson / ember / charcoal",
            "White / ice blue / silver",
            "Deep blue / cyan / seafoam",
            "Violet / lavender / crystal",
            "Black / white / gray",
            "Forest green / leaf / lime",
            "Neon pink / electric blue / black",
            "Magenta / cyan / midnight",
            "Steel / copper / graphite",
            "Amber / orange / dusk"
        };

        createQuickPopup(
            "BEFORE / AFTER PREVIEW",
            fmt::format(
                "CURRENT LEVEL\nYour original level remains unchanged.\n\n"
                "DECORATION PLAN\nPreset: {}\nPalette: {}\nIntensity: {}%\nGlow: {}\nSensitivity: {}%\n\n"
                "This is a settings preview, not a rendered level screenshot. Actual object placement and a live before/after renderer are not connected yet.",
                preset.name, PALETTES[index], intensity, glow ? "ON" : "OFF", sensitivity
            ),
            "Back",
            "Smart plan",
            [](FLAlertLayer*, bool makePlan) {
                if (makePlan) showScanMenu();
                else showMainMenu();
            }
        );
    }

    void showScanMenu() {
        auto index = selectedPreset();
        auto const& preset = PRESETS[index];
        auto intensity = intensityFor(index);
        auto glow = glowFor(index);
        auto sensitivity = Mod::get()->getSavedValue<int>("sensitivity", 50);

        createQuickPopup(
            "SMART DECORATION PLAN",
            fmt::format(
                "Proposed style: {}\nIntensity: {}%\nGlow: {}\nSensitivity: {}%\n\n"
                "The planner can prepare a style configuration, but this version does not yet inspect or modify editor objects. Your level is safe and unchanged. The next implementation step is object scanning, generated placement, and a real rendered preview.",
                preset.name, intensity, glow ? "ON" : "OFF", sensitivity
            ),
            "Preview",
            "OK",
            [](FLAlertLayer*, bool preview) {
                if (preview) showPreviewMenu();
            }
        );
    }

    void showMoreMenu() {
        createQuickPopup(
            "AUTO DECORATOR TOOLS",
            "Choose a tool. Settings are saved on this device.",
            "Smart Decorate",
            "Settings",
            [](FLAlertLayer*, bool settings) {
                if (settings) {
                    createQuickPopup(
                        "PRESET SETTINGS",
                        "Choose what to configure.",
                        "Sensitivity",
                        "More",
                        [](FLAlertLayer*, bool more) {
                            if (!more) {
                                showSensitivityMenu();
                                return;
                            }
                            createQuickPopup(
                                "MORE SETTINGS",
                                "Each preset stores its own intensity and glow preference.",
                                "Intensity",
                                "Other",
                                [](FLAlertLayer*, bool other) {
                                    if (!other) {
                                        showIntensityMenu();
                                        return;
                                    }
                                    createQuickPopup(
                                        "PRESET EXTRAS",
                                        "Choose an extra action.",
                                        "Glow",
                                        "Save preset",
                                        [](FLAlertLayer*, bool save) {
                                            if (save) showCustomPresetMenu();
                                            else showGlowMenu();
                                        }
                                    );
                                }
                            );
                        }
                    );
                } else {
                    showPreviewMenu();
                }
            }
        );
    }

    void showMainMenu() {
        auto index = selectedPreset();
        auto const& preset = PRESETS[index];
        auto intensity = intensityFor(index);
        auto glow = glowFor(index);

        createQuickPopup(
            "AUTO DECORATOR",
            fmt::format(
                "PRESET {}/{}: {}\n{}\n\nIntensity: {}%\nGlow: {}\nSensitivity: {}%\n\n"
                "Preset settings are stored separately. This prototype currently manages settings and saved custom snapshots; it does not yet rewrite level objects.",
                index + 1, PRESET_COUNT, preset.name, preset.description,
                intensity, glow ? "ON" : "OFF",
                Mod::get()->getSavedValue<int>("sensitivity", 50)
            ),
            "Next preset",
            "More",
            [](FLAlertLayer*, bool more) {
                if (more) {
                    showMoreMenu();
                    return;
                }

                auto next = (selectedPreset() + 1) % PRESET_COUNT;
                Mod::get()->setSavedValue<int>("selected-preset", static_cast<int>(next));
                showMainMenu();
            }
        );
    }
}

class $modify(AutoDecoratorEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Temporary red-tinted editor button. The supplied demon artwork still
        // needs to be added as a packaged PNG resource before it can replace this.
        auto icon = CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png");
        if (!icon) {
            icon = CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
        }
        if (!icon) {
            icon = CCSprite::create();
            icon->setContentSize({32.f, 32.f});
        }
        icon->setColor({255, 45, 45});
        icon->setScale(0.78f);

        auto button = CCMenuItemSpriteExtra::create(
            icon,
            this,
            menu_selector(AutoDecoratorEditorUI::onAutoDecorator)
        );

        auto menu = CCMenu::create();
        menu->setID("auto-decorator-floating-menu"_spr);
        menu->addChild(button);
        menu->setPosition({winSize.width - 30.f, winSize.height - 75.f});
        this->addChild(menu, 1000);

        return true;
    }

    void onAutoDecorator(CCObject*) {
        showMainMenu();
    }
};
