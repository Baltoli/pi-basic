function inc(addr)
  [addr] <- [addr] + 1
  return [addr]
end

function incfive(addr)
  x <- inc(addr)
  x <- inc(addr)
  x <- inc(addr)
  x <- inc(addr)
  x <- inc(addr)
  return [addr]
end

[0] <- 10
x <- incfive(0)
