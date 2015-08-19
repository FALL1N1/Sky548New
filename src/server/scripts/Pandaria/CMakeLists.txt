# Copyright (C) 2011-2014 Project SkyFire <http://www.projectskyfire.org/>
# Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

set(scripts_STAT_SRCS
  ${scripts_STAT_SRCS}
  
  # Dungeons
  Pandaria/Dungeons/MoguShanPalace/instance_mogushan_palace.cpp
  Pandaria/Dungeons/MoguShanPalace/mogushan_palace.h
  Pandaria/Dungeons/MoguShanPalace/boss_gekkan.cpp
  Pandaria/Dungeons/MoguShanPalace/boss_trial_of_king.cpp
  Pandaria/Dungeons/MoguShanPalace/boss_xin_the_weaponmaster.cpp
  Pandaria/Dungeons/ShadoPanMonastery/shadopan_monastery.h
  Pandaria/Dungeons/ShadoPanMonastery/instance_shadopan_monastery.cpp
  Pandaria/Dungeons/ShadoPanMonastery/boss_gu_cloudstrike.cpp
  Pandaria/Dungeons/ShadoPanMonastery/boss_master_snowdrift.cpp
  Pandaria/Dungeons/ShadoPanMonastery/boss_sha_of_violence.cpp
  Pandaria/Dungeons/ShadoPanMonastery/boss_taran_zhu.cpp
  Pandaria/Dungeons/TempleoftheJadeSerpent/instance_temple_of_the_jade_serpent.cpp
  Pandaria/Dungeons/TempleoftheJadeSerpent/boss_liu_flameheart.cpp
  Pandaria/Dungeons/TempleoftheJadeSerpent/boss_lorewalker_stonestep.cpp
  Pandaria/Dungeons/TempleoftheJadeSerpent/boss_sha_of_doubt.cpp
  Pandaria/Dungeons/TempleoftheJadeSerpent/boss_wise_mari.cpp
  Pandaria/Dungeons/StormstoutBrewery/stormstout_brewery.h
  Pandaria/Dungeons/StormstoutBrewery/instance_stormstout_brewery.cpp
  Pandaria/Dungeons/StormstoutBrewery/boss_hoptallus.cpp
  Pandaria/Dungeons/StormstoutBrewery/boss_ook_ook.cpp
  Pandaria/Dungeons/StormstoutBrewery/boss_yan_zhu.cpp
  Pandaria/Dungeons/GateoftheSettingSun/gate_of_the_setting_sun.h
  Pandaria/Dungeons/GateoftheSettingSun/instance_gate_of_the_setting_sun.cpp
  Pandaria/Dungeons/GateoftheSettingSun/boss_commander_rimok.cpp
  Pandaria/Dungeons/GateoftheSettingSun/boss_raigonn.cpp
  Pandaria/Dungeons/GateoftheSettingSun/boss_saboteur_kiptilak.cpp
  Pandaria/Dungeons/GateoftheSettingSun/boss_striker_gadok.cpp
  Pandaria/Dungeons/SiegeofNiuzaoTemple/siege_of_niuzao_temple.h
  Pandaria/Dungeons/SiegeofNiuzaoTemple/instance_siege_of_niuzao_temple.cpp
  Pandaria/Dungeons/SiegeofNiuzaoTemple/boss_commander_vojak.cpp
  Pandaria/Dungeons/SiegeofNiuzaoTemple/boss_general_pavalak.cpp
  Pandaria/Dungeons/SiegeofNiuzaoTemple/boss_vizier_jinbak.cpp
  Pandaria/Dungeons/SiegeofNiuzaoTemple/boss_wing_leader_neronok.cpp
  Pandaria/Dungeons/ScarletHalls/scarlet_halls.h
  Pandaria/Dungeons/ScarletHalls/instance_scarlet_halls.cpp
  Pandaria/Dungeons/ScarletHalls/boss_armsmaster_harlan.cpp
  Pandaria/Dungeons/ScarletHalls/boss_flameweaver_koegler.cpp
  Pandaria/Dungeons/ScarletHalls/boss_houndmaster_braun.cpp
  Pandaria/Dungeons/ScarletMonastery/scarlet_monastery.h
  Pandaria/Dungeons/ScarletMonastery/instance_scarlet_monastery.cpp
  Pandaria/Dungeons/ScarletMonastery/boss_brother_korloff.cpp
  Pandaria/Dungeons/ScarletMonastery/boss_high_inquisitor_whitemane.cpp
  Pandaria/Dungeons/ScarletMonastery/boss_thalnos_the_souldrender.cpp
  Pandaria/Dungeons/Scholomance/scholomance.h
  Pandaria/Dungeons/Scholomance/instance_scholomance.cpp
  Pandaria/Dungeons/Scholomance/boss_darkmaster_gandling.cpp
  Pandaria/Dungeons/Scholomance/boss_instructor_chillheart.cpp
  Pandaria/Dungeons/Scholomance/boss_jandice_barov.cpp
  Pandaria/Dungeons/Scholomance/boss_lilian_voss.cpp
  Pandaria/Dungeons/Scholomance/boss_rattlegore.cpp

  # Raids
  Pandaria/Raids/HeartOfFear/instance_heart_of_fear.cpp
  Pandaria/Raids/HeartOfFear/heart_of_fear.h
  Pandaria/Raids/HeartOfFear/boss_ambershaper_unsok.cpp
  Pandaria/Raids/HeartOfFear/boss_blade_lord_tayak.cpp
  Pandaria/Raids/HeartOfFear/boss_garalon.cpp
  Pandaria/Raids/HeartOfFear/boss_grand_empress_shekzeer.cpp
  Pandaria/Raids/HeartOfFear/boss_imperial_vizier_zorlok.cpp
  Pandaria/Raids/HeartOfFear/boss_wind_lord_meljarak.cpp
  Pandaria/Raids/MogushanVault/mogu_shan_vault.cpp
  Pandaria/Raids/MogushanVault/mogu_shan_vault.h
  Pandaria/Raids/MogushanVault/instance_mogu_shan_vault.cpp
  Pandaria/Raids/MogushanVault/boss_will_of_emperor.cpp
  Pandaria/Raids/MogushanVault/boss_stone_guard.cpp
  Pandaria/Raids/MogushanVault/boss_spirit_kings.cpp
  Pandaria/Raids/MogushanVault/boss_garajal.cpp
  Pandaria/Raids/MogushanVault/boss_feng.cpp
  Pandaria/Raids/MogushanVault/boss_elegon.cpp
  Pandaria/Raids/TerraceOfEndlessSpring/terrace_of_the_endless_spring.cpp
  Pandaria/Raids/TerraceOfEndlessSpring/terrace_of_the_endless_spring.h
  Pandaria/Raids/TerraceOfEndlessSpring/boss_tsulong.cpp
  Pandaria/Raids/TerraceOfEndlessSpring/boss_sha_of_fear.cpp
  Pandaria/Raids/TerraceOfEndlessSpring/boss_protectors_of_the_endless.cpp
  Pandaria/Raids/TerraceOfEndlessSpring/boss_lei_shi.cpp
  Pandaria/Raids/ThroneOfThunder/boss_Council.cpp
  Pandaria/Raids/ThroneOfThunder/boss_DarkAnimus.cpp
  Pandaria/Raids/ThroneOfThunder/boss_Durumu.cpp
  Pandaria/Raids/ThroneOfThunder/boss_Horridon.cpp
  Pandaria/Raids/ThroneOfThunder/boss_IronQon.cpp
  Pandaria/Raids/ThroneOfThunder/boss_JiKun.cpp
  Pandaria/Raids/ThroneOfThunder/boss_Jinrokh.cpp
  Pandaria/Raids/ThroneOfThunder/boss_Megaera.cpp
  Pandaria/Raids/ThroneOfThunder/boss_Primordius.cpp
  Pandaria/Raids/ThroneOfThunder/boss_Raden.cpp
  Pandaria/Raids/ThroneOfThunder/boss_Tortos.cpp
  Pandaria/Raids/ThroneOfThunder/boss_TwinConsorts.cpp
  Pandaria/Raids/ThroneOfThunder/boss_LeiShen.cpp
  Pandaria/Raids/ThroneOfThunder/instance_ThroneOfThunder.cpp
  Pandaria/Raids/ThroneOfThunder/ThroneOfThunder.h
  Pandaria/Raids/SiegeOfOrgrimmar/boss_Immerseus.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_Galakras.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_GeneralNazgrim.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_KorKron.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_IronJuggernaut.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_Norushen.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_Malkorok.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_ParagonsOfTheKlaxxi.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_ShaOfPride.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_SiegecrafterBlackfuse.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_SpoilsOfPandaria.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_TheFallenProtectors.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_Thok.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/boss_GarroshHellscream.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/instance_SiegeOfOrgrimmar.cpp
  Pandaria/Raids/SiegeOfOrgrimmar/SiegeOfOrgrimmar.h

  # Timeless Island
  Pandaria/TimelessIsle/zone_timeless_isle.cpp
  Pandaria/TimelessIsle/timeless_isle.h
  Pandaria/TimelessIsle/timeless_isle.cpp
  Pandaria/TimelessIsle/boss_chi_ji.cpp

  # World Bosses
  Pandaria/WorldBosses/boss_ordos.cpp
  Pandaria/WorldBosses/boss_galion.cpp
  Pandaria/WorldBosses/boss_sha_of_anger.cpp
  Pandaria/WorldBosses/boss_oondasta.cpp
  Pandaria/WorldBosses/boss_nalak_the_storm_lord.cpp

  # Zones
  Pandaria/Zones/zone_dread_wastes.cpp
  Pandaria/Zones/zone_isle_of_giants.cpp
  Pandaria/Zones/zone_isle_of_thunder.cpp
  Pandaria/Zones/zone_krasarang_wilds.cpp
  Pandaria/Zones/zone_kun_lai_summit.cpp
  Pandaria/Zones/zone_the_jade_forest.cpp
  Pandaria/Zones/zone_the_veiled_stair.cpp
  Pandaria/Zones/zone_townlong_steppes.cpp
  Pandaria/Zones/zone_vale_of_eternal_blossoms.cpp
  Pandaria/Zones/zone_valley_of_the_four_winds.cpp

  # Wandering Island (Panda start zone)
  Pandaria/Zones/WanderingIsland/WanderingIsland_North.cpp
  Pandaria/Zones/WanderingIsland/WanderingIsland_East.cpp
  Pandaria/Zones/WanderingIsland/WanderingIsland_West.cpp
  Pandaria/Zones/WanderingIsland/WanderingIsland_South.cpp
)

message("  -> Prepared: Pandaria")
