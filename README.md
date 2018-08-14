# protobuf-lua
pack/unpack protobuf from lua 5.3

test:

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
local tbl =msg:decode(str)
for k, v in pairs(tbl) do
  print(k, v)
end
