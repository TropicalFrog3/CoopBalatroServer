local MouseSentTime = 0
local MouseSentInterval = 0.1
local P1Mouse = {x = -100, y = -100}
local P2Mouse = {x = -100, y = -100}
local P2EaseMouse = {x = -100, y = -100} -- Store in screen coordinates
local easingSpeed = 8
local outCoop = true
local playerCount = nil

local resoX, resoY = 0, 0

-- Define a variable to hold the UI panel
local P2Ui = nil
local P1Ui = nil

local ShowP2 = true

-- local stop = false

function G.MULTIPLAYER.COOPEXT.set_P2Mouse(normalizedX, normalizedY)
    P2Mouse.x, P2Mouse.y = normalizedX, normalizedY
end

function G.MULTIPLAYER.COOPEXT.set_resolution(X, Y)
    if X ~= resoX and Y ~= resoY then
    sendWarnMessage(resoX.."x"..resoY, "MULTIPLAYER_COOPEXT")
    G.MULTIPLAYER.COOPEXT.overlay_message("One of your teammates has changed their resolution to ".. X.."x"..Y..".\n" ..
        "You may experience graphical issues with their mouse.\n" ..
        "Do you want to remove mouse support for this player?")
    end

    
end

local function createMouseP2()
    local panel = {
        width = 15,
        height = 15,
        visible = true
    }

    -- Define the red square
    panel.square = {
        color = {0, 1, 0},
        width = 15,
        height = 15
    }

    return panel
end

local function createMouseP1()
    local panel = {
        width = 15,
        height = 15,
        visible = true
    }

    -- Define the red square
    panel.square = {
        color = {0, 0, 1},
        width = 15,
        height = 15
    }

    return panel
end


local update_ref = Controller.update
---@diagnostic disable-next-line: duplicate-set-field
function Controller.update(self, dt)
    update_ref(self, dt)
    if G.LOBBY.code and G.LOBBY.config.gamemode == "coop" and G.LOBBY.players then
        local baseX, baseY = love.graphics.getDimensions()

        if playerCount ~= G.LOBBY.players then
            playerCount = G.LOBBY.players
            love.thread.getChannel("uiToNetwork"):push("action:resolution,X:"..baseX..",Y:"..baseY .. "\n")
        end

        if (resoX ~= baseX or resoY ~= baseY) then
            resoX, resoY = baseX, baseY
            love.thread.getChannel("uiToNetwork"):push("action:resolution,X:"..resoX..",Y:"..resoY .. "\n")
        end

        -- disable mouse visible
        if not outCoop then
            outCoop = not outCoop
            -- love.mouse.setVisible( false )
        end

        P1Mouse.x, P1Mouse.y = love.mouse.getPosition()

        if MouseSentTime > MouseSentInterval then
            love.thread.getChannel("uiToNetwork"):push("action:mouseMove,X:"..P1Mouse.x..",Y:"..P1Mouse.y .. "\n")
            MouseSentTime = 0
        else
            MouseSentTime = MouseSentTime + dt
        end
        -- Easing logic in base resolution coordinates
        P2EaseMouse.x = P2EaseMouse.x + (P2Mouse.x - P2EaseMouse.x) * easingSpeed * dt
        P2EaseMouse.y = P2EaseMouse.y + (P2Mouse.y - P2EaseMouse.y) * easingSpeed * dt

    elseif outCoop then
        outCoop = not outCoop
        love.mouse.setVisible( true )
        P1Mouse = {x = -100, y = -100}
        P2Mouse = {x = -100, y = -100}
        P2EaseMouse = {x = -100, y = -100}
    end
end

local key_press_ref = Controller.key_press_update
---@diagnostic disable-next-line: duplicate-set-field
function Controller.key_press_update(self, key, dt)
    key_press_ref(self, key, dt)

    if key == "escape" then
        if G.STATE == G.STATES.SPLASH then
            Client.send("action:player_mainMenu")
        else
            if not G.OVERLAY_MENU then 
                Client.send("action:player_option")
            elseif not G.OVERLAY_MENU.config.no_esc then
                Client.send("action:player_exitMenu")
            end
        end
    end
end

