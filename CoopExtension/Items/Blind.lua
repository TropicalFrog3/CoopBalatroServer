SMODS.Blind({
	key = "bl_coop",
	dollars = 5,
	mult = 1, -- Jen's Almanac crashes the game if the mult is 0
	boss = { min = 1, max = 10 },
	boss_colour = HEX("ac3232"),
	atlas = "mp_player_blind_chip",
	discovered = true,
	in_pool = function(self)
		return false
	end,
	prefix_config = { key = false, atlas = false },
})

function is_coop_boss()
	if not G.GAME or not G.GAME.blind then
		return false
	end
	return G.GAME.blind.config.blind.key == "bl_pvp"
end