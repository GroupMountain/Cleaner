#include "Global.h"

typedef std::chrono::high_resolution_clock timer_clock;
#define TIMER_START auto start = timer_clock::now();
#define TIMER_END                                                                                                      \
    auto      elapsed    = timer_clock::now() - start;                                                                 \
    long long timeReslut = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

uint              ticks          = 0;
float             average_tps    = 0;
double            mspt           = 0;
bool              culculate_mspt = false;
std::list<ushort> avrtps         = {};

LL_AUTO_TYPED_INSTANCE_HOOK(LevelTickHook, ll::memory::HookPriority::Normal, Level, "?tick@Level@@UEAAXXZ", void) {
    ticks++;
    TIMER_START
    origin();
    TIMER_END
    culculate_mspt = true;
    if (culculate_mspt) {
        mspt           = (double)timeReslut / 1000;
        culculate_mspt = false;
    }
}

namespace TPS {

void CaculateTPS() {
    std::thread([] {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            culculate_mspt = true;
            avrtps.push_back(ticks);
            ticks = 0;
            if (avrtps.size() >= 60) {
                avrtps.clear();
                continue;
            }
            uint ticks_minute = 0;
            for (auto i : avrtps) {
                ticks_minute = ticks_minute + i;
            }
            float res   = (float)ticks_minute / ((float)avrtps.size());
            average_tps = res >= 20 ? 20 : res;
        }
    }).detach();
}

float getCurrentTps() { return mspt <= 50 ? 20 : (float)(1000.0 / mspt); }

float getAverageTps() { return average_tps; }

} // namespace TPS
