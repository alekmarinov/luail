require "luadevil"

for i,v in pairs(_G) do print(i,v ) end

filename = assert(arg[1], "image parameter missing")

function save(ext)
	local fileout = string.gsub(filename, "(%.[^%.]*)", "_"..ext..".jpg")
	os.remove(fileout)
	assert(il.SaveImage(fileout))
end

iid = 1
assert(il.LoadImage(filename))
il.BindImage(iid)
width, height = il.GetInteger("IMAGE_WIDTH"), il.GetInteger("IMAGE_HEIGHT")

nid = assert(il.CloneCurImage())
assert(il.BindImage(nid))
assert(ilu.Wave(math.pi))
save("wave")
assert(il.DeleteImage(nid))

assert(il.BindImage(iid))
nid = assert(il.CloneCurImage())
assert(il.BindImage(nid))
ilu.ImageParameter("FILTER", "NEAREST")
assert(ilu.Scale(width/2, height/2, 0))
save("scale")
assert(il.DeleteImage(nid))

assert(il.BindImage(iid))
nid = assert(il.CloneCurImage())
assert(il.BindImage(nid))
assert(ilu.Pixelize(3))
assert(ilu.Convolution({1, 1, 1, 1, -8, 1, 1, 1, 1}, 3, 0))
assert(il.ClearColor(255, 255, 255, 255))
assert(ilu.ReplaceColor(141, 141, 141, 0.8))
save("buzz")
assert(il.DeleteImage(nid))

assert(il.DeleteImage(iid))
