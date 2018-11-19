//! channel, from, text, color config class
typedef Param4<int, string, string, string> AdminChatParams;

class AdminTool extends ModuleManager
{
	protected ref map<string, string> m_AdminList;
	protected ref map<string, vector> m_TPLocations;
	protected ref map<string,    int> m_ExtendedCommands;

	protected bool m_FreeCamera;
	protected bool m_IsDebugRunning = false;

	protected PlayerBase Admin;
	protected PlayerIdentity identityT;
	protected string PlayerUID;
	protected string GUID;
	
	protected string m_AdminListPath = "$CurrentDir:\\mpmissions\\DayZSurvival.chernarusplus\\ScriptedMods\\";
	
	override void Init()
	{
		Print("AdminTool:: Init():: Loading AdminTool Mods...");
		
		m_AdminList    = new map<string, string>; //UID, name
		m_TPLocations  = new map<string, vector>; //name of town, pos
		m_ExtendedCommands = new map<string,int>; //Command, length


		//-----Add Admins from txt-----
		FileHandle AdminUIDSFile = OpenFile(m_AdminListPath + "Admins.txt",FileMode.READ);
		if (AdminUIDSFile != 0)
		{
			string line_content = "";
			while ( FGets(AdminUIDSFile,line_content) > 0 )
			{
				m_AdminList.Insert(line_content,"null"); //UID , NAME
				Print("Adding Admin: "+ line_content + " To the Admin List!");
			}
			CloseFile(AdminUIDSFile);
		}
		
		//Add Towns to TP array
		m_TPLocations.Insert( "Severograd", "8428.0 0.0 12767.1" );
		m_TPLocations.Insert( "Krasnostav", "11172.0 0.0 12314.1" );
		m_TPLocations.Insert( "Mogilevka", "7537.8 0.0 5211.55" );
		m_TPLocations.Insert( "Stary", "6046.94 0.0 7733.97" );
		m_TPLocations.Insert( "Msta", "11322.55 0.0 5463.36" );
		m_TPLocations.Insert( "Vybor", "3784.16 0.0 8923.48" );
		m_TPLocations.Insert( "Gorka", "9514.27 0.0 8863.69" );
		m_TPLocations.Insert( "Solni", "13402.57 0.0 6303.35" );
		m_TPLocations.Insert( "NWAFS", "4540.52 0.0 9645.84" );
		m_TPLocations.Insert( "NWAFC", "4823.43 0.0 10457.16" );
		m_TPLocations.Insert( "NWAFN", "4214.84 0.0 10977.78" );
		m_TPLocations.Insert( "BAF", "4467.61 0.0 2496.14" );
		m_TPLocations.Insert( "NEAF", "11921.43 0.0 12525.55" );
		m_TPLocations.Insert( "ChernoC", "6649.22 0.0 2710.03" );
		m_TPLocations.Insert( "ChernoW", "6374.08 0.0 2361.01" );
		m_TPLocations.Insert( "ChernoE", "7331.70 0.0 2850.03" );
		m_TPLocations.Insert( "ElektroW", "10077.17 0.0 1988.65" );
		m_TPLocations.Insert( "ElektroE", "10553.55 0.0 2313.37" );
		m_TPLocations.Insert( "BerezC", "12319.54 0.0 9530.15" );
		m_TPLocations.Insert( "BerezS", "11991.42 0.0 9116.95" );
		m_TPLocations.Insert( "BerezN", "12823.14 0.0 10078.97" );
		m_TPLocations.Insert( "Svet", "13900.82 0.0 13258.12" );	
		m_TPLocations.Insert( "ZelenoS", "2572.80 0.0 5105.09" );
		m_TPLocations.Insert( "ZelenoN", "2741.48 0.0 5416.69" );
		m_TPLocations.Insert( "Lopatino", "2714.74 0.0 9996.33" );
		m_TPLocations.Insert( "Tisy", "1723.10 0.0 13983.88" );
		m_TPLocations.Insert( "Novaya", "3395.28 0.0 13013.61" );
		m_TPLocations.Insert( "Novy", "7085.73 0.0 7720.85" );
		m_TPLocations.Insert( "Grishino", "5952.15 0.0 10367.71" );
		m_TPLocations.Insert( "Kabanino", "5363.97 0.0 8594.39" );

		//Init Commands
		m_ExtendedCommands.Insert("/strip",6);
		m_ExtendedCommands.Insert("/tpm",5);
		m_ExtendedCommands.Insert("/tpp",4);
		m_ExtendedCommands.Insert("/tpto",5);
		m_ExtendedCommands.Insert("/spi",4);
		m_ExtendedCommands.Insert("/spg",4);
		m_ExtendedCommands.Insert("/tpc",4);
		m_ExtendedCommands.Insert("/akmkit",4) //arnie
		m_ExtendedCommands.Insert("/m4kit",4) //arnie
		m_ExtendedCommands.Insert("/foodkit",4) //arnie
		m_ExtendedCommands.Insert("/hannibal",4) //arnie
		//Sub commands
		m_ExtendedCommands.Insert("/export",1);
		m_ExtendedCommands.Insert("/ammo",1);
		m_ExtendedCommands.Insert("/stamina",1);
		m_ExtendedCommands.Insert("/LoadoutType",1);
		m_ExtendedCommands.Insert("/CustomLoadouts",1);
		m_ExtendedCommands.Insert("/SpawnArmed",1);
		m_ExtendedCommands.Insert("/updateLoadouts",1);
		m_ExtendedCommands.Insert("/freecam",1);
		m_ExtendedCommands.Insert("/debug",1);
		m_ExtendedCommands.Insert("/nighttime",1);
		m_ExtendedCommands.Insert("/daytime",1);
		m_ExtendedCommands.Insert("/godmode",1);
		m_ExtendedCommands.Insert("/heal",1);
		m_ExtendedCommands.Insert("/kill",1);
		m_ExtendedCommands.Insert("/tpalltome",1);
		m_ExtendedCommands.Insert("/killall",1);
		m_ExtendedCommands.Insert("/spawncar",1);
		//m_ExtendedCommands.Insert("/basecar",1);
	}
	
