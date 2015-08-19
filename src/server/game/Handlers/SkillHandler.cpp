/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2014 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
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

#include "Common.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Opcodes.h"
#include "Player.h"
#include "Pet.h"
#include "UpdateMask.h"
#include "WorldPacket.h"
#include "WorldSession.h"

void WorldSession::HandeSetTalentSpecialization(WorldPacket& recvData)
{
    uint32 tab = recvData.read<uint32>();
    uint8 classId = _player->getClass();

    // Avoid cheat - hack
    if (_player->GetSpecializationId(_player->GetActiveSpec()))
        return;

    uint32 specializationId = 0;
    uint32 specializationSpell = 0;

    for (uint32 i = 0; i < sChrSpecializationStore.GetNumRows(); i++)
    {
        ChrSpecializationEntry const* specialization = sChrSpecializationStore.LookupEntry(i);
        if (!specialization)
            continue;

        if (specialization->classId == classId && specialization->TabPage == tab)
        {
            specializationId = specialization->Id;
            specializationSpell = specialization->specializationSpell;
            break;
        }
    }

    if (specializationId)
    {
        _player->SetSpecializationId(_player->GetActiveSpec(), specializationId);
        _player->SendTalentsInfoData(false);
        if (specializationSpell)
            _player->learnSpell(specializationSpell, false);
        
        _player->InitStatsForLevel(true);
        _player->UpdateMastery();
        _player->InitSpellForLevel();
    }
    _player->SaveToDB();
}

void WorldSession::HandleLearnTalentOpcode(WorldPacket& recvData)
{
    uint32 talentCount = recvData.ReadBits(23);
    uint16 talentId;
    bool anythingLearned = false;
    for (int i = 0; i != talentCount; i++)
    {
        recvData >> talentId;
        if (_player->LearnTalent(talentId))
            anythingLearned = true;
    }

    if (anythingLearned)
        _player->SendTalentsInfoData();
}

void WorldSession::HandleTalentWipeConfirmOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_RESET_TALENTS_RESPONSE");
   
    if (Player* player = GetPlayer())
    {
        player->ResetTalents();
        player->ResetSpec();
        player->SendTalentsInfoData();
        player->CastSpell(_player, 14867, true);
    }

    // We have to find a good structure, with this opcode is possible reset talents or spec or glyphs
    // This structure is wrong
    /*
        ObjectGuid guid;
        uint8 specializationReset;
        uint32 cost;

        recvData.ReadGuidMask(guid, 5, 7, 3, 2, 1, 0, 4, 6);

        recvData.ReadGuidBytes(guid, 1, 0);

        recvData >> specializationReset;

        recvData.ReadGuidBytes(guid, 7, 3, 2, 5, 6, 4);

        recvData >> cost;

        Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_TRAINER);
        if (!unit)
        {
            TC_LOG_DEBUG("network", "WORLD: HandleTalentWipeConfirmOpcode - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(guid)));
            return;
        }

        // remove fake death
        if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
            GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

        if (!specializationReset)
        {
            if (!_player->ResetTalents())
            {
                ObjectGuid Guid = guid;

                WorldPacket data(MSG_TALENT_WIPE_CONFIRM, 8 + 4);    //you have not any talent

                data.WriteGuidMask(Guid, 5, 7, 3, 2, 1, 0, 4, 6);

                data.WriteGuidBytes(Guid, 1, 0);
                data << uint8(0);
                data.WriteGuidBytes(Guid, 7, 3, 2, 5, 6, 4);
                data << uint32(0);
                SendPacket(&data);
                return;
            }
        }
        else
        {
            _player->ResetSpec();
        }

        _player->SendTalentsInfoData();
        unit->CastSpell(_player, 14867, true);                  //spell: "Untalent Visual Effect"
    */
}

void WorldSession::HandleUnlearnSkillOpcode(WorldPacket& recvData)
{
    uint32 skillId;
    recvData >> skillId;

    SkillRaceClassInfoEntry const* rcEntry = GetSkillRaceClassInfo(skillId, GetPlayer()->getRace(), GetPlayer()->getClass());
    if (!rcEntry || !(rcEntry->Flags & SKILL_FLAG_UNLEARNABLE))
        return;

    GetPlayer()->SetSkill(skillId, 0, 0, 0);
}
