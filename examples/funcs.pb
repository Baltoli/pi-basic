function inc(address)
  [address] <- [address] + 1
end

function square(address)
  [address] <- [address] * [address]
end

x <- 0
[x] <- 3
inc(x)