	void AdminTool( DayZSurvival missionServer )
	{

	}
	
	void ~AdminTool()
	{
		
	}

	int TeleportAllPlayersTo(PlayerBase Admin)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers( players );

		vector AdminPos;
		AdminPos = Admin.GetPosition();

		for ( int i = 0; i < players.Count(); ++i )
		{
			PlayerBase Target = players.Get(i);
			Target.SetPosition( AdminPos );
		}
		return i;
	}

	void oSpawnItemFunc(bool ground, PlayerBase player, string ClassName)
	{
		EntityAI item;
		ItemBase itemBs

		vector NewPosition;
		vector OldPosition;

		if (ground)
		{
			OldPosition = player.GetPosition();

			NewPosition[0] = OldPosition[0] + 1.5;
			NewPosition[1] = OldPosition[1] + 0.1;
			NewPosition[2] = OldPosition[2] + 1.5;

			item = GetGame().CreateObject( ClassName, NewPosition, false, true );
		}else{

			item = player.GetInventory().CreateInInventory( ClassName );
			itemBs = ItemBase.Cast(item);	
			itemBs.SetQuantity(1);
		}
	}

	vector SnapToGround(vector pos)
	{
		float pos_x = pos[0];
		float pos_z = pos[2];
		float pos_y = GetGame().SurfaceY( pos_x, pos_z );
		vector tmp_pos = Vector( pos_x, pos_y, pos_z );
		tmp_pos[1] = tmp_pos[1] + pos[1];

		return tmp_pos;
	}

	ref array<string> CheckCommand(string CommandLine)
	{
		ref array<string> ret = new array<string>;
		string strRplce,mKey;
		int cmdLength;

		strRplce = CommandLine;

		for (int i = 0; i < m_ExtendedCommands.Count(); ++i)
		{
			mKey 	  = m_ExtendedCommands.GetKey(i);
			cmdLength = m_ExtendedCommands.Get(mKey);

			if (CommandLine.Contains(mKey))
			{
				strRplce.Replace(mKey + " ","");
				ret.Insert(mKey); //0 = Command 1 = Data
				if (strRplce != "")
				{
					ret.Insert(strRplce);
				}
				return ret;
			}
		}
		return NULL;
	}

	//---------------------------------------------------------------------------------
	void RequestHandler( Param request_info )
	{
		AdminChatParams chat_params = AdminChatParams.Cast(request_info);
		if (chat_params)
		{
			array<Man> players = new array<Man>;
			GetGame().GetPlayers( players );
			
			PlayerBase Admin;
			PlayerIdentity AdminIdentity;
			string AdminUID;

			PlayerBase selectedPlayer;
			PlayerIdentity selectedIdentity;
			string selectedUID;
			
			string chatLine = chat_params.param3;
			string strMessage;
			Param1<string> Msgparam;

			for (int i = 0; i < players.Count(); ++i)
			{
				if (players.Get(i).GetIdentity().GetName() == chat_params.param2 && m_AdminList.Contains(players.Get(i).GetIdentity().GetPlainId()))
				{
					Admin 		  = players.Get(i);
					AdminIdentity = Admin.GetIdentity();
					AdminUID 	  = AdminIdentity.GetPlainId();
				}
			}

			if (Admin && AdminUID != "")
			{
				if (chatLine.Contains("/"))
				{
					ref array<string> chatData = CheckCommand(chatLine);
					string cCommand, cData;
					if (chatData != NULL)
					{
						cCommand = chatData.Get(0);
						cData    = chatData.Get(1);
					} else { cCommand = "UnknownCommand" }

					switch(cCommand)
					{
						case "/strip":
								for ( int a = 0; a < players.Count(); ++a )
								{
									selectedPlayer = players.Get(a);
									selectedIdentity = selectedPlayer.GetIdentity();
									if ( selectedIdentity.GetName() == cData )
									{
										selectedPlayer.RemoveAllItems();
										Msgparam = new Param1<string>( "Player "+cData +" Stripped!" );
										GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
									}
								}
							break;

							case "/tpm":
								for ( int zm = 0; zm < players.Count(); ++zm )
								{
									if ( players.Get(zm).GetIdentity().GetName() == cData )
									{
										Admin.SetPosition(players.Get(zm).GetPosition());
										strMessage = "You were teleported to player " + cData;
										Msgparam = new Param1<string>( strMessage );
										GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
									}
								}
							break;

							case "/tpp":
								for ( int z = 0; z < players.Count(); ++z )
								{
									selectedPlayer = players.Get(z);
									selectedIdentity = selectedPlayer.GetIdentity();
									if ( selectedIdentity.GetName() == cData )
									{
										selectedPlayer.SetPosition(Admin.GetPosition());

										Msgparam = new Param1<string>( "You were teleported by the admin!" );
										GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, selectedIdentity);
											  
										strMessage = "Player " + cData + " was teleported to your location!";
										Msgparam = new Param1<string>( strMessage );
										GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
									}
								}
							break;

							case "/tpto":
								vector position = "0 0 0";
								if (m_TPLocations.Contains(cData))
								{
									m_TPLocations.Find( cData, position );

									vector ofixPlayerPos;
									ofixPlayerPos[0] = position[0];
									ofixPlayerPos[2] = position[2];

									ofixPlayerPos = SnapToGround( ofixPlayerPos );

									Admin.SetPosition(ofixPlayerPos);

									strMessage = "Teleported To Location: " + cData;
									Msgparam = new Param1<string>( strMessage );
									GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
								}
								else
								{
									strMessage = "Teleport Failed! Location: " + cData + " Is not on the list!";
									Msgparam = new Param1<string>( strMessage );
									GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
								}
							break;

							case "/spi":
								oSpawnItemFunc(false,Admin,cData);
								strMessage = "Admin Commands: Item " + cData + " Added in Inventory!";
								Msgparam = new Param1<string>( strMessage );
								GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
							break;

							case "/spg":
								oSpawnItemFunc(true,Admin,cData);
								strMessage = "Admin Commands: Item " + cData + " Spawned around you!";
								Msgparam = new Param1<string>( strMessage );
								GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
							break;

							case "/akmkit":
								oSpawnItemFunc(true,Admin,"AKM");
								oSpawnItemFunc(true,Admin,"AK_WoodBttstck");
								oSpawnItemFunc(true,Admin,"AK_WoodHndgrd");
								oSpawnItemFunc(true,Admin,"Mag_AKM_Drum75Rnd");
								oSpawnItemFunc(true,Admin,"Mag_AKM_Drum75Rnd");
								oSpawnItemFunc(true,Admin,"Mag_AKM_Drum75Rnd");
								oSpawnItemFunc(true,Admin,"KobraOptic");
								oSpawnItemFunc(true,Admin,"Battery9V");
								oSpawnItemFunc(true,Admin,"AssaultBag_Black");
								oSpawnItemFunc(true,Admin,"AK_Suppressor");
								oSpawnItemFunc(true,Admin,"KashtanOptic");
								oSpawnItemFunc(true,Admin,"Mag_AKM_Drum75Rnd");
								strMessage = "AKM Kit Spawned around you!";
								Msgparam = new Param1<string>( strMessage );
								GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
							break;

							case "/m4kit":
								oSpawnItemFunc(true,Admin,"M4A1");
								oSpawnItemFunc(true,Admin,"M4_MPBttstck");
								oSpawnItemFunc(true,Admin,"M4_RISHndgrd");
								oSpawnItemFunc(true,Admin,"Mag_STANAGCoupled_30Rnd");
								oSpawnItemFunc(true,Admin,"Mag_STANAGCoupled_30Rnd");
								oSpawnItemFunc(true,Admin,"Mag_STANAGCoupled_30Rnd");
								oSpawnItemFunc(true,Admin,"ACOGOptic");
								oSpawnItemFunc(true,Admin,"Battery9V");
								oSpawnItemFunc(true,Admin,"AssaultBag_Black");
								oSpawnItemFunc(true,Admin,"M4_Suppressor");
								oSpawnItemFunc(true,Admin,"M68Optic");
								oSpawnItemFunc(true,Admin,"Mag_STANAGCoupled_30Rnd");
								strMessage = "M4 Kit Spawned around you!";
								Msgparam = new Param1<string>( strMessage );
								GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
							break;	

							case "/foodkit":
								oSpawnItemFunc(true,Admin,"UKAssVest_Olive");
								oSpawnItemFunc(true,Admin,"PeachesCan_Opened");
								oSpawnItemFunc(true,Admin,"PeachesCan_Opened");
								oSpawnItemFunc(true,Admin,"PeachesCan_Opened");
								oSpawnItemFunc(true,Admin,"BakedBeansCan_Opened");
								oSpawnItemFunc(true,Admin,"BakedBeansCan_Opened");
								oSpawnItemFunc(true,Admin,"BakedBeansCan_Opened");
								oSpawnItemFunc(true,Admin,"WaterBottle");
								oSpawnItemFunc(true,Admin,"WaterBottle");
								oSpawnItemFunc(true,Admin,"WaterBottle");
								oSpawnItemFunc(true,Admin,"CowSteakMeat");
								oSpawnItemFunc(true,Admin,"CowSteakMeat");
								strMessage = "Food Kit Spawned around you!";
								Msgparam = new Param1<string>( strMessage );
								GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
							break;

							case "/hannibal":
								oSpawnItemFunc(true,Admin,"HumanSteakMeat");
								oSpawnItemFunc(true,Admin,"HumanSteakMeat");
								oSpawnItemFunc(true,Admin,"HumanSteakMeat");
								oSpawnItemFunc(true,Admin,"HumanSteakMeat");
								oSpawnItemFunc(true,Admin,"HumanSteakMeat");
								oSpawnItemFunc(true,Admin,"HumanSteakMeat");
								oSpawnItemFunc(true,Admin,"HumanSteakMeat");
								oSpawnItemFunc(true,Admin,"HumanSteakMeat");
								oSpawnItemFunc(true,Admin,"HumanSteakMeat");
								oSpawnItemFunc(true,Admin,"HumanSteakMeat");
								oSpawnItemFunc(true,Admin,"HumanSteakMeat");
								oSpawnItemFunc(true,Admin,"HumanSteakMeat");
								strMessage = "...where is the nice Ciante?";
								Msgparam = new Param1<string>( strMessage );
								GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
							break;									

							case "/tpc":
								vector tpPos = cData.ToVector();
								vector fixPlayerPos;
								fixPlayerPos[0] = tpPos[0];
								fixPlayerPos[2] = tpPos[2];

								fixPlayerPos = SnapToGround( fixPlayerPos );
								Admin.SetPosition(fixPlayerPos);

								strMessage = "Teleported to:: " + tpPos;
								Msgparam = new Param1<string>( strMessage );
								GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
							break;

							case "/export":
								if (m_MissionServer.GetModule(AdvancedLoadouts))
								{
									AdvancedLoadouts.Cast(m_MissionServer.GetModule(AdvancedLoadouts)).ExportInventory(Admin);
									Msgparam = new Param1<string>( "LOADOUT EXPORTED!" );
									GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
								}
								else
								{
									Msgparam = new Param1<string>( "LOADOUT EXPORT FAILED! MOD DISABLED!" );
									GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
								}
							break;

							case "/ammo":
							EntityAI CurrentWeapon = Admin.GetHumanInventory().GetEntityInHands();
							if( CurrentWeapon )
								{
									CurrentWeapon.SetHealth( CurrentWeapon.GetMaxHealth( "", "" ) );
									Magazine foundMag = ( Magazine ) CurrentWeapon.GetAttachmentByConfigTypeName( "DefaultMagazine" );
									if( foundMag && foundMag.IsMagazine())
									{
										foundMag.ServerSetAmmoMax();
									}
																		
									Object Suppressor = ( Object ) CurrentWeapon.GetAttachmentByConfigTypeName( "SuppressorBase" );
									if( Suppressor )
									{
										Suppressor.SetHealth( Suppressor.GetMaxHealth( "", "" ) );
									}
									string displayName = CurrentWeapon.ConfigGetString("displayName");

									Msgparam = new Param1<string>( "Weapon " + displayName + "Reloaded and Repaired" );
									GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
								}
							break;

							case "/updateLoadouts":
								if (m_MissionServer.GetModule(AdvancedLoadouts))
								{
									AdvancedLoadouts.Cast(m_MissionServer.GetModule(AdvancedLoadouts)).ConstructLoadouts(true);
									Msgparam = new Param1<string>( "LOADOUTS UPDATED!" );
									GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
								}
								else
								{
									Msgparam = new Param1<string>( "LOADOUT UPDATE FAILED! MOD DISABLED!" );
									GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
								}
							break;

							case "/freecam":
								 PlayerBase pBody = Admin;

								 if (m_FreeCamera)
									{
										GetGame().SelectPlayer(Admin.GetIdentity(), pBody);
										m_FreeCamera = false;
										Msgparam = new Param1<string>( "Exiting FreeCam!" );
										GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
									}
									else
									{
										GetGame().SelectPlayer(Admin.GetIdentity(), NULL);
										GetGame().SelectSpectator(Admin.GetIdentity(), "freedebugcamera", Admin.GetPosition());
										m_FreeCamera = true;
										Msgparam = new Param1<string>( "FreeCam Spawned!" );
										GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
									}

							break;

							case "/debug":
								  if (m_IsDebugRunning)
									 {
										Msgparam = new Param1<string>( "DeBug Monitor (Status Monitor) Disabled!" );
										GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
										GetGame().SetDebugMonitorEnabled(0);
										m_IsDebugRunning = false;
									 }
									 else
									 {
										Msgparam = new Param1<string>( "DeBug Monitor (Status Monitor) Enabled!" );
										GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
										GetGame().SetDebugMonitorEnabled(1);
										m_IsDebugRunning = true;
									 }

							break;

							case "/nighttime":
								GetGame().GetWorld().SetDate( 1988, 9, 23, 22, 0 );
								Msgparam = new Param1<string>( "NIGHT TIME!!" );
								GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
							break;

							case "/daytime":
								GetGame().GetWorld().SetDate( 1988, 5, 23, 12, 0 );
								Msgparam = new Param1<string>( "DAY TIME!!" );
								GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
							break;

							/*case "/godmode":
								string GmodeCheck;
								g_Game.GetProfileString("SafeZoneStatus"+GUID,GmodeCheck);
								if (GmodeCheck == "true")
								   {
										g_Game.SetProfileString("SafeZoneStatus"+GUID,"false");
										Msgparam = new Param1<string>( "God Mode DISABLED!" );
										GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
								   }
								   else
								   {
										g_Game.SetProfileString("SafeZoneStatus"+GUID,"true");
										Msgparam = new Param1<string>( "God Mode ENABLED!" );
										GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
								   }

							break;
							*/
							case "/heal":
								 Msgparam = new Param1<string>( "Player Healed!" );
								 GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
								 Admin.SetHealth( Admin.GetMaxHealth( "", "" ) );
								 Admin.SetHealth( "","Blood", Admin.GetMaxHealth( "", "Blood" ) );
							break;

							case "/kill":
								  Admin.SetHealth(0);
							break;
																
							case "/tpalltome":
								 int tpCount = TeleportAllPlayersTo(Admin);
								 string msgc = "All " + tpCount.ToString() + " Players Teleported to my POS!";
								 Msgparam = new Param1<string>( msgc );
								 GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
							break;
																
							case "/killall":
								for ( int ig = 0; ig < players.Count(); ++ig )
								{
									PlayerBase Target = players.Get(ig);
									if ( Target.GetIdentity() != AdminIdentity )
									{
										Target.SetHealth(0);						
									}
								}
							break;
							
							case "/spawncar":
							EntityAI MyV3S;
							vector NewPosition;
							vector OldPosition;
							OldPosition = Admin.GetPosition();
							NewPosition[0] = OldPosition[0] + 1.5;
							NewPosition[1] = OldPosition[1] + 0.2;
							NewPosition[2] = OldPosition[2] + 1.5;
							MyV3S = GetGame().CreateObject( "OffroadHatchback", NewPosition, false, true, true );		            
							MyV3S.GetInventory().CreateAttachment("HatchbackHood");
							MyV3S.GetInventory().CreateAttachment("HatchbackTrunk");
							MyV3S.GetInventory().CreateAttachment("HatchbackDoors_CoDriver");
							MyV3S.GetInventory().CreateAttachment("HatchbackWheel");
							MyV3S.GetInventory().CreateAttachment("HatchbackWheel");
							MyV3S.GetInventory().CreateAttachment("HatchbackWheel");
							MyV3S.GetInventory().CreateAttachment("HatchbackWheel");
							MyV3S.GetInventory().CreateAttachment("CarRadiator");
							MyV3S.GetInventory().CreateAttachment("SparkPlug");
							MyV3S.GetInventory().CreateAttachment("EngineBelt");
							MyV3S.GetInventory().CreateAttachment("CarBattery");
							auto carfluids = Car.Cast( MyV3S );
							carfluids.Fill( CarFluid.FUEL, 1000 );
							carfluids.Fill( CarFluid.OIL, 1000 );
							carfluids.Fill( CarFluid.BRAKE, 1000 );
							carfluids.Fill( CarFluid.COOLANT, 1000 );
							break;
							/*
							case "/basecar":
							EntityAI Myhatchback;
							//EntityAI itemEnt;
							//ItemBase itemBs;
							vector carlocNew;
							vector carlocOld;
							carlocOld = Admin.GetPosition();
							carlocNew[0] = carlocOld[0] + 1.5;
							carlocNew[1] = carlocOld[1] + 0.2;
							carlocNew[2] = carlocOld[2] + 1.5;
							Myhatchback = GetGame().CreateObject( "OffroadHatchback", NewPosition, false, true, true );		            
							Myhatchback.GetInventory().CreateAttachment("HatchbackHood");
							Myhatchback.GetInventory().CreateAttachment("HatchbackTrunk");
							Myhatchback.GetInventory().CreateAttachment("HatchbackDoors_CoDriver");
							Myhatchback.GetInventory().CreateAttachment("HatchbackWheel");
							Myhatchback.GetInventory().CreateAttachment("HatchbackWheel");
							Myhatchback.GetInventory().CreateAttachment("HatchbackWheel");
							Myhatchback.GetInventory().CreateAttachment("HatchbackWheel");
							Myhatchback.GetInventory().CreateAttachment("CarRadiator");
							Myhatchback.GetInventory().CreateAttachment("SparkPlug");
							Myhatchback.GetInventory().CreateAttachment("EngineBelt");
							Myhatchback.GetInventory().CreateAttachment("CarBattery");
							itemEnt = Myhatchback.GetInventory().CreateInInventory("Rag");
							itemBs = ItemBase.Cast(itemEnt);
							itemBs.SetQuantity(6);
							
							auto carfluids1 = Car.Cast( Myhatchback );
							carfluids1.Fill( CarFluid.FUEL, 1000 );
							carfluids1.Fill( CarFluid.OIL, 1000 );
							carfluids1.Fill( CarFluid.BRAKE, 1000 );
							carfluids1.Fill( CarFluid.COOLANT, 1000 );
							break;
							*/


							//break;

							default:
								Msgparam = new Param1<string>( "Error: The following command is Unknown." );
								GetGame().RPCSingleParam(Admin, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, AdminIdentity);
							break;
					}
				}
			}
		}
	}
}
