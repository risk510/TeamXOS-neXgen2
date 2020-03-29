-- Variables
MainStrings = {}
MainStrings[1] = "Clear Cache"
MainStrings[2] = "Lock Hard Drive"
MainStrings[3] = "Un-Lock Hard Drive"
MainStrings[4] = "Format Partition"
MainStrings[5] = "Open DVD Tray"
MainStrings[6] = "Close DVD Tray"
MainSelection = 1
MainTotal = 6
FontAlpha = 255
StartFadeOut = 0


-- Frame Render
function Render()

	-- Draw Background
	DrawTexturedQuad( "Background", 30, 610, 20, 460 )

	-- C:\
	FontPosition( "Info", 60, 94 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "C: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_FREE_C )

	-- D:\
	FontPosition( "Info", 60, 112 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "D: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_DVD_DRIVE_STATUS )

	-- E:\
	FontPosition( "Info", 60, 130 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "E: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_FREE_E )

	-- F:\
	FontPosition( "Info", 60, 148 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "F: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_FREE_F )

	-- Time
	FontPosition( "Info", 60, 202 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "Time: " )
	OutputString = StringMerge( XBOX_TIME_HOUR, ":", XBOX_TIME_MINUTE, ":", XBOX_TIME_SECOND )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", OutputString )

	-- Date
	FontPosition( "Info", 60, 220 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "Date: " )
	OutputString = StringMerge( XBOX_DATE_DAY, "/", XBOX_DATE_MONTH, "/", XBOX_DATE_YEAR )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", OutputString )

	-- Mobo Temp
	FontPosition( "Info", 60, 238 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "Mobo: " )
	OutputString = StringMerge( XBOX_TEMP_SYS_C, "/", XBOX_TEMP_SYS_F )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", OutputString )

	-- CPU Temp
	FontPosition( "Info", 60, 256 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "CPU: " )
	OutputString = StringMerge( XBOX_TEMP_CPU_C, "/", XBOX_TEMP_CPU_F )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", OutputString )

	-- XBOX Version
	FontPosition( "Info", 60, 274 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "Version: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_VERSION )

	-- XBOX Encoder
	FontPosition( "Info", 60, 292 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "Encoder: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_ENCODER )

	-- Fan Speed
	FontPosition( "Info", 60, 310 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "Fan Speed: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_FAN_SPEED )
	
	-- IP Address
	FontPosition( "Info", 60, 328 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "IP: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_IP_ADDRESS )

	-- Track Playing
	FontPosition( "Info", 100, 388 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "Track: " )
	OutputString = StringMerge( XBOX_SND_TRACK_TITLE )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info",OutputString )

	-- SoundTrack Playing
	FontPosition( "Info", 100, 406 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "Album: " )
	OutputString = StringMerge( XBOX_SND_TRACK_ALBUM )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info",OutputString )

	-- Time Played / Total
	FontPosition( "Info", 100, 424 )
	FontColour( 255, 153, 153, 255 )
	FontDrawText( "Info", "Time: " )
	OutputString = StringMerge( XBOX_SND_TRACK_TIME_PLAYED, " / ", XBOX_SND_TRACK_TIME_TOTAL )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", OutputString )

	

	-- MENU
	for itemp = 1, MainTotal do

		FontOffset = itemp * 26
		FontPosY = 155 + FontOffset
		FontPosition( "Menu", 461, FontPosY )

		if itemp == MainSelection then
			FontColour( FontAlpha, 153, 153, 255 )
		else
			FontColour( FontAlpha, 255, 255, 255 )
		end

		FontDrawText( "Menu", MainStrings[itemp], XBOX_FONT_CENTER )

	end

end


-- Frame Updates
function FrameMove()

	-- Fade Out?
	if StartFadeOut > 0 then

		-- Faded Load Next Scene
		if FontAlpha == 0 then

			-- Clear Menu Cache
			if MainSelection == 1 then
				ClearCache()
SceneLoad( "SKINS:\\Nexglow\\clearcache.lua" )
			end
			
			-- Lock HDD
			if MainSelection == 2 then
				SceneLoad( "SKINS:\\Nexglow\\Lockhdd.lua" )
			end
			
			-- Un-Lock Hard Drive
			if MainSelection == 3 then
				SceneLoad( "SKINS:\\Nexglow\\Unlockhdd.lua" )
			end
			
			-- Format
			if MainSelection == 4 then
				SceneLoad( "SKINS:\\Nexglow\\FormatDrive.lua" )
			end
			
			-- Open DVD Tray
			if MainSelection == 5 then
				TrayOpen()
				SceneLoad( "SKINS:\\Nexglow\\settings.lua" )
			end
			
			-- Close DVD Tray
			if MainSelection == 6 then
				TrayClose()
				SceneLoad( "SKINS:\\Nexglow\\settings.lua" )
			end
		end

		FontAlpha = FontAlpha - 9
	end


	-- MakeSure Not Already Done!
	if StartFadeOut == 0 then

		-- A Button
		if XBOX_PAD_A_BUTTON or XBOX_IR_SELECT then
			StartFadeOut = 1
		end
		
		-- B Button
		if XBOX_PAD_B_BUTTON or XBOX_IR_BACK then
			SceneLoad( "SKINS:\\Nexglow\\Main.lua" )
		end

		-- UP
		if XBOX_PAD_UP or XBOX_IR_UP then
			MainSelection = MainSelection - 1
		end

		-- DOWN
		if XBOX_PAD_DOWN or XBOX_IR_DOWN then
			MainSelection = MainSelection + 1
		end

		-- CHECK BOUNDS
		if MainSelection < 1 then
			MainSelection = 1
		end
		if MainSelection > MainTotal then
			MainSelection = MainTotal
		end
	end

	--
	if FontAlpha < 0 then
		FontAlpha = 0
	end

end