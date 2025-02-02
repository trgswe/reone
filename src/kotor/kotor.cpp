/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "kotor.h"

#include "../../audio/files.h"
#include "../../audio/player.h"
#include "../../common/collectionutil.h"
#include "../../common/exception/validation.h"
#include "../../common/logutil.h"
#include "../../common/pathutil.h"
#include "../../common/streamutil.h"
#include "../../common/streamwriter.h"
#include "../../graphics/format/tgawriter.h"
#include "../../graphics/lipanimations.h"
#include "../../graphics/models.h"
#include "../../graphics/pipeline.h"
#include "../../graphics/renderbuffer.h"
#include "../../graphics/textures.h"
#include "../../graphics/walkmeshes.h"
#include "../../graphics/window.h"
#include "../../gui/gui.h"
#include "../../movie/format/bikreader.h"
#include "../../resource/2das.h"
#include "../../resource/format/erfreader.h"
#include "../../resource/format/erfwriter.h"
#include "../../resource/format/gffwriter.h"
#include "../../resource/gffs.h"
#include "../../resource/resources.h"
#include "../../script/scripts.h"

#include "../game/combat.h"
#include "../game/dialogs.h"
#include "../game/location.h"
#include "../game/object/factory.h"
#include "../game/party.h"
#include "../game/services.h"
#include "../game/soundsets.h"
#include "../game/surfaces.h"

#include "cursors.h"
#include "gui/console.h"
#include "gui/loadscreen.h"
#include "gui/map.h"
#include "gui/profileoverlay.h"
#include "script/routines.h"

using namespace std;

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace fs = boost::filesystem;

