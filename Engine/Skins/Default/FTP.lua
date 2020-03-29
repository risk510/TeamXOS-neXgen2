require ("includes.lua")
-- Textures
LoadTextures()

-- Fonts
LoadFonts()

-- Variables
TIA = 0
TIB = 0
TIC = 0
TM = 1
MIA = GetMenuItems(5)
MIC = GetMenuItems(6)
MTA = 0
MTC = 0

ftplls("*")
MIA=GetMenuItems(5)

MainStrings={}
MainStrings[1]="Connect"
MainStrings[2]="<<== Download"
MainStrings[3]="Upload ==>>"
MainStrings[4]="Refresh"
MainStrings[5]="Close"
MIB=5

-- Frame Render
function Render()

	-- Draw Background
	DrawTexturedQuad( "Background_FTP", 0, 640, 0, 480 )

	--FontPosition( "Info", 134, 58 )
	--FontColour( 255, 80, 255, 80 )
	--FontDrawText( "Info", "C: " )
	--FontColour( 255, 255, 255, 255 )
	--FontDrawText( "Info", XBOX_FREE_C )

	-- Local Items
	if MIA>0 then
		for instance=0, 12 do
			if instance+MTA<MIA then
				if instance+MTA~=TIA or TM~=0 then
					FontColour( 128, 255, 255, 255 )
				elseif instance+MTA==TIA and TM==0 then
					FontColour( 255, 255, 255, 255 )
				end
				FontPosition( "Info", 88, 128+instance*16 )
				mystr=GetMenuItemName(5, instance+MTA)
				FontDrawText( "Info", mystr )
			end
		end
	end

	-- Menu Nav Controls
	for i=1,MIB do
		if TIB==i-1 and TM==1 then
			FontPosition( "Info", 280, 120+40*i )
			FontColour( 255, 255, 255, 255 )
			FontDrawText( "Info", MainStrings[i] )
		else
			FontPosition( "Info", 280, 120+40*i )
			FontColour( 128, 255, 255, 255 )
			FontDrawText( "Info", MainStrings[i] )
		end
	end

	-- Remote Items
	if MIC>0 then
		for instance=0, 12 do
			if instance+MTC<MIC then
				if instance+MTC~=TIC or TM~=2 then
					FontColour( 128, 255, 255, 255 )
				elseif instance+MTC==TIC and TM==2 then
					FontColour( 255, 255, 255, 255 )
				end
				FontPosition( "Info", 400, 128+instance*16 )
				mystr=GetMenuItemName(6, instance+MTC)
				FontDrawText( "Info", mystr )
			end
		end
	end

end


framecnt=30
-- Frame Updates
function FrameMove()

-- B Button
if XBOX_PAD_B_BUTTON or XBOX_IR_BACK then
	SceneLoad( "SKINS:\\Default\\Main.lua" )
end

	-- Pad Up
	if XBOX_PAD_UP then
		if TM==0 and MIA>0 then
			TIA=TIA-1
		elseif TM==1 then
			TIB=TIB-1
		elseif TM==2 and MIC>0 then
			TIC=TIC-1
		end
		framecnt=10
	end

	-- Pad Down
	if XBOX_PAD_DOWN then
		if TM==0 and MIA>0 then
			TIA=TIA+1
		elseif TM==1 then
			TIB=TIB+1
		elseif TM==2 and MIC>0 then
			TIC=TIC+1
		end
		framecnt=10
	end

	-- Pad Left
	if XBOX_PAD_LEFT then
		TM=TM-1
		framecnt=10
	end

	-- Pad Right
	if XBOX_PAD_RIGHT then
		TM=TM+1
		framecnt=10
	end

	-- Left Trigger
	if XBOX_PAD_LEFT_TRIGGER then
		if TM==0 and MIA>0 then
			-- TIA=TIA-round(contApushl/32,0)
		elseif TM==1 then
			TIB=TIB-1
		elseif TM==2 and MIC>0 then
			-- TIC=TIC-round(contApushl/32,0)
		end
		framecnt=10
	end

	-- Right Trigger
	if XBOX_PAD_RIGHT_TRIGGER then
		if TM==0 and MIA>0 then
			-- TIA=TIA+round(contApushr/32,0)
		elseif TM==1 then
			TIB=TIB+1
		elseif TM==2 and MIC>0 then
			-- TIC=TIC+round(contApushr/32,0)
		end
		framecnt=10
	end

	if TM<0 then
		TM=0
	end

	if TM>2 then
		TM=2
	end

	if (MIA>0) then
		if (TIA<0) then
			TIA=0
		elseif (TIA>=MIA) then
			TIA=MIA-1
		end
		if (TIA>MTA+12) then
			MTA=TIA-12
		elseif (TIA<MTA) then
			MTA=TIA
		end
	end

	if (TIB<0) then
		TIB=0
	elseif (TIB>=MIB) then
		TIB=MIB-1
	end

	if(MIC>0) then
		if (TIC<0) then
			TIC=0
		elseif (TIC>=MIC) then
			TIC=MIC-1
		end
		if (TIC>MTC+12) then
			MTC=TIC-12
		elseif (TIC<MTC) then
			MTC=TIC
		end
	end

	MIA=GetMenuItems(5)
	MIC=GetMenuItems(6)

	if XBOX_PAD_A_BUTTON then
		if TM==0 then
			if GetItemType(5, TIA)==1 then
				ftplcd(GetItemFilename(5, TIA))
				TIA=0
				MTA=0
			end
		elseif TM==1 then
			if TIB==0 then
				ftpopen()
				TIC=0
				MTC=0
			elseif TIB==1 then
				ftpget(GetItemFilename(6, TIC))
			elseif TIB==2 then
				ftpput(GetItemFilename(5, TIA))
			elseif TIB==3 then
				ftplls("*")
				ftplist("dirFiltered")
				MIA=GetMenuItems(5)
				MIC=GetMenuItems(6)
				TIC=0
				MTC=0
				TIA=0
				MTA=0
			elseif TIB==4 then
				ftpclose()
				MIC=0
				MTC=0
				TIC=0
			end
		elseif TM==2 then
			if GetItemType(6, TIC)==1 then
				ftpcd(GetItemFilename(6, TIC))
				TIC=0
				MTC=0
			end
		end
		framecnt=10
	end


end