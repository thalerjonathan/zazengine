frameCount = 0

function onStartup()
	print( "lua got called: onStartup" );
end

function postStartup()
	print( "lua got called: postStartup" )
end



function onShutdown()
	print( "lua got called: onShutdown" )
	print( frameCount )
end



function onSubSystemLoad()
	print( "lua got called: onSubSystemLoad" )
end

function postSubSystemLoad()
	print( "lua got called: postSubSystemLoad" )
end



function beginFrame( factor )
	frameCount = frameCount + 1
	-- print( "lua got called: beginFrame" )
end

function endFrame()
	-- print( "lua got called: endFrame" )
end
