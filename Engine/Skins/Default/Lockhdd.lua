-- Reset Value so we dont get (NULL) return
XBOX_LOCK_HDD_CHECK1 = ""
XBOX_LOCK_HDD_CHECK2 = ""
XBOX_LOCK_HDD_CHECK3 = ""
XBOX_LOCK_HDD_CHECK4 = ""
XBOX_LOCK_HDD_CHECK5 = ""
XBOX_LOCK_HDD_STATUS = ""

XBOX_LOCK_HDD()
-- Variables
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

	


	FontPosition( "Menu", 461, 213 )
	FontColour( FontAlpha, 255, 255, 255 )
	FontDrawText( "Menu", XBOX_LOCK_HDD_CHECK1 , XBOX_FONT_CENTER )
	FontPosition( "Menu", 461, 233 )
	FontDrawText( "Menu", XBOX_LOCK_HDD_CHECK2 , XBOX_FONT_CENTER )
	FontPosition( "Menu", 461, 253 )
	FontDrawText( "Menu", XBOX_LOCK_HDD_CHECK3 , XBOX_FONT_CENTER )
	FontPosition( "Menu", 461, 273 )
	FontDrawText( "Menu", XBOX_LOCK_HDD_CHECK4 , XBOX_FONT_CENTER )
	FontPosition( "Menu", 461, 293 )
	FontDrawText( "Menu", XBOX_LOCK_HDD_CHECK5 , XBOX_FONT_CENTER )
	FontPosition( "Menu", 461, 313 )
	FontDrawText( "Menu", XBOX_LOCK_HDD_STATUS , XBOX_FONT_CENTER )
	
end


-- Frame Updates
function FrameMove()

	-- A Button
	if XBOX_PAD_A_BUTTON or XBOX_IR_SELECT then
	end
		
	if XBOX_PAD_B_BUTTON or XBOX_IR_BACK then
		SceneLoad( "SKINS:\\Default\\Settings.lua" )
	end

end