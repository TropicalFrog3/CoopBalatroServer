SMODS.Atlas {
    key = 'my_object_atlas', -- A unique key for your atlas
    px = 32,                -- Width of your image in pixels (single resolution)
    py = 32,                -- Height of your image in pixels (single resolution)
    path = 'modicon.png',   -- The filename of your image (e.g., "MyObject.png")
    --atlas_table = 'ASSET_ATLAS' --  Defaults to ASSET_ATLAS, good for non-animated sprites
}

SMODS.Sprite {
    key = 'p1_mouse_cursor',
    atlas = 'my_p1_mouse_atlas',
    pos = {x = 0, y = 0}
}

SMODS.Sprite {
    key = 'p2_mouse_cursor',
    atlas = 'my_p2_mouse_atlas',
    pos = {x = 0, y = 0}
}

