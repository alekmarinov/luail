require "luadevil"

filename = assert(arg[1], "image parameter missing")

function save(ext)
	local fileout = string.gsub(filename, "(%.[^%.]*)", "_"..ext..".tiff")
	os.remove(fileout)
	assert(il.SaveImage(fileout))
end

iid = 1
assert(il.LoadImage(filename))
il.BindImage(iid)
width, height = il.GetInteger("IMAGE_WIDTH"), il.GetInteger("IMAGE_HEIGHT") 
print("Loaded "..filename.." "..width.."x"..height)
ilu.Scale(width, height, 1)
save("converted")
