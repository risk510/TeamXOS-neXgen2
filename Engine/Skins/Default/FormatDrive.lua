-- Variables
MainStrings = {}
MainStrings[1] = "Format C:"
MainStrings[2] = "Format E:"
MainStrings[3] = "Format F:"
MainStrings[4] = "Format G:"
MainStrings[5] = "Format X:"
MainStrings[6] = "Format Y:"
MainStrings[7] = "Format Z:"
MainSelection = 1
MainTotal = 7
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

	-- Warning Text
	FontPosition( "Menu", 461, 100 )
	FontColour( 255, 255, 0, 0 )
	FontDrawText( "Menu", "*** WARNING - WARNING ***", XBOX_FONT_CENTER )
	FontColour( 255, 255, 255, 0 )
	FontPosition( "Menu", 461, 120 )
	FontDrawText( "Menu", "This Will Remove All", XBOX_FONT_CENTER )
	FontPosition( "Menu", 461, 140 )
	FontDrawText( "Menu", "Data From Your Drive!!", XBOX_FONT_CENTER )


	-- MENU
	for itemp = 1, MainTotal do

		FontOffset = itemp * 26
		FontPosY = 195 + FontOffset
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

			-- Format C:
			if MainSelection == 1 then
				FormatDrive("C")
			end
			
			-- Format E:
			if MainSelection == 2 then
				FormatDrive("E")
			end
			
			-- Format F:
			if MainSelection == 3 then
				FormatDrive("F")
			end
			
			-- Format G:
			if MainSelection == 4 then
				FormatDrive("G")
			end
			
			-- Format X:
			if MainSelection == 5 then
				FormatDrive("X")
			end
			
			-- Format Y:
			if MainSelection == 6 then
				FormatDrive("Y")
			end
			
			-- Format Z:
			if MainSelection == 7 then
				FormatDrive("Z")
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
			SceneLoad( "SKINS:\\Default\\Settings.lua" )
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