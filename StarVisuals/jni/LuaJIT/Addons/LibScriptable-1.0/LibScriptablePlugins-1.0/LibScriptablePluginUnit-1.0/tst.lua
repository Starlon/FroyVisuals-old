str="txt1 txt2 txt3";
sep = " "
fields = {str:match((str:gsub("[^"..sep.."]*"..sep, "([^"..sep.."]*)"..sep)))}

for k, v in pairs(fields) do
	print(k, v)
end
