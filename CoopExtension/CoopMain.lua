-- CoopMain.lua
-- Cooperative Extension for Balatro Multiplayer Mod

-- /i\ Useful shits to know /i\
-- Updating ui is in the coopgame_ui.lua file
--      it override the game functions / hook 

-- Initialize the Coop Extension
CoopExtension = {}

local Disableable_Option_Cycle = G.MULTIPLAYER.COMPONENTS.Disableable_Option_Cycle


function CoopExtension:Initialize()
    sendTraceMessage("Initializing Coop Extension...", "MULTIPLAYER_COOP_EXTENSION")
end

-- Function to start cooperative mode
function CoopExtension:StartCoopMode()
    sendTraceMessage("Starting Cooperative Mode...", "MULTIPLAYER_COOP_EXTENSION")
    -- G.LOBBY.type = "coop"
    -- G:set_game_mode('coop')
end

-- UI return 
function CoopExtension:UiLobbyOptions()
    return 
    {
        -- label = G.localization.misc.dictionary["opts_gm"] or "Gamemode Modifiers",
        -- tab_definition_function = function()
        --     return {
        --         n = G.UIT.ROOT,
        --         config = {
        --             emboss = 0.05,
        --             minh = 6,
        --             r = 0.1,
        --             minw = 10,
        --             align = "tm",
        --             padding = 0.2,
        --             colour = G.C.BLACK,
        --         },
        --         nodes = {
        --             {
        --                 n = G.UIT.R,
        --                 config = {
        --                     padding = 0,
        --                     align = "cm",
        --                 },
        --                 nodes = {
        --                     Disableable_Option_Cycle({
        --                         id = "starting_lives_option",
        --                         enabled_ref_table = G.LOBBY,
        --                         enabled_ref_value = "is_host",
        --                         label = G.localization.misc.dictionary["opts_lives"] or "Lives",
        --                         options = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,},
        --                         current_option = G.LOBBY.config.starting_lives,
        --                         opt_callback = "change_starting_lives",
        --                     }),
        --                     G.LOBBY.type == "coop"
        --                             and Disableable_Option_Cycle({
        --                                 id = "showdown_starting_antes_option",
        --                                 enabled_ref_table = G.LOBBY,
        --                                 enabled_ref_value = "is_host",
        --                                 label = G.localization.misc.dictionary["opts_start_antes"]
        --                                     or "Starting Antes",
        --                                 options = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 },
        --                                 current_option = G.LOBBY.config.coop_starting_antes,
        --                                 opt_callback = "change_showdown_starting_antes",
        --                             })
        --                         or nil,
        --                 },
        --             },
        --         },
        --     }
        -- end,
    }
end

-- UI Main Menu return
function CoopExtension:UiMainMenu(var_495_0)
    return 
    {
        label = G.localization.misc.dictionary["coop_name"] or "Coop",
        chosen = true,
        tab_definition_function = function()
            return {
                n = G.UIT.ROOT,
                config = {
                    emboss = 0.08,
                    minh = 6,
                    r = 0.15,
                    minw = 10,
                    align = "tm",
                    padding = 0.3,
                    colour = G.C.DARK_BLUE,
                },
                nodes = {
                    {
                        n = G.UIT.R,
                        config = {
                            align = "tm",
                            padding = 0.07,
                            w = 8,
                            h = 2,
                        },
                        nodes = {
                            UIBox_button({
                                id = "start_coop",
                                label = {
                                    G.localization.misc.dictionary["start_lobby"]
                                        or "Start Lobby",
                                },
                                colour = G.C.GREEN,
                                button = "start_lobby",
                                minw = 5,
                                shadow = true,
                            }),
                        },
                    },
                    {
                        n = G.UIT.R,
                        config = {
                            align = "tm",
                            padding = 0.07,
                            minw = 8,
                            minh = 4,
                            colour = G.C.LIGHT_GREY,
                        },
                        nodes = {
                            {
                                n = G.UIT.T,
                                config = {
                                    text = G.MULTIPLAYER.UTILS.wrapText(
                                        G.localization.misc.dictionary["coop_desc"] or
                                            "Multiplayer Co-op mode allows you to team up with friends for an engaging experience!",
                                        50
                                    ),
                                    shadow = true,
                                    scale = var_495_0 * 0.65,
                                    colour = G.C.WHITE,
                                },
                            },
                        },
                    },
                },
            }
        end,
    }
end

CoopExtension:Initialize()

return CoopExtension
