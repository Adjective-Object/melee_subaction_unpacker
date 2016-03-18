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



## Flag Theories

Will be referencing flag #s like this:



    00000000000000000000000010000000
                                   ^ this is flag 0

    00000000000000000000000010000000
                            ^ this is flag 7

    00000000000000000000000010000000
    ^ this is flag 31


 - flag 8 is set high for roy's sword, sword hilt, scabbard, and both
    shoulderpads. Maybe this indicates that it is attached to a separate
    model?
 - 




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
    int32   unknownA = 1, usually
    int32   unknownB = 0, usually
    float   numFramesPlusOne = ?
    int32   probablyAnOffset
}

numFramesPlusOne is the number of frames in the animation, plus one.
This is followed by a block of data. This is immediately followed by the offset
table of the HAL file. This means that the last line is probably an offset
within the current file.

However, in a lot of the files, the offsets reach past the internal hal data
file for the move. I'm not sure what to do with this.

 - If we rule out the idea of stepping into random memory, it could be an
   offset from the root of the parent file (the memory map origin).
 - Otherwise, it could be an offset from any of:
     - The figatree header
     - The internal datfile's header
     - The internal datfile's data section
     - The PL\*.dat file

Worth investigating is the offset table of the datfile? There's a number of hal
files, but I think the offsets in their offset tables reach outside the range
of the internal files. Does that mean that each offset table is wrt the root of
the mmapped file? Is it replicated for each mmapped file?


## Pl\*\*AJ.dat :: Theory on internal hal files

My best guess is that these data
files were made manually, and were intended to be laid out like this:


    ┌──────────────┐
    │ Hal Header   │
    ├──────────────┤

     Space Intended
     for animation
     data

    ├──────────────┤
    │ Offset Table │
    ├──────────────┤
    │ Root List    │
    ├──────────────┤
    │ String Table │
    └──────────────┘


However, since the hal headers are referenced from specific addresses, they
can't be moved around without changing the offset in the Pl\*\*.dat file.
This means that if an animation is elongated past the original size allocated
for it, it has to be moved elsewhere in the file.

maybe during development the animation data of datfiles were shuffled around 
because it would be easier than repeatedly updating the offsets of every 
animation in player scripting file, so we end up with the data sections
containing animation data for different things.


# Offsets?

In the case of bowser's ThrowAirw\_figatree animation, the offset is
4816 (0x12d0), which if wrt the root of the mmapped file, is 27 words until
the next previously visited structure.




