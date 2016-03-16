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


# Figatree Formatting
Pl\*AJ.dat files are made form a bunch of concatenated HAL data files. Each HAL
file contains a single file, with the same name as the animation that contains
a figatree object.

Figatrees start with 2 values, unknown use.
{
    int     unknownA = 1
    int     unknownB = 0
    float   numFramesPlusOne = ?
}

numFramesPlusOne is the number of frames in the animation, plus one.
This is followed by a block of data. This is followed by a run of memory up
until the string table, 0x250 long in bowser's ThrowAirLo from the front of
the fig to the front of the file.

This can't hold the animation data, it's not long enough, so it's probably
an array of some number of pointers

It also likely is not organized in frame-major order (i.e. pointers to info
on each frame), since [(1570 - 1320) / 4 / 6] comes out to 24.6, which is
less than the number of frames in bowser's down throw (29)

Bowser's skeleton has 75 joints in it too.
(difference) / 4 / 2 comes out to 74?
4: size of pointer
2: number of parameters on a joint (scale, rotation)

could also be offset by the ecb and 



