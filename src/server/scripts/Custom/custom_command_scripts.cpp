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

class custom_commandscript : public CommandScript
{
public:
	custom_commandscript() : CommandScript("misc_commandscript") { }

	ChatCommand* GetCommands() const
	{
		static ChatCommand buffCommandTable[] =
		{
				{ "name",           SEC_GAMEMASTER,      false, &HandleBuffNameCommand,            "", NULL },
				{ "all",			SEC_GAMEMASTER,      false, &HandleBuffAllCommand,             "", NULL },
				{ "",               SEC_GAMEMASTER,      false, NULL,                              "", NULL },
				{ NULL,             0,                  false, NULL,                              "", NULL }
		};
		static ChatCommand commandTable[] =
		{
				{ "buff",           SEC_GAMEMASTER,      false, NULL,                   "", buffCommandTable },
				{ "sakura",         SEC_GAMEMASTER,  	 false, &HandleSakuraCommand,    "", NULL },
				{ NULL,             0,                  false,  NULL,                       "", NULL }
		};
		return commandTable;
	}

	// buff player with given game_buff.entry
	static bool HandleBuffNameCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		char* nameStr = strtok((char*)args, " ");
		Player* target;
		uint64 target_guid;
		std::string target_name;

		if (!handler->extractPlayerTarget(nameStr, &target, &target_guid, &target_name))
			return false;

		if (target)
		{
			if (handler->HasLowerSecurity(target, 0))
				return false;

			std::string chrNameLink = handler->playerLink(target_name);
			target->CastSpell(target, 24425, false); //Spirit of Zandalar
			target->CastSpell(target, 22888, false); //Rallying Cry
			target->CastSpell(target, 35076, false); //Blessing of A'dal
			target->CastSpell(target, 27571, false); //Cascade of Roses
		}
		else
		{
			return false;
		}

		return true;
	}

	// buff all online players
	static bool HandleBuffAllCommand(ChatHandler* handler, const char* /*args*/)
	{
		Player* target;
		uint64 target_guid;
		std::string target_name;

		QueryResult resultDB = CharacterDatabase.Query("SELECT name FROM characters WHERE online > 0");
		if (!resultDB)
		{
			handler->SendSysMessage(LANG_ACCOUNT_LIST_EMPTY);
			return true;
		}


		do
		{
			Field* fieldsDB = resultDB->Fetch();
			std::string str = fieldsDB[0].GetString();
			std::vector<char> writable(str.begin(), str.end());
			writable.push_back('\0');
			char* nameStr = &writable[0];


			if (!handler->extractPlayerTarget(nameStr, &target, &target_guid, &target_name))
				return false;
			if (handler->HasLowerSecurity(target,0))
				continue;

			target->CastSpell(target, 24425, false);
		}while (resultDB->NextRow());



		return true;
	}


	static bool HandleSakuraCommand(ChatHandler* handler, const char* args)
	{

	    if (!*args)
	        return false;

	    Player* player = handler->GetSession()->GetPlayer();

	    char* arg1 = strtok((char*)args, " ");
	    char* arg2 = strtok(NULL, " ");

	    if (! arg1)
	        return false;

	    if (! arg2)
	        return false;

	    char dir = arg1[0];
	    uint32 value = (int)atoi(arg2);
	    float x = player->GetPositionX();
	    float y = player->GetPositionY();
	    float z = player->GetPositionZ();
	    float o = player->GetOrientation();
	    uint32 mapid = player->GetMapId();
	    Map const *spawnmap = sMapMgr->CreateBaseMap(mapid);

	    if (!MapManager::IsValidMapCoord(mapid,x,y,z))
	    {
	    	handler->SendSysMessage(LANG_INVALID_TARGET_COORD);
	    	handler->SetSentErrorMessage(true);
	        return false;
	    }
	    // stop flight if need
	    if (player->isInFlight())
	    {
	        player->GetMotionMaster()->MovementExpired();
	        player->CleanupAfterTaxiFlight();
	    }
	    // save only in non-flight case
	    else
	        player->SaveRecallPosition();

	    switch (dir)
	    {
	    case 'u':
	        {
	            player->TeleportTo(mapid, x, y, z + value, o);
	        }
	        break;
	    case 'd':
	        {
	            player->TeleportTo(mapid, x, y, z - value, o);
	        }
	        break;
	    case 'f':
	        {
	            float fx = x + cosf(o)*value;
	            float fy = y + sinf(o)*value;
	            float fz = std::max(spawnmap->GetHeight(fx, fy, MAX_HEIGHT), spawnmap->GetWaterLevel(fx, fy));
	            player->TeleportTo(mapid, fx, fy, fz, o);
	        }
	        break;
	    case 'b':
	        {
	            float bx = x - cosf(o)*value;
	            float by = y - sinf(o)*value;
	            float bz = std::max(spawnmap->GetHeight(bx, by, MAX_HEIGHT), spawnmap->GetWaterLevel(bx, by));
	            player->TeleportTo(mapid, bx, by, bz, o);
	        }
	        break;
	    }
	    return true;
	}


};

void AddSC_custom_commandscript()
{
	new custom_commandscript();
}
