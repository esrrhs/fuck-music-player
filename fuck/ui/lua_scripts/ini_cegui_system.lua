-----------------------------------------
-- Start of handler functions
-----------------------------------------
-----------------------------------------
-- Script Entry Point
-----------------------------------------
local guiSystem = CEGUI.System:getSingleton()
local schemeMgr = CEGUI.SchemeManager:getSingleton()
local winMgr = CEGUI.WindowManager:getSingleton()

schemeMgr:create("TaharezLook.scheme");

local root = winMgr:createWindow("DefaultWindow", "Root")

local wheel = winMgr:loadWindowLayout("uiwheel.layout")
root:addChildWindow(wheel);

guiSystem:setGUISheet(root)
guiSystem:setDefaultMouseCursor("TaharezLook", "MouseArrow")
guiSystem:setDefaultTooltip("TaharezLook/Tooltip")

winMgr:getWindow("uiwheel/music_item_0"):setRotation(CEGUI.Vector3(0,0,30));
winMgr:getWindow("uiwheel/music_item_1"):setRotation(CEGUI.Vector3(0,0,15));
winMgr:getWindow("uiwheel/music_item_3"):setRotation(CEGUI.Vector3(0,0,-15));
winMgr:getWindow("uiwheel/music_item_4"):setRotation(CEGUI.Vector3(0,0,-30));