local function drawMouseUI(panel, normalizedX, normalizedY)
    if not panel.visible then return end
    local x, y = normalizedX - panel.width / 2, normalizedY - panel.height / 2
    love.graphics.setColor(panel.square.color)
    love.graphics.rectangle("fill", x, y, panel.width, panel.height)
end

local draw_ref = love.draw
---@diagnostic disable-next-line: duplicate-set-field
function love.draw()
    draw_ref()
    if G.LOBBY.code and G.LOBBY.config.gamemode == "coop" and G.LOBBY.players then

        

        if not P2Ui then P2Ui = createMouseP2() end
        if not P1Ui then P1Ui = createMouseP1() end

        -- Draw P1's Mouse UI
        -- if P1Ui then drawMouseUI(P1Ui, P1Mouse.x, P1Mouse.y) end

        -- Draw P2's Mouse UI only if UserChooseMouse is false
        if ShowP2 and P2Ui then 
            drawMouseUI(P2Ui, P2EaseMouse.x, P2EaseMouse.y)
        end
    end
end

function G.FUNCS.apply_changes()
    ShowP2 = false
    G.FUNCS.exit_overlay_menu()
end

local function create_UIBox_generic_options_with_2_buttons(args)
    args = args or {}
    local back_func = args.back_func or "exit_overlay_menu"
    local apply_func = args.apply_func or "apply_changes"
    local contents = args.contents or ({
        n=G.UIT.T, 
        config={text = "EMPTY", colour = G.C.UI.RED, scale = 0.4}
    })
  
    return {
      n = G.UIT.ROOT, 
      config = {align = "cm", minw = G.ROOM.T.w * 5, minh = G.ROOM.T.h * 5, padding = 0.1, r = 0.1, colour = args.bg_colour or {G.C.GREY[1], G.C.GREY[2], G.C.GREY[3], 0.7}}, 
      nodes = {
        {n = G.UIT.R, config = {align = "cm", minh = 1, r = 0.3, padding = 0.07, minw = 1, colour = args.outline_colour or G.C.JOKER_GREY, emboss = 0.1}, nodes = {
          {n = G.UIT.C, config = {align = "cm", minh = 1, r = 0.2, padding = 0.2, minw = 1, colour = args.colour or G.C.L_BLACK}, nodes = {
            {n = G.UIT.R, config = {align = "cm", padding = args.padding or 0.2, minw = args.minw or 7}, nodes = contents},
            
            -- Buttons Row (Apply & Cancel)
            {
              n = G.UIT.R,
              config = {align = "cm", minw = 5, padding = 0.1},
              nodes = {
                -- Apply Button
                {n = G.UIT.R, config = {align = "cm", minw = 2.5, r = 0.1, hover = true, colour = G.C.GREEN, button = apply_func, shadow = true}, nodes = {
                  {n = G.UIT.T, config = {text = "Yes", scale = 0.5, colour = G.C.UI.TEXT_LIGHT, shadow = true}}
                }},
                
                -- Cancel Button (Back)
                {n = G.UIT.R, config = {align = "cm", minw = 2.5, r = 0.1, hover = true, colour = G.C.RED, button = back_func, shadow = true}, nodes = {
                  {n = G.UIT.T, config = {text = "No", scale = 0.5, colour = G.C.UI.TEXT_LIGHT, shadow = true}}
                }}
              }
            }
          }},
        }},
      }
    }
  end

function G.MULTIPLAYER.COOPEXT.overlay_message(message)
	G.SETTINGS.paused = true
	local message_table = G.MULTIPLAYER.UTILS.string_split(message, "\n")
	local message_ui = {
		{
			n = G.UIT.R,
			config = {
				padding = 0.2,
				align = "cm",
			},
			nodes = {
				{
					n = G.UIT.T,
					config = {
						scale = 0.8,
						shadow = true,
						text = "MULTIPLAYER",
						colour = G.C.UI.TEXT_LIGHT,
					},
				},
			},
		},
	}

	for _, v in ipairs(message_table) do
		table.insert(message_ui, {
			n = G.UIT.R,
			config = {
				padding = 0.1,
				align = "cm",
			},
			nodes = {
				{
					n = G.UIT.T,
					config = {
						scale = 0.6,
						shadow = true,
						text = v,
						colour = G.C.UI.TEXT_LIGHT,
					},
				},
			},
		})
	end

	G.FUNCS.overlay_menu({
		definition = create_UIBox_generic_options_with_2_buttons({
			contents = {
				{
					n = G.UIT.C,
					config = {
						padding = 0.2,
						align = "cm",
					},
					nodes = message_ui,
				},
			},
		}),
	})
