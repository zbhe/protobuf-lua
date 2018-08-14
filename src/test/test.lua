lpb = require "lpb"
lpb.add_proto_path("./src/test/")
lpb.import_proto("test.proto")
local msg = lpb.get_message("test.Person")
local str = msg:encode({
	name="Bjarne Stroustrup",
	id = 1,
	friends = {"Scott Meyers", "Stanley Lippman"},
	email="bj@google.com"
})
function PrintTbl(T, Indent)
	Indent = Indent or 0
	local IndentStr = ""
	for i = 1, Indent do
		IndentStr = IndentStr .. "\t"
	end
	local Str = "{\n"
	for k, v in pairs(T) do
		if type(v) == type({}) then
			Str = Str .. string.format("%s\t%s = %s,\n", IndentStr, k, PrintTbl(v, Indent + 1) )
		else
			Str = Str .. string.format("%s\t%s = %s,\n", IndentStr, k, v)
		end
	end
	return string.format("%s%s}", Str, IndentStr)
end

local tbl =msg:decode(str)
print(PrintTbl(tbl))
