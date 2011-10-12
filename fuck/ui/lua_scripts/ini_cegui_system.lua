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
