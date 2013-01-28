-- Metadata

IconAssets = group{quality=9.95}
Icon = image{"icon.png"}

function frames(fmt, count1, count2)
    t = {}
    for i = 0, count1 do
	for j = 0, count2 do
            t[1+#t] = string.format(fmt, i, j)
	end
    end
    return t
end

-- Animation: "giga"

gigaGroup = group{quality=6.3}
giga = image{ frames("imagesW/w2 [www.imagesplitter.net]-%d-%d.png", 4, 4) }
-- bg = image{"BG.png", quality=10}