end


--///////////////////////////////////////////////////////////////////////////////////////
local handplayed = false
local discard_e = nil
local discard_hook = nil
local play_e = nil

local discard_cards_from_highlighted_ref = G.FUNCS.discard_cards_from_highlighted
---@diagnostic disable-next-line: duplicate-set-field
function G.FUNCS.discard_cards_from_highlighted(e, hook)
    sendWarnMessage(" ========= discard_cards_from_highlighted =========", "MULTIPLAYER_COOPEXT")
    
    if G.LOBBY.code and G.LOBBY.config.gamemode == "coop" then
        
        love.thread.getChannel("uiToNetwork"):push("action:discardCards\n")
    end

    return discard_cards_from_highlighted_ref(e, hook)
end

local play_cards_from_highlighted_ref = G.FUNCS.play_cards_from_highlighted
---@diagnostic disable-next-line: duplicate-set-field
function G.FUNCS.play_cards_from_highlighted(e)
    sendWarnMessage(" ========= play_cards_from_highlighted =========", "MULTIPLAYER_COOPEXT")
    
    if G.LOBBY.code and G.LOBBY.config.gamemode == "coop" and G.LOBBY.players then
        
        love.thread.getChannel("uiToNetwork"):push("action:handPlayed\n")
    end

    return play_cards_from_highlighted_ref(e)
end



