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
        tr("cleaner.vote.title"),
        tr("cleaner.vote.subtitle", {pl->getRealName()}),
        tr("cleaner.vote.ok"),
        tr("cleaner.vote.no")
    );
    ll::service::getLevel()->forEachPlayer([&](Player& pl) -> bool {
        fm.sendTo(pl, [](Player& player, ll::form::ModalFormResult result, ll::form::FormCancelReason reason) {
            if (result.has_value()) {
                switch (result.value()) {
                case ll::form::ModalFormSelectedButton::Upper: {
                    voteList[player.getUuid()] = true;
                    player.sendMessage(tr("cleaner.vote.accept"));
                    return;
                }
                case ll::form::ModalFormSelectedButton::Lower: {
                    voteList[player.getUuid()] = false;
                    player.sendMessage(tr("cleaner.vote.deny"));
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
    float percentage = Config->getValue<float>({"VoteClean", "Percentage"}, 50.0f) / 100.0f;
    int   voteCount  = 0;
    for (auto& key : voteList) {
        if (key.second == true) {
            voteCount++;
        }
    }
    float result = ((float)voteCount) / ((float)playerCount);
    if (result >= percentage) {
        if (ConfigFile::mAnnounce) {
            Helper::broadcastMessage(tr("cleaner.vote.succeed"));
        }
        if (ConfigFile::mSendToast) {
            Helper::broadcastToast(tr("cleaner.vote.succeed"));
        }
        Cleaner::CleanTask();
    } else {
        if (ConfigFile::mAnnounce) {
            Helper::broadcastMessage(tr("cleaner.vote.failed"));
        }
        if (ConfigFile::mSendToast) {
            Helper::broadcastToast(tr("cleaner.vote.failed"));
        }
    }
    hasVote     = false;
    playerCount = 0;
}

void voteClean(Player* pl) {
    voteList.clear();
    canVote     = false;
    hasVote     = true;
    playerCount = getPlayerCount();
    if (ConfigFile::mAnnounce) {
        Helper::broadcastMessage(tr("cleaner.vote.voteMessage", {pl->getRealName()}));
    }
    if (ConfigFile::mSendToast) {
        Helper::broadcastToast(tr("cleaner.vote.voteMessage", {pl->getRealName()}));
    }
    sendVoteForm(pl);
    auto cooldown = std::chrono::seconds::duration(Config->getValue<int>({"VoteClean", "Cooldown"}, 120));
    auto waitTime = std::chrono::seconds::duration(Config->getValue<int>({"VoteClean", "CheckDelay"}, 30));
    Cleaner::scheduler.add<DelayTask>(cooldown, [] { canVote = false; });
    Cleaner::scheduler.add<DelayTask>(waitTime, [] { checkVote(); });
}

void confirmForm(Player* pl) {
    auto fm = ll::form::ModalForm(
        tr("cleaner.vote.title"),
        tr("cleaner.vote.confirmTubtitle"),
        tr("cleaner.vote.confirmOk"),
        tr("cleaner.vote.confirmNo")
    );
    fm.sendTo(*pl, [](Player& player, ll::form::ModalFormResult result, ll::form::FormCancelReason reason) {
        if (result.has_value()) {
            switch (result.value()) {
            case ll::form::ModalFormSelectedButton::Upper: {
                return voteClean(&player);
            }
            case ll::form::ModalFormSelectedButton::Lower: {
                return player.sendMessage(tr("cleaner.vote.cancel"));
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
            pl->sendMessage(tr("cleaner.vote.cooldown"));
        }
    } else {
        if (voteList.count(pl->getUuid())) {
            pl->sendMessage(tr("cleaner.vote.voted"));
        } else {
            voteList[pl->getUuid()] = true;
            pl->sendMessage(tr("cleaner.vote.accept"));
        }
    }
}

} // namespace VoteClean