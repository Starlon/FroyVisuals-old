function smart_dump_fast(v)
   local dump = ""

   local mode = 0

   for i = 1, v:len() do
      local c = v:byte(i)

      if mode == 0 then
         if c < 32 then
            mode = 1
            dump = dump .. "<" .. string.format( "%02X", c )
         else
            dump = dump .. string.char(c)
         end
      else
         if c < 32 then
            dump = dump .. string.format( "%02X", c )
         else
            mode = 0
            dump = dump .. ">" .. string.char(c)
         end
      end
   end

   if mode == 1 then dump = dump .. ">" end

   return dump
end

