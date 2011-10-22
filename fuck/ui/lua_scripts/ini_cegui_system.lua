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

local wheel = winMgr:loadWindowLayout("uiwheel.layout")
root:addChildWindow(wheel);

guiSystem:setGUISheet(root)
guiSystem:setDefaultMouseCursor("TaharezLook", "MouseArrow")
guiSystem:setDefaultTooltip("TaharezLook/Tooltip")

-- ¸ÄÎªlayout¿ØÖÆ
--winMgr:getWindow("uiwheel/music_item_0"):setRotation(CEGUI.Vector3(0,0,30));
--winMgr:getWindow("uiwheel/music_item_1"):setRotation(CEGUI.Vector3(0,0,15));
--winMgr:getWindow("uiwheel/music_item_3"):setRotation(CEGUI.Vector3(0,0,-15));
--winMgr:getWindow("uiwheel/music_item_4"):setRotation(CEGUI.Vector3(0,0,-30));

winMgr:getWindow("uiwheel/music_item_0"):setText(GetWheelName(0));
winMgr:getWindow("uiwheel/music_item_1"):setText(GetWheelName(1));
winMgr:getWindow("uiwheel/music_item_2"):setText(GetWheelName(2));
winMgr:getWindow("uiwheel/music_item_3"):setText(GetWheelName(3));
winMgr:getWindow("uiwheel/music_item_4"):setText(GetWheelName(4));


winMgr:getWindow("uiwheel/next"):subscribeEvent("Clicked", "click_next")
winMgr:getWindow("uiwheel/last"):subscribeEvent("Clicked", "click_last")

logger:logEvent( "ini_cegui_system ok" )  