#include "$CurrentDir:\\mpmissions\\DayZSurvival.chernarusplus\\ScriptedMods\\ModuleManager.c"
#include "$CurrentDir:\\mpmissions\\DayZSurvival.chernarusplus\\ScriptedMods\\Tunables.c"
#include "$CurrentDir:\\mpmissions\\DayZSurvival.chernarusplus\\ScriptedMods\\Modules\\AdminTool\\AdminTool.c"
#include "$CurrentDir:\\mpmissions\\DayZSurvival.chernarusplus\\ScriptedMods\\Modules\\AdvancedLoadouts\\AdvancedLoadouts.c"
#include "$CurrentDir:\\mpmissions\\DayZSurvival.chernarusplus\\ScriptedMods\\Modules\\Misc\\BuildingSpawner.c"
#include "$CurrentDir:\\mpmissions\\DayZSurvival.chernarusplus\\ScriptedMods\\Modules\\SafeZone\\SafeZoneFunctions.c"
#include "$CurrentDir:\\mpmissions\\DayZSurvival.chernarusplus\\ScriptedMods\\Modules\\ServerEvents\\InfectedHordes.c"

//#include "$CurrentDir:\\mpmissions\\DayZSurvival.chernarusplus\\ScriptedMods\\Modules\\Misc\\MOTDMessages.c"

class DayZSurvival : MissionServer
{
	private ref set<ref ModuleManager> m_Modules;
	ref InfectedHordes m_ZombieEvents;
	protected float m_LogInTimerLength = 1;     //in seconds the spawn timer when players login!
	bool m_StaminaStatus = false;

	void DayZSurvival()
	{
		Print("VANILLA PLUS PLUS IS ALIVE!!");
		m_Modules = new set<ref ModuleManager>;
		RegisterModules();
	}
	
	void ~DayZSurvival()
	{
		
	}
	
	void RegisterModules()
	{
		m_Modules.Insert(new ModTunables(this));
		
		if (ModTunables.Cast(GetModule(ModTunables)).IsActive("AdminTools"))
		{
			m_Modules.Insert(new AdminTool(this));
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActive("AdvancedLoadouts"))
		{
			m_Modules.Insert(new AdvancedLoadouts(this));
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActive("SafeZone"))
		{
			m_Modules.Insert(new SafeZone(this));
		}
	}
	
	void InitModules()
	{
		for ( int i = 0; i < m_Modules.Count(); ++i)
		{
			m_Modules.Get(i).Init();
		}
	}
	
