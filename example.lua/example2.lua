require "luadevil"
require "luagl"

filename = assert(arg[1], "image parameter missing")

function Keyboard(key)
	if key == 27 then
		os.exit()
	end
end

function DrawFrame()
	gl.Enable( "TEXTURE_2D" )
	gl.BindTexture( "TEXTURE_2D", texid )
	gl.Begin( "QUADS" )
	gl.TexCoord(0, 0) gl.Vertex(-1.0, 1.0)
	gl.TexCoord(1, 0) gl.Vertex(1.0, 1.0)
	gl.TexCoord(1, 1) gl.Vertex(1.0, -1.0)
	gl.TexCoord(0, 1) gl.Vertex(-1.0, -1.0)
	gl.End()
	gl.Disable( "TEXTURE_2D" )

	glut.SwapBuffers()
	gl.Flush()
end

function Reshape(width, height)
	gl.Viewport(0, 0, width, height)
	gl.MatrixMode('MODELVIEW')
	gl.LoadIdentity()
	gl.Ortho(-1, 1, 1, -1, -1.0, 1.0)
	DrawFrame()
end

glut.Init()
glut.InitDisplayMode("RGB, DOUBLE")
glut.InitWindowSize(800, 600)
glut.CreateWindow("GL/Devil")
glut.DisplayFunc('DrawFrame')
glut.ReshapeFunc('Reshape')
glut.KeyboardFunc('Keyboard')

assert(il.LoadImage(filename))
--ilu.Mirror()
--ilu.Saturate1(0.9)
--ilu.Wave(math.pi)
texid = ilut.GLBindTexImage()
width, height = il.GetInteger("IMAGE_WIDTH"), il.GetInteger("IMAGE_HEIGHT")

glut.MainLoop()
