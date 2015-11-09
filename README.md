# Melee Subaction Unpacker
a CLI tool to try to unpack melee's subaction scripting language. 

## Requirements
 - building requires g++ w/ c++11 support, and make
 - documentation generation documentation requires 
    - python 2.7
    - Beautiful Soup 4 (installable through pip)
    - ansi2html (installable through pip)

## Getting the datfiles
I can't supply you with the character datfiles because of legal reasons,
so you need to track them down somehow and place them in the datfiles
folder. (The character datfiles are the Pl__.dat files

## Interpreting the output
output is given in a tree-like structure. The most important one
to understand is the Subaction output format. For an example, we
look at the unpacking of Shiek's forward air:

        45 (0x7ab8): PlySeak5K_Share_ACTION_AttackAirF_figatree
            offset: 0x4864
            |timer_async 08 00 00 05 
            | autocancel 4c 00 00 01 
            |        sfx 44 04 00 00 00 00 00 9e 00 00 7f 40 
            |gen_article ac 02 00 00 
            |     hitbox 2c 00 b0 0d 05 78 02 54 00 00 00 00 0c 99 00 13 00 00 01 0b 
            | timer_sync 04 00 00 03 
            |   endcolls 40 00 00 00 
            | timer_sync 04 00 00 03 
            | autocancel 4c 00 00 00 
            |timer_async 08 00 00 28 
            |       iasa 5c 00 00 00 
            |        end 00 00 00 00 
            endoff: 0x48a8

 - `45`: The hex number representing the index of this action in the action
    table.
 - `(0x7ab8)`: The hex offset (relative to the data section) of the action's
    header.
 - `PlySeak5K_Share_ACTION_AttackAirF_figatree`: the name of the action in the
    ftData's string table
 - `offset: 0x4864`: the offset in the data section of the script 
 - `|  <commandname>: xx .. .. ..`: The name of the command, followed by its
    identifier and the arguments passed to it. The first byte is the command
    id, and the remainder are the arguments passed to that command.
 - `endoff: 0x48a8`: the offset to the last instruction in this script (the 
    null terminator / end script command)

in this command, we can see
    - we wait until the 5th frame
    - after those frames elapse, we
        - disable autocanceling
        - play a sound effect
        - make a particle
        - put out a hitbox
   - we then wait for another 3 frames (frame 8), and end all collissions
   - after another 3 frames (frame 11), we enable autocancelling
   - we then wait until frame 0x28 (frame 40), and signal the 
     command as interuptable

## Following GOTOs and Subroutines
Execution is followed through GOTO statements and subroutines. For example,
Roy's FuraSleepStart (Stun Starting Action)

        ce (0x937c): PlyEmblem5K_Share_ACTION_FuraSleepStart_figatree
            offset: 0x63d0
            | bodyaura_1 b8 a4 00 00 ac 01 a0 00 
            | subroutine 14 00 00 00 00 00 38 10 
                offset: 0x3810
                |   texswap? a2 00 08 01 
                | timer_sync 04 00 00 02 
                |   texswap? a2 00 08 02 
                | timer_sync 04 00 00 04 
                |   texswap? a2 00 08 01 
                | timer_sync 04 00 00 01 
                |   texswap? a2 00 08 00 
                |     return 18 00 00 00 
                endoff: 0x382c
            |   texswap? a2 00 08 01 
            |    setloop 0c 00 00 04 
            |    graphic 28 f8 00 00 01 a0 00 00 00 00 01 2c 02 bc 00 00 00 00 00 00 
            | timer_sync 04 00 00 04 
            |   execloop 10 00 00 00 
            |        end 00 00 00 00 
            endoff: 0x6404

The body of the called command is printed with a 1 higher indentation level.
Because the command is called with a subroutine and not a goto statement, on
return we continue following the execution of the caller.


## [RECURSION NOT ALLOWED]? What?
In order to stop the tracer from going infinitely deep, when a subaction calls
itself, 

            offset: 0x41c8
            |    unknown d0 00 00 03 
            |timer_async 08 00 00 1c 
            |    unknown d8 02 00 00 00 00 01 73 00 00 5a 40 08 00 00 2e 
            |    unknown d8 00 00 00 00 00 01 73 00 00 5a 40 20 00 00 00 
            |       goto 1c 00 00 00 00 00 41 c8 
            [RECURSION NOT ALLOWED 0x41c8]

## Assumed command functionality
question marks are put after command names that are just educated guesses of
functionality. Rationale for most of these assumptions are proided in
[` src/event_mapper.h`](./src/event_mapper.h).
