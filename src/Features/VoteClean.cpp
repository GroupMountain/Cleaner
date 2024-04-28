#include "Cleaner.h"
#include "Global.h"

namespace VoteClean {

bool hasVote     = false;
bool canVote     = true;
int  playerCount = 0;

std::unordered_map<mce::UUID, bool> voteList;

static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mAutoCleanTask;

int getPlayerCount() {
    int result = 0;
    ll::service::getLevel()->forEachPlayer([&](Player& pl) -> bool {
        if (!pl.isSimulatedPlayer()) {
            result++;
        }
        return true;
    });
    return result;
}

void sendVoteForm(Player* pl) {
    auto fm = ll::form::ModalForm(
        I18nAPI::get("cleaner.vote.title"),
        I18nAPI::get("cleaner.vote.subtitle", {pl->getRealName()}),
        I18nAPI::get("cleaner.vote.ok"),
        I18nAPI::get("cleaner.vote.no")
    );
    ll::service::getLevel()->forEachPlayer([&](Player& pl) -> bool {
        fm.sendTo(pl, [](Player& player, ll::form::ModalFormResult result, ll::form::FormCancelReason reason) {
            if (result.has_value()) {
                switch (result.value()) {
                case ll::form::ModalFormSelectedButton::Upper: {
                    voteList[player.getUuid()] = true;
                    player.sendMessage(I18nAPI::get("cleaner.vote.accept"));
                    return;
                }
                case ll::form::ModalFormSelectedButton::Lower: {
                    voteList[player.getUuid()] = false;
                    player.sendMessage(I18nAPI::get("cleaner.vote.deny"));
                    return;
                }
                default:
                    return;
                }
            }
        });
        return true;
    });
}

void checkVote() {
    auto& config     = Cleaner::Entry::getInstance().getConfig();
    float percentage = config.VoteClean.Percentage / 100.0f;
    int   voteCount  = 0;
    for (auto& key : voteList) {
        if (key.second == true) {
            voteCount++;
        }
    }
    float result = ((float)voteCount) / ((float)playerCount);
    if (result >= percentage) {
        if (config.Basic.SendBroadcast) {
            Helper::broadcastMessage(I18nAPI::get("cleaner.vote.succeed"));
        }
        if (config.Basic.SendToast) {
            Helper::broadcastToast(I18nAPI::get("cleaner.vote.succeed"));
        }
        Cleaner::CleanTask();
    } else {
        if (config.Basic.SendBroadcast) {
            Helper::broadcastMessage(I18nAPI::get("cleaner.vote.failed"));
        }
        if (config.Basic.SendToast) {
            Helper::broadcastToast(I18nAPI::get("cleaner.vote.failed"));
        }
    }
    hasVote     = false;
    playerCount = 0;
}

void voteClean(Player* pl) {
    auto& config = Cleaner::Entry::getInstance().getConfig();
    voteList.clear();
    canVote     = false;
    hasVote     = true;
    playerCount = getPlayerCount();
    if (config.Basic.SendBroadcast) {
        Helper::broadcastMessage(I18nAPI::get("cleaner.vote.voteMessage", {pl->getRealName()}));
    }
    if (config.Basic.SendToast) {
        Helper::broadcastToast(I18nAPI::get("cleaner.vote.voteMessage", {pl->getRealName()}));
    }
    sendVoteForm(pl);
    auto cooldown = std::chrono::seconds::duration(config.VoteClean.Cooldown);
    auto waitTime = std::chrono::seconds::duration(config.VoteClean.CheckDelay);
    Cleaner::Entry::getInstance().getScheduler().add<DelayTask>(cooldown, [] { canVote = false; });
    Cleaner::Entry::getInstance().getScheduler().add<DelayTask>(waitTime, [] { checkVote(); });
}

void confirmForm(Player* pl) {
    auto fm = ll::form::ModalForm(
        I18nAPI::get("cleaner.vote.title"),
        I18nAPI::get("cleaner.vote.confirmTubtitle"),
        I18nAPI::get("cleaner.vote.confirmOk"),
        I18nAPI::get("cleaner.vote.confirmNo")
    );
    fm.sendTo(*pl, [](Player& player, ll::form::ModalFormResult result, ll::form::FormCancelReason reason) {
        if (result.has_value()) {
            switch (result.value()) {
            case ll::form::ModalFormSelectedButton::Upper: {
                return voteClean(&player);
            }
            case ll::form::ModalFormSelectedButton::Lower: {
                return player.sendMessage(I18nAPI::get("cleaner.vote.cancel"));
            }
            default:
                return;
            }
        }
    });
}

void voteCommandExecute(Player* pl) {
    if (!hasVote) {
        if (canVote) {
            confirmForm(pl);
        } else {
            pl->sendMessage(I18nAPI::get("cleaner.vote.cooldown"));
        }
    } else {
        if (voteList.count(pl->getUuid())) {
            pl->sendMessage(I18nAPI::get("cleaner.vote.voted"));
        } else {
            voteList[pl->getUuid()] = true;
            pl->sendMessage(I18nAPI::get("cleaner.vote.accept"));
        }
    }
}

} // namespace VoteClean