
function unrequire(m)
	package.loaded[m] = nil
	rawset(_G, m, nil)
end
