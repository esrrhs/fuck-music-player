-----------------------------------------
-- Start of handler functions
-----------------------------------------
function click_next(args)
    local winMgr = CEGUI.WindowManager:getSingleton()

   	local pos = GetNowListPos();
   	SetNowListPos(pos + 1);
end
function click_last(args)
    local winMgr = CEGUI.WindowManager:getSingleton()

   	local pos = GetNowListPos();
   	SetNowListPos(pos - 1);
end

function click_random(args)
    local winMgr = CEGUI.WindowManager:getSingleton()
    local logger = CEGUI.Logger:getSingleton()  

   	local random = CEGUI.toCheckbox(winMgr:getWindow("uiwheel/random_checkbox"));
   	
   	if random:isSelected() then
   	
        RandomShuffleMusic();
   	    SetNowListPos(0); 
        logger:logEvent( "click_random random ok" ) 
   	end
   	
end
function change_volume(args)
    local winMgr = CEGUI.WindowManager:getSingleton()
    local logger = CEGUI.Logger:getSingleton()  

   	local volumewin = CEGUI.toScrollbar(winMgr:getWindow("uiwheel/Controls/volume"));
   	local volume = volumewin:getScrollPosition();
   	
   	SetMusicVolume(volume);
end

function ini_uioading()

    local guiSystem = CEGUI.System:getSingleton()
    local schemeMgr = CEGUI.SchemeManager:getSingleton()
    local winMgr = CEGUI.WindowManager:getSingleton()
    local logger = CEGUI.Logger:getSingleton()  

    logger:logEvent( "ini_uioading begin..." )  
    local wheel = winMgr:loadWindowLayout("uiloading.layout")
    
    local root = guiSystem:getGUISheet()
    root:addChildWindow(wheel);
    
    winMgr:getWindow("uiloading"):hide()

    logger:logEvent( "ini_uiwheel ok" )  

end

function ini_uiwheel()

    local guiSystem = CEGUI.System:getSingleton()
    local schemeMgr = CEGUI.SchemeManager:getSingleton()
    local winMgr = CEGUI.WindowManager:getSingleton()
    local logger = CEGUI.Logger:getSingleton()  

    logger:logEvent( "ini_uiwheel begin..." )  
    local wheel = winMgr:loadWindowLayout("uiwheel.layout")
    
    local root = guiSystem:getGUISheet()
    root:addChildWindow(wheel);
    
    -- ¸ÄÎªlayout¿ØÖÆ
    --winMgr:getWindow("uiwheel/music_item_0"):setRotation(CEGUI.Vector3(0,0,30));
    --winMgr:getWindow("uiwheel/music_item_1"):setRotation(CEGUI.Vector3(0,0,15));
    --winMgr:getWindow("uiwheel/music_item_3"):setRotation(CEGUI.Vector3(0,0,-15));
    --winMgr:getWindow("uiwheel/music_item_4"):setRotation(CEGUI.Vector3(0,0,-30));

    winMgr:getWindow("uiwheel/next"):subscribeEvent("Clicked", "click_next")
    winMgr:getWindow("uiwheel/last"):subscribeEvent("Clicked", "click_last")
    winMgr:getWindow("uiwheel/random_checkbox"):subscribeEvent("CheckStateChanged", "click_random")
    winMgr:getWindow("uiwheel/Controls/volume"):subscribeEvent("ScrollPosChanged", "change_volume")

    winMgr:getWindow("uiwheel"):hide()

    logger:logEvent( "ini_uiwheel ok" )  
end

-----------------------------------------
-- Script Entry Point
-----------------------------------------
local guiSystem = CEGUI.System:getSingleton()
local schemeMgr = CEGUI.SchemeManager:getSingleton()
local winMgr = CEGUI.WindowManager:getSingleton()
local logger = CEGUI.Logger:getSingleton()  

logger:logEvent( "ini_cegui_system begin..." )  

schemeMgr:create("TaharezLook.scheme");

-- Designed wallpaper: CEGUI sample NASA GPN-2000-001437.tga (was deleted in 2011;
-- without it the D3D clear stays flat black behind transparent DefaultWindow).
CEGUI.ImagesetManager:getSingleton():createFromImageFile("BackgroundImage", "GPN-2000-001437.tga");

local root = winMgr:createWindow("TaharezLook/StaticImage", "Root")
root:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
root:setProperty("FrameEnabled", "False")
root:setProperty("BackgroundEnabled", "False")
root:setProperty("Image", "set:BackgroundImage image:full_image")

guiSystem:setGUISheet(root)
guiSystem:setDefaultMouseCursor("TaharezLook", "MouseArrow")
guiSystem:setDefaultTooltip("TaharezLook/Tooltip")

ini_uioading()
ini_uiwheel()

logger:logEvent( "ini_cegui_system ok" )  