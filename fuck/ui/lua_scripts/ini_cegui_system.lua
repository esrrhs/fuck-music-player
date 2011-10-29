-----------------------------------------
-- Start of handler functions
-----------------------------------------
function click_next(args)
    local winMgr = CEGUI.WindowManager:getSingleton()

   	winMgr:getWindow("uiwheel/music_item_0"):setText(GetWheelName(1));
end
function click_last(args)
    local winMgr = CEGUI.WindowManager:getSingleton()

   	winMgr:getWindow("uiwheel/music_item_0"):setText(GetWheelName(2));
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

local root = winMgr:createWindow("DefaultWindow", "Root")

guiSystem:setGUISheet(root)
guiSystem:setDefaultMouseCursor("TaharezLook", "MouseArrow")
guiSystem:setDefaultTooltip("TaharezLook/Tooltip")

ini_uioading()
ini_uiwheel()

logger:logEvent( "ini_cegui_system ok" )  