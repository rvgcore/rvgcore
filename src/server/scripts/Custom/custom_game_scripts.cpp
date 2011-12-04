/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptPCH.h"
#include "MapManager.h"
#include "Chat.h"
#include "ObjectMgr.h"
#include "AccountMgr.h"

class custom_gamescript : public CommandScript
{
public:
	custom_gamescript() : CommandScript("misc_commandscript") { }

	ChatCommand* GetCommands() const
	{
		static ChatCommand commandTable[] =
		{
				{ "gamble",         SEC_GAMEMASTER,         false, &HandleGambleCommand,              "", NULL },
				{ "roulette",       SEC_GAMEMASTER,         false, &HandleRouletteCommand,            "", NULL },
				{ NULL,             0,                  false,  NULL,                       "", NULL }
		};
		return commandTable;
	}

	// Allows your players to gamble for fun and prizes
	static bool HandleGambleCommand(ChatHandler* handler, const char* args)
	{
		Player *chr = handler->GetSession()->GetPlayer();

		char* px = strtok((char*)args, " ");

		if (!px)
			return false;
		uint32 money = (uint32)atoi(px);

		if (chr->GetMoney() < money)
		{
			handler->SendSysMessage("You can not bet with money you do not have!");
			return true;
		}

		else
		{
			if (money>0)
			{
				if (rand()%100 < 50)
				{
					chr->ModifyMoney(money*2);
					handler->SendSysMessage("You have won and doubled your bet");
				}
				else
				{
					chr->ModifyMoney(-int(money));
					handler->SendSysMessage("You have lost");
				}
			}
		}

		return true;
	}



	static bool HandleRouletteCommand(ChatHandler* handler, const char* args)
	{
		Player *chr = handler->GetSession()->GetPlayer();

		char* px = strtok((char*)args, " ");

		if (!px)
			return false;

		uint32 money = (uint32)atoi(px);

		if (chr->GetMoney() < money)
		{
			return true;
		}

		else
		{
			if (money>0)
			{
				if (rand()%36 < 1)
				{
					chr->ModifyMoney(money*36);
					handler->SendSysMessage("You have won 36 times your bet, congratulations!");
				}
				else
				{
					chr->ModifyMoney(-int(money));
					handler->SendSysMessage("You have lost");
				}
			}
		}

		return true;
	}


};

void AddSC_custom_gamescript()
{
	new custom_gamescript();
}
