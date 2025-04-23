local SendCard = true

local click_ref = Card.click
---@diagnostic disable-next-line: duplicate-set-field
function Card:click()
    sendWarnMessage(" ========= card_click =========", "MULTIPLAYER_COOP_EXTENSION")
    click_ref(self)

    if SendCard and G.LOBBY.code and G.LOBBY.config.gamemode == "coop" then
        local message = string.format("action:cardClick,playing:%s", self.playing_card)
        sendWarnMessage( message, "MULTIPLAYER_COOP_EXTENSION")
        love.thread.getChannel("uiToNetwork"):push(message)
    end
end

function G.MULTIPLAYER.COOPEXT.Card_Click(playing_card)
    sendWarnMessage(" ========= Clicking_Card: ".. playing_card.." =========", "MULTIPLAYER_COOP_EXTENSION")
    for _, card in ipairs(G.hand.cards) do
        local cp = string.format(card.playing_card)
        if cp == string.format(playing_card) then
            sendWarnMessage(" ========= card.playing_card == playing_card  =========", "MULTIPLAYER_COOP_EXTENSION")
            SendCard = false
            card:click()
            SendCard = true
        else
            sendWarnMessage(" ========= not found: ".. cp .." != ".. playing_card .. "=========", "MULTIPLAYER_COOP_EXTENSION")
        end
    end    
end