namespace reone {

namespace kotor {

static constexpr char kKeyFilename[] = "chitin.key";
static constexpr char kPatchFilename[] = "patch.erf";
static constexpr char kModulesDirectoryName[] = "modules";
static constexpr char kTexturePackDirectoryName[] = "texturepacks";
static constexpr char kMusicDirectoryName[] = "streammusic";
static constexpr char kSoundsDirectoryName[] = "streamsounds";
static constexpr char kWavesDirectoryName[] = "streamwaves";
static constexpr char kVoiceDirectoryName[] = "streamvoice";
static constexpr char kLipsDirectoryName[] = "lips";
static constexpr char kLocalizationLipFilename[] = "localization";
static constexpr char kOverrideDirectoryName[] = "override";

static constexpr char kTexturePackFilenameGUI[] = "swpc_tex_gui.erf";
static constexpr char kTexturePackFilenameHigh[] = "swpc_tex_tpa.erf";
static constexpr char kTexturePackFilenameMedium[] = "swpc_tex_tpb.erf";
static constexpr char kTexturePackFilenameLow[] = "swpc_tex_tpc.erf";

static constexpr char kBlueprintResRefCarth[] = "p_carth";
static constexpr char kBlueprintResRefBastila[] = "p_bastilla";
static constexpr char kBlueprintResRefAtton[] = "p_atton";
static constexpr char kBlueprintResRefKreia[] = "p_kreia";

static constexpr char kExeFilenameKotor[] = "swkotor.exe";
static constexpr char kExeFilenameTsl[] = "swkotor2.exe";

static const vector<string> g_nonTransientLipFiles {"global.mod", "localization.mod"};

static bool g_conversationsEnabled = true;

void KotOR::initResourceProviders() {
    static const unordered_map<TextureQuality, string> texPackByQuality {
        {TextureQuality::High, kTexturePackFilenameHigh},
        {TextureQuality::Medium, kTexturePackFilenameMedium},
        {TextureQuality::Low, kTexturePackFilenameLow}};

    if (_tsl) {
        _services.resources.indexKeyFile(getPathIgnoreCase(_path, kKeyFilename));

        fs::path texPacksPath(getPathIgnoreCase(_path, kTexturePackDirectoryName));
        _services.resources.indexErfFile(getPathIgnoreCase(texPacksPath, kTexturePackFilenameGUI));

        auto texPack = texPackByQuality.find(_options.graphics.textureQuality)->second;
        _services.resources.indexErfFile(getPathIgnoreCase(texPacksPath, texPack));

        _services.resources.indexDirectory(getPathIgnoreCase(_path, kMusicDirectoryName));
        _services.resources.indexDirectory(getPathIgnoreCase(_path, kSoundsDirectoryName));
        _services.resources.indexDirectory(getPathIgnoreCase(_path, kVoiceDirectoryName));

        fs::path lipsPath(getPathIgnoreCase(_path, kLipsDirectoryName));
        _services.resources.indexErfFile(getPathIgnoreCase(lipsPath, kLocalizationLipFilename));

        _services.resources.indexDirectory(getPathIgnoreCase(_path, kOverrideDirectoryName));
        _services.resources.indexExeFile(getPathIgnoreCase(_path, kExeFilenameTsl));

    } else {
        _services.resources.indexKeyFile(getPathIgnoreCase(_path, kKeyFilename));
        _services.resources.indexErfFile(getPathIgnoreCase(_path, kPatchFilename));

        fs::path texPacksPath(getPathIgnoreCase(_path, kTexturePackDirectoryName));
        _services.resources.indexErfFile(getPathIgnoreCase(texPacksPath, kTexturePackFilenameGUI));

        auto texPack = texPackByQuality.find(_options.graphics.textureQuality)->second;
        _services.resources.indexErfFile(getPathIgnoreCase(texPacksPath, texPack));

        _services.resources.indexDirectory(getPathIgnoreCase(_path, kMusicDirectoryName));
        _services.resources.indexDirectory(getPathIgnoreCase(_path, kSoundsDirectoryName));
        _services.resources.indexDirectory(getPathIgnoreCase(_path, kWavesDirectoryName));

        fs::path lipsPath(getPathIgnoreCase(_path, kLipsDirectoryName));
        for (auto &filename : g_nonTransientLipFiles) {
            _services.resources.indexErfFile(getPathIgnoreCase(lipsPath, filename));
        }

        _services.resources.indexDirectory(getPathIgnoreCase(_path, kOverrideDirectoryName));
        _services.resources.indexExeFile(getPathIgnoreCase(_path, kExeFilenameKotor));
    }
}

void KotOR::init() {
    Game::init();

    auto routines = make_unique<Routines>(*this, _services);
    _scriptRunner = make_unique<ScriptRunner>(*routines, _services.scripts);

    auto map = make_unique<Map>(*this, _services);
    auto console = make_unique<Console>(*this, _services);
    auto profileOverlay = make_unique<ProfileOverlay>(_services, _options);

    console->init();
    profileOverlay->init();

    if (_tsl) {
        _mainMenuMusicResRef = "mus_sion";
        _charGenMusicResRef = "mus_main";
        _charGenLoadScreenResRef = "load_default";

        _guiColorBase = glm::vec3(0.192157f, 0.768627f, 0.647059f);
        _guiColorHilight = glm::vec3(0.768627f, 0.768627f, 0.686275f);
        _guiColorDisabled = glm::vec3(0.513725f, 0.513725f, 0.415686f);

        routines->initForTSL();
        map->setArrowResRef("mm_barrow_p");

    } else {
        _mainMenuMusicResRef = "mus_theme_cult";
        _charGenMusicResRef = "mus_theme_rep";
        _charGenLoadScreenResRef = "load_chargen";

        _guiColorBase = glm::vec3(0.0f, 0.639216f, 0.952941f);
        _guiColorHilight = glm::vec3(0.980392f, 1.0f, 0.0f);
        _guiColorDisabled = glm::vec3(0.0f, 0.349020f, 0.549020f);

        routines->initForKotOR();
        map->setArrowResRef("mm_barrow");
    }

    _screen = GameScreen::MainMenu;

    _routines = move(routines);
    _map = move(map);
    _console = move(console);
    _profileOverlay = move(profileOverlay);
}

void KotOR::loadModuleNames() {
    fs::path modules(getPathIgnoreCase(_path, kModulesDirectoryName));
    if (modules.empty()) {
        throw ValidationException("Modules directory not found");
    }
    for (auto &entry : fs::directory_iterator(modules)) {
        string filename(boost::to_lower_copy(entry.path().filename().string()));
        if (boost::ends_with(filename, ".mod") || (boost::ends_with(filename, ".rim") && !boost::ends_with(filename, "_s.rim"))) {
            string moduleName(boost::to_lower_copy(filename.substr(0, filename.size() - 4)));
            _moduleNames.insert(move(moduleName));
        }
    }
}

void KotOR::start() {
    playVideo("legal");
    openMainMenu();
}

void KotOR::loadModuleResources(const string &moduleName) {
    _services.twoDas.invalidate();
    _services.gffs.invalidate();
    _services.resources.clearTransientProviders();

    fs::path modulesPath(getPathIgnoreCase(_path, kModulesDirectoryName));
    if (modulesPath.empty()) {
        throw ValidationException("Modules directory not found");
    }

    fs::path modPath(getPathIgnoreCase(modulesPath, moduleName + ".mod"));
    if (!modPath.empty()) {
        _services.resources.indexErfFile(getPathIgnoreCase(modulesPath, moduleName + ".mod", false));
    } else {
        _services.resources.indexRimFile(getPathIgnoreCase(modulesPath, moduleName + ".rim"));
        _services.resources.indexRimFile(getPathIgnoreCase(modulesPath, moduleName + "_s.rim"));
    }

    fs::path lipsPath(getPathIgnoreCase(_path, kLipsDirectoryName));
    if (!lipsPath.empty()) {
        _services.resources.indexErfFile(getPathIgnoreCase(lipsPath, moduleName + "_loc.mod"));
    }

    if (isTSL()) {
        _services.resources.indexErfFile(getPathIgnoreCase(modulesPath, moduleName + "_dlg.erf"));
    }
}

void KotOR::onModuleSelected(const string &module) {
    _mainMenu->onModuleSelected(module);
}

void KotOR::drawHUD() {
    _hud->draw();
}

game::CameraType KotOR::getConversationCamera(int &cameraId) const {
    return _conversation->getCamera(cameraId);
}

void KotOR::setBarkBubbleText(string text, float duration) {
    _hud->barkBubble().setBarkText(text, duration);
}

void KotOR::loadInGameMenus() {
    if (!_hud) {
        loadHUD();
    }
    if (!_inGame) {
        loadInGame();
    }
    if (!_dialog) {
        loadDialog();
    }
    if (!_computer) {
        loadComputer();
    }
    if (!_container) {
        loadContainer();
    }
    if (!_partySelect) {
        loadPartySelection();
    }
}

void KotOR::loadMainMenu() {
    _mainMenu = make_unique<MainMenu>(*this, _services);
    _mainMenu->load();
}

void KotOR::loadHUD() {
    _hud = make_unique<HUD>(*this, _services);
    _hud->load();
}

void KotOR::loadDialog() {
    _dialog = make_unique<DialogGUI>(*this, _services);
    _dialog->load();
}

void KotOR::loadComputer() {
    _computer = make_unique<ComputerGUI>(*this, _services);
    _computer->load();
}

void KotOR::loadContainer() {
    _container = make_unique<ContainerGUI>(*this, _services);
    _container->load();
}

void KotOR::loadPartySelection() {
    _partySelect = make_unique<PartySelection>(*this, _services);
    _partySelect->load();
}

void KotOR::loadSaveLoad() {
    _saveLoad = make_unique<SaveLoad>(*this, _services);
    _saveLoad->load();
}

void KotOR::loadLoadingScreen() {
    _loadScreen = make_unique<LoadingScreen>(*this, _services);
    static_cast<LoadingScreen *>(_loadScreen.get())->load();
}

void KotOR::loadCharacterGeneration() {
    _charGen = make_unique<CharacterGeneration>(*this, _services);
    _charGen->load();
}

void KotOR::loadInGame() {
    _inGame = make_unique<InGameMenu>(*this, _services);
    _inGame->load();
}

void KotOR::openMainMenu() {
    if (!_mainMenu) {
        loadMainMenu();
    }
    if (!_saveLoad) {
        loadSaveLoad();
    }
    playMusic(_mainMenuMusicResRef);
    changeScreen(GameScreen::MainMenu);
}

void KotOR::openInGame() {
    changeScreen(GameScreen::InGame);
}

void KotOR::openInGameMenu(InGameMenuTab tab) {
    setCursorType(CursorType::Default);
    switch (tab) {
    case InGameMenuTab::Equipment:
        _inGame->openEquipment();
        break;
    case InGameMenuTab::Inventory:
        _inGame->openInventory();
        break;
    case InGameMenuTab::Character:
        _inGame->openCharacter();
        break;
    case InGameMenuTab::Abilities:
        _inGame->openAbilities();
        break;
    case InGameMenuTab::Messages:
        _inGame->openMessages();
        break;
    case InGameMenuTab::Journal:
        _inGame->openJournal();
        break;
    case InGameMenuTab::Map:
        _inGame->openMap();
        break;
    case InGameMenuTab::Options:
        _inGame->openOptions();
        break;
    default:
        break;
    }
    changeScreen(GameScreen::InGameMenu);
}

void KotOR::openContainer(const shared_ptr<Object> &container) {
    stopMovement();
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _container->open(container);
    changeScreen(GameScreen::Container);
}

void KotOR::openPartySelection(const PartySelectionContext &ctx) {
    stopMovement();
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _partySelect->prepare(ctx);
    changeScreen(GameScreen::PartySelection);
}

void KotOR::openSaveLoad(SaveLoadMode mode) {
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _saveLoad->setMode(mode);
    _saveLoad->refresh();
    changeScreen(GameScreen::SaveLoad);
}

void KotOR::openLevelUp() {
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _charGen->startLevelUp();
    changeScreen(GameScreen::CharacterGeneration);
}

void KotOR::startCharacterGeneration() {
    withLoadingScreen(_charGenLoadScreenResRef, [this]() {
        if (!_charGen) {
            loadCharacterGeneration();
        }
        _loadScreen->setProgress(100);
        drawAll();
        playMusic(_charGenMusicResRef);
        changeScreen(GameScreen::CharacterGeneration);
    });
}

void KotOR::startDialog(const shared_ptr<Object> &owner, const string &resRef) {
    if (!g_conversationsEnabled)
        return;

    shared_ptr<GffStruct> dlg(_services.gffs.get(resRef, ResourceType::Dlg));
    if (!dlg) {
        warn("Game: conversation not found: " + resRef);
        return;
    }

    stopMovement();
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    changeScreen(GameScreen::Conversation);

    auto dialog = _services.dialogs.get(resRef);
    bool computerConversation = dialog->conversationType == ConversationType::Computer;
    _conversation = computerConversation ? _computer.get() : static_cast<Conversation *>(_dialog.get());
    _conversation->start(dialog, owner);
}

void KotOR::resumeConversation() {
    _conversation->resume();
}

void KotOR::pauseConversation() {
    _conversation->pause();
}

void KotOR::changeScreen(GameScreen screen) {
    GUI *gui = getScreenGUI();
    if (gui) {
        gui->resetFocus();
    }
    _screen = screen;
}

GUI *KotOR::getScreenGUI() const {
    switch (_screen) {
    case GameScreen::MainMenu:
        return _mainMenu.get();
    case GameScreen::Loading:
        return static_cast<LoadingScreen *>(_loadScreen.get());
    case GameScreen::CharacterGeneration:
        return _charGen.get();
    case GameScreen::InGame:
        return _cameraType == game::CameraType::ThirdPerson ? _hud.get() : nullptr;
    case GameScreen::InGameMenu:
        return _inGame.get();
    case GameScreen::Conversation:
        return _conversation;
    case GameScreen::Container:
        return _container.get();
    case GameScreen::PartySelection:
        return _partySelect.get();
    case GameScreen::SaveLoad:
        return _saveLoad.get();
    default:
        return nullptr;
    }
}

void KotOR::getDefaultPartyMembers(string &member1, string &member2, string &member3) const {
    if (_tsl) {
        member1 = kBlueprintResRefAtton;
        member2 = kBlueprintResRefKreia;
    } else {
        member1 = kBlueprintResRefCarth;
        member2 = kBlueprintResRefBastila;
    }
    member3.clear();
}

} // namespace kotor

} // namespace reone
