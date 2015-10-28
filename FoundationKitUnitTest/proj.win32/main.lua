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
collectgarbage("collect")
collectgarbage("setpause", 100)
collectgarbage("setstepmul", 5000)
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    error(msg)
end