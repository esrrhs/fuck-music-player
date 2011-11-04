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
        
        --更新数目和列表
        winMgr:getWindow("uiloading/loading_dir"):setText(GetFindDir());
        winMgr:getWindow("uiloading/num"):setText(GetFindNum());
        winMgr:getWindow("uiloading/deatail"):setText(GetFindName());
    end 
    --查找结束
    if s == 3 then
        local time = GetCustomValue("find_end_stay_time");
        time = time + elapsed;
        SetCustomValue("find_end_stay_time", time);
        
        if time > 3 then 
            
            OnFindEnd();
            winMgr:getWindow("uiloading"):hide()
            winMgr:getWindow("uiwheel"):show()
        
		        SetNowListPos(0);
        
            SetStatus(4);
            
        else
        
            winMgr:getWindow("uiloading"):show()
            winMgr:getWindow("uiwheel"):hide()
            
            winMgr:getWindow("uiloading/loading_dir"):setText("-done-");
            winMgr:getWindow("uiloading/deatail"):setText("-done-");
            winMgr:getWindow("uiloading/num"):setText(GetFindNum());
            
        end
    end 
    --播放
    if s == 4 then
    
    end
end
function GetNowListPos()
    return GetCustomValue("now_list_pos");
end
function SetNowListPos(pos)
    
    local guiSystem = CEGUI.System:getSingleton()
    local schemeMgr = CEGUI.SchemeManager:getSingleton()
    local winMgr = CEGUI.WindowManager:getSingleton()
    local logger = CEGUI.Logger:getSingleton()  
    
    if pos < 0 then
        return
    end
    
    if pos < 0 then
        return
    end
    
    local wheelnum = GetWheelNum();
    if pos >= wheelnum then
        return
    end
    
    if (pos - 2) >= 0 then
        winMgr:getWindow("uiwheel/music_item_0"):setText(GetWheelName(pos - 2));
        winMgr:getWindow("uiwheel/music_item_0"):show();
    else
        winMgr:getWindow("uiwheel/music_item_0"):hide();
    end
    
    if (pos - 1) >= 0 then
        winMgr:getWindow("uiwheel/music_item_1"):setText(GetWheelName(pos - 1));
        winMgr:getWindow("uiwheel/music_item_1"):show();
    else
        winMgr:getWindow("uiwheel/music_item_1"):hide();
    end
     
    winMgr:getWindow("uiwheel/music_item_2"):setText(GetWheelName(pos));
    PlayMusic(pos);
    
    if (pos + 1) < wheelnum then
        winMgr:getWindow("uiwheel/music_item_3"):setText(GetWheelName(pos + 1));
        winMgr:getWindow("uiwheel/music_item_3"):show();
    else
        winMgr:getWindow("uiwheel/music_item_3"):hide();
    end
       
    if (pos + 2) < wheelnum then
        winMgr:getWindow("uiwheel/music_item_4"):setText(GetWheelName(pos + 2));
        winMgr:getWindow("uiwheel/music_item_4"):show();
    else
        winMgr:getWindow("uiwheel/music_item_4"):hide();
    end
    
    SetCustomValue("now_list_pos", pos);
end