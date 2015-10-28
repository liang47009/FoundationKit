------------------------------------------------------------------
-- tableex.lua
-- Author     : libo
-- Version    : 1.0.0.0
-- Date       : 2012-08-03
-- Description: 
------------------------------------------------------------------

-- Extensions to the table module
module ("table", package.seeall)

--[[ EXTRA FUNCTIONS ]]--
--local fmt  = string.format
--[[ CONSTANTS ]]--
--local HOURPERDAY  = 24

--[[ LOCAL ARE FASTER ]]--
local next = next

-- Return whether table is empty.
-- @param t table
-- @return <code>true</code> if empty or <code>false</code> otherwise
function empty (t)
  return not next (t)
end

-- Make table is read-only
-- @param t target table, t must be a table
-- @return a read-only table
function readOnly(t)
	return setmetatable({},{
			__index = t,
			__newindex = function(t,key,value)
							error("Attempt to modify a read-only table",2)
						 end,
			__metatable = false
		});
end

-- Find the number of elements in a table.
-- @param t table
-- @return number of elements in t
function size(t)
  local n = 0
  for _ in pairs (t) do
    n = n + 1
  end
  return n
end

-- Clears all contens of a table.
-- All pairs of key and value stored in table 'source' will be removed by
-- setting nil to each key used to store values in table 'source'.
-- @param tab Table which must be cleared.
-- @usage return table.clear(results)
function clear(tab)
    if tab == nil then return {} end
	local elem = next(tab)
	while elem ~= nil do
		tab[elem] = nil
		elem = next(tab)
	end
	return tab
end

-- Sort key by the table
-- @param sortTable target Table
-- @param sortFun sort function
-- @return Iterator of the target table elements
-- @usage for elements in table.pairsByKeys(targetTable) do ...  end
function pairsByKeys(sortTable, sortFun)
	local keyTable = {}
	for key in pairs(sortTable) do
		insert(keyTable, key)
	end
	sort(keyTable, sortFun)
	local i = 0
	local iter = function()
		i = i +1
		if keyTable[i] == nil then return nil
		else return keyTable[i], sortTable[keyTable[i]]
		end
	end
	return iter
end

-- check a value indexof the table
-- @param tTable target Table
-- @param value be check value
-- @return if target table has the value return true ,also return false.
-- @usage index = table.indexOf(source, value)
function indexOf(tTable, value)
	if type(tTable) == "table" then
		for key, val in pairs(tTable) do
			if val == value then
				return key
			end
		end
	else
		error("the param type is not table")
	end
	return -1
end

-- clone a table
-- @param t Source Table
-- @param nometa Set meta table ?
-- @return Table containing copied elements.
-- @usage copied = table.clone(source)
-- @usage copied = table.clone(source, true)
function clone (t, nometa)
  local u = {}
  if not nometa then
    setmetatable (u, getmetatable (t))
  end
  for i, v in pairs (t) do
    u[i] = v
  end
  return u
end

-- Copies all elements stored in a table into another.
-- Each pair of key and value stored in table 'source' will be set into table
-- 'destiny'.
-- If no 'destiny' table is defined, a new empty table is used.
-- @param source Table containing elements to be copied.
-- @param destiny [optional] Table which elements must be copied into.
-- @return Table containing copied elements.
-- @usage copied = table.copy(results)
-- @usage table.copy(results, newcopy)
function copy(source, destiny)
	if source then
		if not destiny then destiny = {} end
		for field, value in pairs(source) do
			rawset(destiny, field, value)
		end
	end
	return destiny
end

--- make a deep copy of a table, recursively copying all the keys and fields.
-- This will also set the copied table's metatable to that of the original.
-- @param t A table
-- @return new table
function deepcopy(t)
    if type(t) ~= 'table' then return t end
    local mt = getmetatable(t)
    local res = {}
    for k,v in pairs(t) do
        if type(v) == 'table' then
            v = deepcopy(v)
        end
        res[k] = v
    end
    setmetatable(res,mt)
    return res
end

function copycount(source,pos)
    local res = {}
    local index = 0
    for field, value in pairs(source) do
        rawset(res, field, value)
        index = index + 1
        if index == pos then break end
    end
    return res
end

function asstring(tableName, tableData)
    return tableName .. " = " .. serialization(tableData)
end

function serialization(object)
    local szRetBuf = ""
    local szType = type(object)
    if szType == "number" then
        szRetBuf = szRetBuf .. tostring(object)
    elseif szType == "string" or szType == "boolean" then
        szRetBuf = szRetBuf .. string.format("%q", tostring(object))
    elseif szType == "table" then
        szRetBuf = szRetBuf .. "{\n"
        for i, v in pairs(object) do
            szRetBuf = szRetBuf .. "["
            szRetBuf = szRetBuf .. serialization(i)
            szRetBuf = szRetBuf .. "]= "
            szRetBuf = szRetBuf .. serialization(v)
            szRetBuf = szRetBuf .. ",\n"
        end
        szRetBuf = szRetBuf .. "}"
    else
    end
    return szRetBuf
end

function keys(t)
    local keys = {}
    for k, v in pairs(t) do
        keys[#keys + 1] = k
    end
    return keys
end

function values(t)
    local values = {}
    for k, v in pairs(t) do
        values[#values + 1] = v
    end
    return values
end