#include "Config.h"
#include "Global.h"
#include "Language.h"

GMLIB::Files::JsonConfig*     Config                 = nullptr;
GMLIB::Files::I18n::JsonI18n* Language               = nullptr;
int                           commandPermissionLevel = 4;

void initPlugin() {
    Config = new GMLIB::Files::JsonConfig("./plugins/Cleaner/config/config.json", defaultConfig);
    Config->init();
    std::string langPath     = "./plugins/Cleaner/language/";
    std::string languageCode = Config->getValue<std::string>({"Basic", "Language"}, "en_US");
    Language                 = new GMLIB::Files::I18n::JsonI18n(langPath, languageCode);
    Language->updateOrCreateLanguage("en_US", defaultLanguage_en_US);
    Language->updateOrCreateLanguage("zh_CN", defaultLanguage_zh_CN);
    Language->loadAllLanguages();
    Language->chooseLanguage(languageCode);
}

std::string tr(std::string key, std::vector<std::string> data) { return Language->translate(key, data); }