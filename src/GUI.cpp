#include "Global.h"
#include <llapi/FormUI.h>
#include "setting.h"
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Level.hpp>
using namespace LanguageSettings;
using namespace Form;

string finished;

extern bool checkcanvote;
extern bool checktimeout;
extern int ac_count;
extern string before;

extern void _CleanerMain();

void ConfirmForm(Player* player){
    ModalForm form(guitit, reask, reok, reno);  
    form.sendTo(player, [](Player* player, bool isConfirm){   
        if(isConfirm){
            if(checktimeout = true){
                ac_count++;
                player->sendText(reac);
            }
            else{
                player->sendText(reout);
            }
        }                                       
        else{
            if (checktimeout = true) {
                player->sendText(rede);
            }
            else{
                player->sendText(reout);
            }
        }                                                          
    });
}

void VoteForm(Player* player){
    ModalForm form(guitit, guiqus, sendok, sendno);     
    form.sendTo(player, [](Player* player, bool isConfirm){
        if(isConfirm){
            int t;
            auto players = Global<Level>->getAllPlayers();
            for(t = 0; t <= players.size()-1; t++){
                ConfirmForm(players[t]);
            }
            ac_count = 0;
            checktimeout = true;
            checkcanvote = false;
            Schedule::delay([]{
                checkcanvote = true;
            }, 20*Settings::cd);
            Schedule::delay([]{
                auto all = Global<Level>->getAllPlayers().size();
                int caculate = 100*ac_count/all;
                checktimeout = false;
                if (caculate >= Settings::percentage) {
                    Level::broadcastText(before + votesuccess, TextType::RAW);
                    _CleanerMain();
                }
                else {
                    Level::broadcastText(before + votefail, TextType::RAW);
                }
            }, 20*Settings::votedelay);
        }                                       
        else{
            player->sendText(sendcancel, TextType::RAW);
        }                                                          
    });
}