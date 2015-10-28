------------------------------------------------------------------
-- stringex.lua
-- Author     : libo
-- Version    : 1.0.0.0
-- Date       : 2012-08-03
-- Description: 
------------------------------------------------------------------

--- Additions to the string module
module ("string", package.seeall)
local tinsert = table.insert

function split(str, delimiter)
    if (delimiter=='') then return false end
    local pos,arr = 0, {}
    for st,sp in function() return find(str, delimiter, pos, true) end do
        tinsert(arr, sub(str, pos, st - 1))
        pos = sp + 1
    end
    tinsert(arr, sub(str, pos))
    return arr
end


--执行速度快但内存消耗大，不建议使用，这里只是展示string.gfind 的迭代用法
function splitex(strSrc, strSeparator)
	local tSplitArray = {}
	if type(strSrc) == "string" and type(strSeparator) == "string" then
		if string.sub(strSrc,-1) ~= strSeparator then
			strSrc = strSrc .. strSeparator
		end
		for value in gfind(strSrc,"(.-)%" .. strSeparator)do
			tinsert(tSplitArray,value)
		end
	end
	return tSplitArray
end


function SplitIterator(szFullString, szSeparator)
    local nFindStartIndex = 1
    local strRet = ""
    return function()
        local nFindLastIndex = find(szFullString, szSeparator, nFindStartIndex)
        if not nFindLastIndex then
            local nStrLen = len(szFullString)
            if nFindStartIndex >= nStrLen then
                strRet = nil
            else
                strRet =  sub(szFullString, nFindStartIndex, nStrLen)
            end
            nFindStartIndex = nStrLen
        else
            strRet = sub(szFullString, nFindStartIndex, nFindLastIndex - 1)
            nFindStartIndex = nFindLastIndex + len(szSeparator) 
        end
        return strRet
    end
end

function empty(src)
    if not src then return true end
    return not (len(src) > 0 )
end

function ltrim(str)
    return gsub(str, "^[ \t\n\r]+", "")
end

function rtrim(str)
    return gsub(str, "[ \t\n\r]+$", "")
end

function trim(str)
    str = gsub(str, "^[ \t\n\r]+", "")
    return gsub(str, "[ \t\n\r]+$", "")
end

function ucfirst(str)
    return upper(string.sub(str, 1, 1)) .. string.sub(str, 2)
end


function urlencode(str)
    -- convert line endings
    str = gsub(tostring(str), "\n", "\r\n")
    -- escape all characters but alphanumeric, '.' and '-'
    str = gsub(str, "([^%w%.%- ])", urlencodeChar)
    -- convert spaces to "+" symbols
    return gsub(str, " ", "+")
end

function utf8len(str)
    local len  = #str
    local left = len
    local cnt  = 0
    local arr  = {0, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc}
    while left ~= 0 do
        local tmp = byte(str, -left)
        local i   = #arr
        while arr[i] do
            if tmp >= arr[i] then
                left = left - i
                break
            end
            i = i - 1
        end
        cnt = cnt + 1
    end
    return cnt
end

function formatNumberThousands(num)
    local formatted = tostring(tonumber(num))
    local k
    while true do
        formatted, k = gsub(formatted, "^(-?%d+)(%d%d%d)", '%1,%2')
        if k == 0 then break end
    end
    return formatted
end

