-- Variables
AppNumber = GetMenuItems( EMU_LIST )
MainSelection = 0
ItemOffset = 0
FontAlpha = 255

-- TitleImages
for i=0, AppNumber-1 do
	TextureLoad("EMUSTXR" .. i, GetItemIconPath(EMU_LIST,i))
end


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
	for itemp = 0, AppNumber-1 do

		FontOffset1 = itemp + 1
		FontOffset2 = FontOffset1 * 26
		FontPosY = 195 + FontOffset2 + ItemOffset
		FontPosition( "Menu", 450, FontPosY )
		if (FontPosY < 205) or (FontPosY > 350) then
			FontAlpha = 0
		else
			FontAlpha = 255
		end
		if itemp == MainSelection then
			FontColour( FontAlpha, 80, 255, 80 )
			DrawTexturedQuad( "EMUSTXR" .. itemp, 310, 405, 86, 172)
		else
			FontColour( FontAlpha, 255, 255, 255 )
		end

		if itemp == MainSelection then
			FontColour( FontAlpha, 153, 153, 255 )
		else
			FontColour( FontAlpha, 255, 255, 255 )
		end

		CurrentApp = GetMenuItemName( EMU_LIST, itemp )
		FontDrawText( "Menu", CurrentApp, XBOX_FONT_CENTER )

	end

end


-- Frame Updates
function FrameMove()

		-- UP
		if XBOX_PAD_UP then
			if (MainSelection >= 0) and (AppNumber-1>=MainSelection) then
				MainSelection = MainSelection - 1
				if MainSelection > 4 then
					ItemOffset = ItemOffset + 26
				else
					ItemOffset = 0
				end
			end
		end

		-- DOWN
		if XBOX_PAD_DOWN then
			if (MainSelection >= 0) and (AppNumber-1>=MainSelection) then
				MainSelection = MainSelection + 1
				if MainSelection > 4 then
					ItemOffset = ItemOffset - 26
				else
					ItemOffset = 0
				end
			end
		end

		if XBOX_PAD_A_BUTTON then
			LaunchXBE(GetItemFilename(EMU_LIST, MainSelection))
		end
		
		if XBOX_PAD_B_BUTTON then
			SceneLoad( "SKINS:\\Nexglow\\Main.lua" )
		end

		-- CHECK BOUNDS
		if MainSelection < 0 then
			MainSelection = 0
		end
		if MainSelection == AppNumber then
			MainSelection = AppNumber - 1
			ItemOffset = ItemOffset + 26
		end
end