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
	DrawTexturedQuad( "Background", 0, 640, 0, 480 )

	-- C:\
	FontPosition( "Info", 134, 58 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "C: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_FREE_C )

	-- D:\
	FontPosition( "Info", 134, 76 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "D: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_DVD_DRIVE_STATUS )

	-- E:\
	FontPosition( "Info", 134, 94 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "E: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_FREE_E )

	-- F:\
	FontPosition( "Info", 134, 112 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "F: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_FREE_F )

	-- Time
	FontPosition( "Info", 134, 163 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "Time: " )
	OutputString = StringMerge( XBOX_TIME_HOUR, ":", XBOX_TIME_MINUTE, ":", XBOX_TIME_SECOND )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", OutputString )

	-- Date
	FontPosition( "Info", 134, 181 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "Date: " )
	OutputString = StringMerge( XBOX_DATE_DAY, "/", XBOX_DATE_MONTH, "/", XBOX_DATE_YEAR )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", OutputString )

	-- Mobo Temp
	FontPosition( "Info", 134, 199 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "Mobo: " )
	OutputString = StringMerge( XBOX_TEMP_SYS_C, "/", XBOX_TEMP_SYS_F )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", OutputString )

	-- CPU Temp
	FontPosition( "Info", 134, 217 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "CPU: " )
	OutputString = StringMerge( XBOX_TEMP_CPU_C, "/", XBOX_TEMP_CPU_F )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", OutputString )

	-- XBOX Version
	FontPosition( "Info", 134, 235 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "Version: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_VERSION )

	-- XBOX Encoder
	FontPosition( "Info", 134, 253 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "Encoder: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_ENCODER )

	-- Fan Speed
	FontPosition( "Info", 134, 271 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "Fan Speed: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_FAN_SPEED )
	
	-- IP Address
	FontPosition( "Info", 134, 289 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "IP: " )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_IP_ADDRESS )

	-- Track Playing
	FontPosition( "Info", 134, 337 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "Track: " )
	FontPosition( "Info", 134, 354 )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_SND_TRACK_TITLE )

	-- SoundTrack Playing
	FontPosition( "Info", 134, 371 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "Album: " )
	FontPosition( "Info", 134, 388 )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", XBOX_SND_TRACK_ALBUM )

	-- Time Played / Total
	FontPosition( "Info", 134, 405 )
	FontColour( 255, 80, 255, 80 )
	FontDrawText( "Info", "Time: " )
	OutputString = StringMerge( XBOX_SND_TRACK_TIME_PLAYED, " / ", XBOX_SND_TRACK_TIME_TOTAL )
	FontColour( 255, 255, 255, 255 )
	FontDrawText( "Info", OutputString )

	

	-- MENU
	for itemp = 1, MainTotal do

		FontOffset = itemp * 26
		FontPosY = 095 + FontOffset
		FontPosition( "Menu", 461, FontPosY )

		if itemp == MainSelection then
			FontColour( FontAlpha, 80, 255, 80 )
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
				SceneLoad( "SKINS:\\Default\\clearcache.lua" )
			end
			
			-- Lock HDD
			if MainSelection == 2 then
				SceneLoad( "SKINS:\\Default\\Lockhdd.lua" )
			end
			
			-- Un-Lock Hard Drive
			if MainSelection == 3 then
				SceneLoad( "SKINS:\\Default\\Unlockhdd.lua" )
			end
			
			-- Format
			if MainSelection == 4 then
				SceneLoad( "SKINS:\\Default\\FormatDrive.lua" )
			end
			
			-- Open DVD Tray
			if MainSelection == 5 then
				TrayOpen()
				SceneLoad( "SKINS:\\Default\\settings.lua" )
			end
			
			-- Close DVD Tray
			if MainSelection == 6 then
				TrayClose()
				SceneLoad( "SKINS:\\Default\\settings.lua" )
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
			SceneLoad( "SKINS:\\Default\\Main.lua" )
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