function onStartup()
	print( "lua got called: onStartup" );
end

function postStartup()
	print( "lua got called: postStartup" )
end



function onShutdown()
	print( "lua got called: onShutdown" )
end



function onSubSystemLoad()
	print( "lua got called: onSubSystemLoad" )
end

function postSubSystemLoad()
	print( "lua got called: postSubSystemLoad" )
end



function beginFrame( factor )
	print( "lua got called: beginFrame" )
end

function endFrame()
	print( "lua got called: endFrame" )
end
