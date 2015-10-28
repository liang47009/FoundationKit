
function testFunction0()
    print(" ================ testFunction0")
end

function testFunction1()
    print(" ================ testFunction1")
end


function printReqMsg(...)
    print(string.format(...))
end

function doRegisterEvent()
    registerEvent("testFunction0", testFunction0);
    registerEvent("testFunction1", testFunction1);
end

function doUnregisterEvent()
    unregisterEvent("testFunction0", testFunction0);
    unregisterEvent("testFunction1", testFunction1);
end