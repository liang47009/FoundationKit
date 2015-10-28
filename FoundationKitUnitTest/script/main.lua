require("req.lua")

-- cclog
cclog = function(...)
    print(string.format(...))
end

-- for CCLuaEngine traceback
function __G__TRACKBACK__(msg)
    cclog("----------------------------------------")
    cclog("LUA ERROR: " .. tostring(msg) .. "\n")
    cclog(debug.traceback())
    cclog("----------------------------------------")
end

local function main()
    cclog("==================== run main")
    collectgarbage("collect")
    collectgarbage("setpause", 100)
    collectgarbage("setstepmul", 5000)
    
    printReqMsg("=========  printReqMsg")
    doRegisterEvent();
    executeEvent("testFunction0");
    executeEvent("testFunction1");
    doUnregisterEvent();
    
        executeEvent("testFunction0");
    executeEvent("testFunction1");
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    error(msg)
end