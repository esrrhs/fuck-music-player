function UIScript_Heartbeat(elapsed)
    local guiSystem = CEGUI.System:getSingleton()
    local schemeMgr = CEGUI.SchemeManager:getSingleton()
    local winMgr = CEGUI.WindowManager:getSingleton()
    local logger = CEGUI.Logger:getSingleton()  
    local s = GetStatus();
    --正在查找
    if s == 2 then
        winMgr:getWindow("uiloading"):show()
        winMgr:getWindow("uiwheel"):hide()
    end 
    --查找结束
    if s == 3 then
        local time = GetCustomValue("find_end_stay_time");
        time = time + elapsed;
        SetCustomValue("find_end_stay_time", time);
        
        if time > 5 then
            OnFindEnd();
            winMgr:getWindow("uiloading"):hide()
            winMgr:getWindow("uiwheel"):show()
        
		        winMgr:getWindow("uiwheel/music_item_0"):setText(GetWheelName(0));
		        winMgr:getWindow("uiwheel/music_item_1"):setText(GetWheelName(1));
		        winMgr:getWindow("uiwheel/music_item_2"):setText(GetWheelName(2));
		        winMgr:getWindow("uiwheel/music_item_3"):setText(GetWheelName(3));
		        winMgr:getWindow("uiwheel/music_item_4"):setText(GetWheelName(4));
        
            SetStatus(4);
            
        else
        
            winMgr:getWindow("uiloading"):show()
            winMgr:getWindow("uiwheel"):hide()
            
        end
    end 
    --播放
    if s == 4 then
    
    end
end
