# Some Bones

Trying to figure out meaning of joint flags
can't rule out the idea of bone numbering for hitboxes?

these binary values are from a bitset of the number,
after it has been converted to host endianness

Some Fox Bones:

    1d7d8: 00010000000000000000000000001001 left ear
    1d998: 00010000000000000000000000001001 right ear
    1d768: 00010000000001010000000010001001 head
    1d6f8: 00010000000000000000000000001001 neck

    1ced8: 00010000000000000000000000001001 lower torso
    1cf48: 00010000000000000000000000001001 upper torso

    1c668: 00010000000001010000000110001110 bone 0
    1c6d8: 00010000000000000000000000001001 bone 0 -> torso
    1c748: 00010000000000000000000000001001 ball joint at end of bone 0 -> torso connection
    1c7b8: 00010000000000000000000000001001 ecb bottom bone?
    1c828: 00010000000000000000000000001001 lowest torso?



# Skeleton?
I can't think of any common properties among these bones. It will likely be
worth creating a dummy obj exporter that creates a skeleton where each bone's 
color is determined by the flags for the joint.





# Last Bit flips some axis?
Characters mostly seem to have the last bit 0 on joints you would need to be 
rotated/flipped along some axis -> usually the result of recycling a
bone structure. (i.e. left & right arms have the same bones, but flipped)

Were these structures written manually in a hex editor, so applying preset
transforms like this would be faster to develop than recalculating a proper
matrix? Or maybe it's a quirk of the in-houyse editor they were using when two
substructures were set as mirrors of one another?


# Feet Angles
Also, character's feet default to a weird 30-ish degree angle, but it
varies from character to character. It's possible that this is because the
default poses for their models has this bend (to make the feet look more
natural from the side).
I can see this happening when rushing to make fixes later in development, if 
animation angles are wrt the default pose, since that would mean that changing
the base pose would alter all other animations accordingly.