	ModuleManager GetModule(typename moduleType)
	{
		for ( int i = 0; i < m_Modules.Count(); ++i)
		{
			ModuleManager module = m_Modules.Get(i);
			if (module.GetModuleType() == moduleType) 
			{
				return module;
			}
		}
		return NULL;
	}
	
	
	override void OnInit()
	{
		super.OnInit();
		InitModules();
		
		if (!ModTunables.Cast(GetModule(ModTunables)).IsActiveMisc("Debugmode"))
		{
			Hive ce = CreateHive();
			if (ce)
			ce.InitOffline();
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActiveMisc("ProxyExportMode"))
		{
			CETesting TestHive = GetTesting();
			TestHive.ExportProxyProto();
			TestHive.ExportProxyData( "7500 0 7500", 15000 );
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActiveMisc("SessionFeed"))
		{
			g_Game.SetProfileString("SessionFeed", "true");
		}
		else
		{
			g_Game.SetProfileString("SessionFeed", "false");
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActiveMisc("CustomBuildings"))
		{
			ref BuildingSpawner bldspnwer = new BuildingSpawner;
			bldspnwer.Init();
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActiveMisc("StaminaStatus"))
		{
			m_StaminaStatus = true; //Disable Stamina
		}
		
		if (ModTunables.Cast(GetModule(ModTunables)).IsActive("InfectedHordes"))
		{
			m_ZombieEvents = new InfectedHordes;
		}
		
		//-----------
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.PlayerCounter, 110000, true);  //Default 120000 2 mins Looped
		//GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.CustomMOTD, TIME_INTERVAL, true);  //Default 120000 2 mins Looped
		//-----------
	}

	override void OnPreloadEvent(PlayerIdentity identity, out bool useDB, out vector pos, out float yaw, out int queueTime)
	{
		if (GetHive())
		{
			useDB = true;
			queueTime = m_LogInTimerLength;
		}
		else
		{
			useDB = false;
			pos = "7500 0 7500";
			yaw = 0;
			queueTime = m_LogInTimerLength;
		}
	}

	override void TickScheduler(float timeslice)
	{
		GetGame().GetWorld().GetPlayerList(m_Players);
		if( m_Players.Count() == 0 ) return;
		for(int i = 0; i < SCHEDULER_PLAYERS_PER_TICK; i++)
		{
			if(m_currentPlayer >= m_Players.Count() )
			{
				m_currentPlayer = 0;
			}

			PlayerBase currentPlayer = PlayerBase.Cast(m_Players.Get(m_currentPlayer));
			currentPlayer.OnTick();

			if (m_StaminaStatus) {
			currentPlayer.GetStaminaHandler().SyncStamina(1000,1000);
            currentPlayer.GetStatStamina().Set(currentPlayer.GetStaminaHandler().GetStaminaCap());
			}
			if (GetModule(SafeZone)) { SafeZone.Cast(GetModule(SafeZone)).SafeZoneHandle(currentPlayer); }
			m_currentPlayer++;
		}
	}

	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		Entity playerEnt;

		playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");
		Class.CastTo(m_player, playerEnt);
		
		GetGame().SelectPlayer(identity, m_player);
		return m_player;
	}
	
	override void OnEvent(EventType eventTypeId, Param params) 
	{
		super.OnEvent(eventTypeId,params);
		
		switch(eventTypeId)
		{
			case ChatMessageEventTypeID:
				 ChatMessageEventParams chat_params = ChatMessageEventParams.Cast(params);
				 if (chat_params.param1 == 0 && chat_params.param2 != "") //trigger only when channel is Global == 0 and Player Name does not equal to null
					{
						Param4<int,string,string,string> request_info = new Param4<int,string,string,string>(chat_params.param1, chat_params.param2, chat_params.param3, chat_params.param4);
						AdminTool.Cast(GetModule(AdminTool)).RequestHandler(request_info); //Send the param to Admintools
					}
			break;
		}
	}
	
	void GlobalMessage(int Channel, string Message)
	{
		if (Message != "")
		{
			GetGame().ChatPlayer(Channel,Message);
		}
	}

	void PlayerCounter()
	{
		array<Man> players = new array<Man>;
	    GetGame().GetPlayers( players );
	    int numbOfplayers = players.Count();
	    GlobalMessage(1,"Online Players: "+ numbOfplayers.ToString());
	}
	
	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		ItemBase itemBs;
		EntityAI itemEnt;
		
		if (GetModule(AdvancedLoadouts))
		{
			if (AdvancedLoadouts.Cast(GetModule(AdvancedLoadouts)).CheckTunables("StaticLoadouts"))
			{
				bool reqld = AdvancedLoadouts.Cast(GetModule(AdvancedLoadouts)).LoadRandomStaticLD(player);
			}
			else if (AdvancedLoadouts.Cast(GetModule(AdvancedLoadouts)).CheckTunables("RandomizedLoadouts"))
			{
				AdvancedLoadouts.Cast(GetModule(AdvancedLoadouts)).LoadRndGenLoadout(player);
			}
			else
			{
				//Vanilla
				itemEnt = player.GetInventory().CreateInInventory( "Rag" );
				itemBs = ItemBase.Cast(itemEnt);							
				itemBs.SetQuantity(6);
			}

			if (AdvancedLoadouts.Cast(GetModule(AdvancedLoadouts)).CheckTunables("SpawnArmed"))
			{
			    //Gun spawner Handle
				//SpawnGunIn( PlayerBase player, string ClassName, bool isPrimary, TstringArray Attachments, TstringArray Extras) NOTE: Set bool to 'true' IF weapon was primary
				int oRandValue = Math.RandomIntInclusive(0,2);
				switch(oRandValue.ToString())
				{
					case "0":
					AdvancedLoadouts.Cast(GetModule(AdvancedLoadouts)).SpawnGunIn( player , "fnx45", true, {"fnp45_mrdsoptic","PistolSuppressor"},{"mag_fnx45_15rnd","mag_fnx45_15rnd"} );
					break;

					case "1":
					AdvancedLoadouts.Cast(GetModule(AdvancedLoadouts)).SpawnGunIn( player , "CZ75", true, {"PistolSuppressor"} , {"Mag_CZ75_15Rnd","Mag_CZ75_15Rnd"} );
					break;

					case "2":
					AdvancedLoadouts.Cast(GetModule(AdvancedLoadouts)).SpawnGunIn( player , "makarovij70", true, {"PistolSuppressor"} , {"mag_ij70_8rnd","mag_ij70_8rnd"} );
					break;
				}
			}
		}
		else
		{
			//Vanilla
			/*
			player.RemoveAllItems();
			player.GetInventory().CreateInInventory("TTSKOPants");
			itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("TTsKOJacket_Camo");
			itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("CombatBoots_Black");
			itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("CombatKnife");
			itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("AssaultBag_Ttsko");
			itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Compass");
			itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("FNX45");
			itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
			itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("AmmoBox_45ACP_25rnd");
			itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("roadFlare");
			itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Canteen");
			itemBs = ItemBase.Cast(itemEnt);
			itemEnt = player.GetInventory().CreateInInventory("Rag");
			itemBs = ItemBase.Cast(itemEnt);
			itemBs.SetQuantity(6);
			itemEnt = player.GetInventory().CreateInInventory("SpaghettiCan_Opened");
			itemBs = ItemBase.Cast(itemEnt);
			itemEnt = player.GetInventory().CreateInInventory("SodaCan_Cola");
			itemBs = ItemBase.Cast(itemEnt);
			itemEnt = player.GetInventory().CreateInInventory("SodaCan_Cola");
			itemBs = ItemBase.Cast(itemEnt);
			*/
	player.RemoveAllItems();

	/*
	EntityAI itemEnt;
	ItemBase itemBs;
	*/
	
	switch (Math.RandomInt(0, 11)) {
		case 0:
		// Soldier
		player.GetInventory().CreateInInventory("TTSKOPants");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("TTsKOJacket_Camo");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("CombatBoots_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AssaultBag_Ttsko");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("CombatKnife");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Compass");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("FNX45");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AmmoBox_45ACP_25rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("roadFlare");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Canteen");
		itemBs = ItemBase.Cast(itemEnt);

		break;
		case 1:
		// Paramedic
		player.GetInventory().CreateInInventory("ParamedicPants_Green");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("ParamedicJacket_Green");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("DryBag_Green");
		itemBs = ItemBase.Cast(itemEnt);
				player.GetInventory().CreateInInventory("KitchenKnife");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("SalineBagIV");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("BandageDressing");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("CombatBoots_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("BloodTestKit");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("MakarovIJ70");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Mag_IJ70_8Rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AmmoBox_380_35rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("roadFlare");
		itemBs = ItemBase.Cast(itemEnt);

		break;
		
		case 2:
		// Office worker
		player.GetInventory().CreateInInventory("SlacksPants_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("WoolCoat_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AthleticShoes_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("ChildBag_Blue");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("KitchenKnife");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Paper");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("ThinFramesGlasses");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("MakarovIJ70");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Mag_IJ70_8Rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AmmoBox_380_35rnd");
		itemBs = ItemBase.Cast(itemEnt);
		//player.GetInventory().CreateInInventory("Barrel_Green");
		//itemBs = ItemBase.Cast(itemEnt);

		break;
		case 3:
		// Biker
		player.GetInventory().CreateInInventory("Jeans_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("RidersJacket_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("MotoHelmet_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("HikingBootsLow_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("DryBag_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Matchbox");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("KitchenKnife");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Pipe");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("FNX45");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AmmoBox_45ACP_25rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Lockpick");
		itemBs = ItemBase.Cast(itemEnt);

		break;
		case 4:
		// Hiker
		player.GetInventory().CreateInInventory("HikingJacket_Red");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("CargoPants_Blue");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("HikingBootsLow_Blue");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("MountainBag_Blue");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("KitchenKnife");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Compass");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("WaterBottle");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Hatchet");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Matchbox");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("MakarovIJ70");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Mag_IJ70_8Rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AmmoBox_380_35rnd");
		itemBs = ItemBase.Cast(itemEnt);

		break;
		case 5:
		// Cop
		player.GetInventory().CreateInInventory("PoliceJacket");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("PolicePants");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("CombatBoots_Grey");
		itemBs = ItemBase.Cast(itemEnt);		
		player.GetInventory().CreateInInventory("UKAssVest_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("DryBag_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Flashlight");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Battery9V");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("PersonalRadio");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("roadFlare");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("CZ75");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Mag_CZ75_15Rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AmmoBox_9x19_25rnd");
		itemBs = ItemBase.Cast(itemEnt);

		break;
		case 6:
		// Lumberjack
		player.GetInventory().CreateInInventory("Shirt_RedCheck");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Jeans_Blue");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("WoodAxe");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("MountainBag_Red");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("WorkingBoots_Brown");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("KitchenKnife");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Ushanka_Green");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Matchbox");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("MakarovIJ70");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Mag_IJ70_8Rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AmmoBox_380_35rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Compass");
		itemBs = ItemBase.Cast(itemEnt);


		break;
		case 7:
		// Hood
		player.GetInventory().CreateInInventory("TrackSuitPants_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("TrackSuitJacket_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AthleticShoes_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("DryBag_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("KitchenKnife");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("SodaCan_Kvass");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("MakarovIJ70");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Mag_IJ70_8Rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AmmoBox_380_35rnd");
		itemBs = ItemBase.Cast(itemEnt);
		//player.GetInventory().CreateInInventory("Barrel_Green");
		//itemBs = ItemBase.Cast(itemEnt);
		break;
		case 8:
		// Fireman
		player.GetInventory().CreateInInventory("FirefighterJacket_Beige");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("FirefightersPants_Beige");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("FirefightersHelmet_White");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("WorkingBoots_Yellow");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("DryBag_Yellow");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("KitchenKnife");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("FirefighterAxe");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("MakarovIJ70");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Mag_IJ70_8Rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AmmoBox_380_35rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("WaterBottle");
		itemBs = ItemBase.Cast(itemEnt);
		//player.GetInventory().CreateInInventory("Barrel_Green");
		//itemBs = ItemBase.Cast(itemEnt);
		break;
		case 9:
		// Kacknoob
		player.GetInventory().CreateInInventory("MiniDress_BlueChecker");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Skirt_Blue");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("MedicalScrubsHat_Blue");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Ballerinas_Blue");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("DryBag_Blue");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("FirefighterAxe");
		itemBs = ItemBase.Cast(itemEnt);
		//player.GetInventory().CreateInInventory("Barrel_Green");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("FNX45");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
		itemBs = ItemBase.Cast(itemEnt);    
		player.GetInventory().CreateInInventory("PistolOptic");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("PistolSuppressor");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AmmoBox_45ACP_25rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Rice");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("WaterBottle");
		itemBs = ItemBase.Cast(itemEnt);
		break;
		case 10:
		// Ritter
		player.GetInventory().CreateInInventory("FurImprovisedBag");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("GreatHelm");
		itemBs = ItemBase.Cast(itemEnt);
		//player.GetInventory().CreateInInventory("LeatherJacket_Natural");
		//itemBs = ItemBase.Cast(itemEnt);
		//player.GetInventory().CreateInInventory("LeatherPants_Natural");
		//itemBs = ItemBase.Cast(itemEnt);
		//player.GetInventory().CreateInInventory("LeatherMoccasinsShoes_Natural");
		//itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Sword");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("HumanSteakMeat");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("HumanSteakMeat");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("MakarovIJ70");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Mag_IJ70_8Rnd");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AmmoBox_380_35rnd");
		itemBs = ItemBase.Cast(itemEnt);
		/*
		player.GetInventory().CreateInInventory("Ammo_ArrowBoned");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Ammo_ArrowBoned");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("Ammo_ArrowBoned");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("RecurveBow");
		itemBs = ItemBase.Cast(itemEnt);
		*/

		break;

	}
	// Give universal gear
	itemEnt = player.GetInventory().CreateInInventory("Rag");
	itemBs = ItemBase.Cast(itemEnt);
	itemBs.SetQuantity(6);
	itemEnt = player.GetInventory().CreateInInventory("SpaghettiCan_Opened");
	itemBs = ItemBase.Cast(itemEnt);
	itemEnt = player.GetInventory().CreateInInventory("SodaCan_Cola");
	itemBs = ItemBase.Cast(itemEnt);
	itemEnt = player.GetInventory().CreateInInventory("SodaCan_Cola");
	itemBs = ItemBase.Cast(itemEnt);

		}
	}
}