G.MULTIPLAYER.COOPEXT.discard_cards_from_highlighted = function(e, hook)
    stop_use()
    G.CONTROLLER.interrupt.focus = true
    G.CONTROLLER:save_cardarea_focus('hand')

    for k, v in ipairs(G.playing_cards) do
        v.ability.forced_selection = nil
    end

    if G.CONTROLLER.focused.target and G.CONTROLLER.focused.target.area == G.hand then G.card_area_focus_reset = {area = G.hand, rank = G.CONTROLLER.focused.target.rank} end
    local highlighted_count = math.min(#G.hand.highlighted, G.discard.config.card_limit - #G.play.cards)
    sendWarnMessage("highlighted_count: "..#G.hand.highlighted.. " card_limit: ".. G.discard.config.card_limit.. " cards: " .. #G.play.cards, "MULTIPLAYER_COOPEXT")
    if highlighted_count > 0 then
        sendWarnMessage(" ========= highlighted_count > 0 =========", "MULTIPLAYER_COOPEXT")
        update_hand_text({immediate = true, nopulse = true, delay = 0}, {mult = 0, chips = 0, level = '', handname = ''})
        table.sort(G.hand.highlighted, function(a,b) return a.T.x < b.T.x end)
        inc_career_stat('c_cards_discarded', highlighted_count)
        SMODS.calculate_context({pre_discard = true, full_hand = G.hand.highlighted, hook = hook})
        
        -- TARGET: pre_discard
        local cards = {}
        local destroyed_cards = {}
        for i=1, highlighted_count do
            G.hand.highlighted[i]:calculate_seal({discard = true})
            local removed = false
            local effects = {}
            SMODS.calculate_context({discard = true, other_card =  G.hand.highlighted[i], full_hand = G.hand.highlighted}, effects)
            SMODS.trigger_effects(effects)
            for _, eval in pairs(effects) do
                if type(eval) == 'table' then
                    for key, eval2 in pairs(eval) do
                        if key == 'remove' or (type(eval2) == 'table' and eval2.remove) then removed = true end
                    end
                end
            end
            table.insert(cards, G.hand.highlighted[i])
            if removed then
                destroyed_cards[#destroyed_cards + 1] = G.hand.highlighted[i]
                if SMODS.shatters(G.hand.highlighted[i]) then
                    G.hand.highlighted[i]:shatter()
                else
                    G.hand.highlighted[i]:start_dissolve()
                end
            else 
                G.hand.highlighted[i].ability.discarded = true
                draw_card(G.hand, G.discard, i*100/highlighted_count, 'down', false, G.hand.highlighted[i])
            end
        end

        -- context.remove_playing_cards from discard
        if destroyed_cards[1] then
            SMODS.calculate_context({remove_playing_cards = true, removed = destroyed_cards})
        end
        
        -- TARGET: effects after cards destroyed in discard

        G.GAME.round_scores.cards_discarded.amt = G.GAME.round_scores.cards_discarded.amt + #cards
        check_for_unlock({type = 'discard_custom', cards = cards})
        if not hook then
            if G.GAME.modifiers.discard_cost then
                ease_dollars(-G.GAME.modifiers.discard_cost)
            end
            ease_discard(-1)
            G.GAME.current_round.discards_used = G.GAME.current_round.discards_used + 1
            G.STATE = G.STATES.DRAW_TO_HAND
            G.E_MANAGER:add_event(Event({
                trigger = 'immediate',
                func = function()
                    G.STATE_COMPLETE = false
                    return true
                end
            }))
        end
    end
end

G.MULTIPLAYER.COOPEXT.play_cards_from_highlighted = function(e)
    if G.play and G.play.cards[1] then return end
    --check the hand first

    stop_use()
    G.GAME.blind.triggered = false
    G.CONTROLLER.interrupt.focus = true
    G.CONTROLLER:save_cardarea_focus('hand')

    for k, v in ipairs(G.playing_cards) do
        v.ability.forced_selection = nil
    end
    
    table.sort(G.hand.highlighted, function(a,b) return a.T.x < b.T.x end)

    G.E_MANAGER:add_event(Event({
        trigger = 'immediate',
        func = function()
            G.STATE = G.STATES.HAND_PLAYED
            G.STATE_COMPLETE = true
            return true
        end
    }))
    inc_career_stat('c_cards_played', #G.hand.highlighted)
    inc_career_stat('c_hands_played', 1)
    ease_hands_played(-1)
    delay(0.4)

        for i=1, #G.hand.highlighted do
            if G.hand.highlighted[i]:is_face() then inc_career_stat('c_face_cards_played', 1) end
            G.hand.highlighted[i].base.times_played = G.hand.highlighted[i].base.times_played + 1
            G.hand.highlighted[i].ability.played_this_ante = true
            G.GAME.round_scores.cards_played.amt = G.GAME.round_scores.cards_played.amt + 1
            draw_card(G.hand, G.play, i*100/#G.hand.highlighted, 'up', nil, G.hand.highlighted[i])
        end

        check_for_unlock({type = 'run_card_replays'})

        if G.GAME.blind:press_play() then
            G.E_MANAGER:add_event(Event({
                trigger = 'immediate',
                func = (function()
                    SMODS.juice_up_blind()
                    G.E_MANAGER:add_event(Event({trigger = 'after', delay = 0.06*G.SETTINGS.GAMESPEED, blockable = false, blocking = false, func = function()
                        play_sound('tarot2', 0.76, 0.4);return true end}))
                    play_sound('tarot2', 1, 0.4)
                    return true
                end)
            }))
            delay(0.4)
        end

        G.E_MANAGER:add_event(Event({
            trigger = 'immediate',
            func = (function()
                check_for_unlock({type = 'hand_contents', cards = G.play.cards})

                G.E_MANAGER:add_event(Event({
                    trigger = 'immediate',
                    func = function()
                        G.FUNCS.evaluate_play()
                        return true
                    end
                }))

                G.E_MANAGER:add_event(Event({
                    trigger = 'after',
                    delay = 0.1,
                    func = function()
                        check_for_unlock({type = 'play_all_hearts'})
                        G.FUNCS.draw_from_play_to_discard()
                        G.GAME.hands_played = G.GAME.hands_played + 1
                        G.GAME.current_round.hands_played = G.GAME.current_round.hands_played + 1
                        return true
                    end
                }))
                G.E_MANAGER:add_event(Event({
                    trigger = 'immediate',
                    func = function()
                        G.STATE_COMPLETE = false
                        return true
                    end
                }))
                return true
            end)
        }))
end






