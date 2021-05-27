function printalltext()
  printtext("db74: ", memory.readbyte(0xdb74), 0x31, 0)
  printtext("d9d4: ", memory.readbyte(0xd9d5), 0xD9, 1)
  printtext("d911: ", memory.readbyterange(0xd911, 3), {0x31, 0xEC, 0x0A}, 2)
  printtext("d0d1: ", memory.readbyterange(0xd0d1, 2), {0x88, 0x32}, 3)
  printtext("pwRNG: ", memory.readbyte(0xdac3), function(b) return b >= 0x8B and b <= 0xB6 end, 4)
  printtext("timer: ", memory.readbyte(0xd13a), 0x6E, 5)
  printtext("steps: ", memory.readbyte(0xd13b), 0xE9, 6)
  printtext("xy: ", memory.readbyterange(0xdab4, 4), {0x52, 0, 0x31, 0xF9}, 7)
  printtext("seed: ", memory.readbyterange(0xdab8, 4), {0xE8, 0xC3, 0x82, 0xB8}, 8)
end

function printtext(text, data, compare, y)
  local color = "green"
  if (type(data) == 'table') then
	local datatext = ""
	local i = 0
	for _ in pairs(data) do i = i + 1 end
	for j = 0, i - 1 do
		datatext = datatext .. string.format("%02X ", data[j])
		if data[j] ~= compare[j + 1] then
			color = "red"
		end
	end
	gui.text(0, y * 16, text .. datatext, color)
	return
  end
  
  if (type(compare) == 'number' and data ~= compare)
  or (type(compare) == 'function' and not compare(data)) then
    color = "red"
  end
  
  gui.text(0, y * 16, text .. string.format("%X", data), color)
end

while true do
  printalltext()
  emu.frameadvance()
end