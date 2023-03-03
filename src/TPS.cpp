#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/mc/Actor.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/ScheduleAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/DynamicCommandAPI.h>
#include "setting.h"
#include "version.h"
#include "Global.h"
#include <llapi/RemoteCallAPI.h>

extern Logger logger;
extern string endstr;

unsigned short ticks = 0;
unsigned short Tick_per_second;
float Tick_per_minute;
std::list<unsigned short> ticks_per_minute;

std::string TpsColor1(unsigned short tps) {
	if (tps < 15) return "§6" + std::to_string(tps);
	if (tps <= 10) return "§c" + std::to_string(tps);
	return "§a" + std::to_string(tps);
}

std::string TpsColor2(float tps) {
    auto pre1 = std::to_string(tps);
    auto pre2 = pre1.replace(pre1.end()-4,pre1.end(),"");
	if (tps < 15) return "§6" + pre2;
	if (tps <= 10) return "§c" + pre2;
	return "§a" + pre2;
}

void CaculateTPS() {
	Schedule::repeat([]() {
		ticks++;
	}, 1);
	std::thread([] {
		while (true) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			if (ticks > 20) {
				ticks = 20;
			}
			Tick_per_second = ticks;
			ticks_per_minute.push_back(ticks);
			ticks = 0;
			if (ticks_per_minute.size() >= 60) {
				ticks_per_minute.clear();
				continue;
			}
			unsigned short ticks_minute = 0;
			for (unsigned short i : ticks_per_minute) {
				ticks_minute = ticks_minute + i;
			}
			Tick_per_minute = (float)ticks_minute / (float)ticks_per_minute.size();
		}
	})
	.detach();
}

void RegTPSCommand() {
    using ParamType = DynamicCommand::ParameterType;
    auto command = DynamicCommand::createCommand("tps", LanguageSettings::tpscmd + endstr, CommandPermissionLevel::Any, { (CommandFlagValue)0x80 }, { (CommandFlagValue)1 });
    command->addOverload();
    command->setCallback([](DynamicCommand const& command, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>& results) {
		LanguageSettings::LoadLanguageFromJson(LanguageFile);
		auto pr1 = ReplaceStr(LanguageSettings::tpscmdout,"{1}",TpsColor1(Tick_per_second));
        output.success(ReplaceStr(pr1,"{2}",TpsColor2(Tick_per_minute)));
    });
    DynamicCommand::setup(std::move(command));
}