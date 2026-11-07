#!/usr/bin/env python3
"""The scripted inputs of the host build's test runs (docs/host.md, "Test scripts"), one script for every build.

    eval "$(python3 tools/host_play_script.py town)"   # sets ACWW_KEYS and ACWW_TOUCH
    python3 tools/host_play_script.py --list

Scripts (headless runs with the oracle's firmware image and a fresh save file unless said otherwise):
  play      the oracle's play script: the touch on the title, A every second from frame 2400.
  town      a new game through the intro (the name entries: a touch on the keyboard's Confirm button every
            600 frames and Start every second confirm a name), the town hall's tutorial, out into town.
  save      town, then the walk to the player's house, into the house and out, Tom Nook's talk about the loan,
            Start -> "Save and end": the first save (written to ACWW_SAVE) and back to the title.
  continue  with the save file from `save`: the title's continue path, the player wakes up in the house and
            stands in front of it in town.
  menu      with the save file from `save`: continue, out of the house, Y: the pause menu on the pockets (frame 4400;
            R / L at later frames go through its tabs, 200 frames apart is enough).
  gate-host, gate-visit
            local wireless between two copies (tools/host_wireless_test.sh), each with the save file from `save` and
            ACWW_TEST_JOB_DONE=1 (the gate opens only after Nook's part-time job): continue, walk to the gate house,
            in, talk to Copper. gate-host opens the gate ("Invite guests!", DS to DS) and walks off; gate-visit goes
            out (DS to DS), picks the town found, travels, arrives at the host's gate, goes back in and asks Copper to
            go home. Real time from frame 9600 on (ACWW_FAST=9600): the copies wait for each other.
Frame numbers are virtual frames (ACWW_KEYS / ACWW_TOUCH: frame:frames:value); keys are KEYINPUT bits (hex).
"""
import sys

A, B, START, RIGHT, LEFT, UP, DOWN, R, L, X, Y = 0x1, 0x2, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800


def every(start, end, step, frames, value):
    return [(f, frames, value) for f in range(start, end, step)]


def play():
    keys = every(2400, 30000, 60, 6, A)
    touch = [(2200, 10, '128,96')]
    return keys, touch


TOWN_END = 21050  # the last A / Start of the intro and the tutorial


def town():
    keys = every(2400, TOWN_END, 60, 6, A) + every(7650, TOWN_END, 60, 6, START)
    keys += every(TOWN_END + 50, TOWN_END + 650, 60, 6, B)   # (closes Pelly's last talk)
    keys += [(TOWN_END + 750, 400, DOWN)]                     # out of the town hall
    touch = [(2200, 10, '128,96')] + every(7630, TOWN_END, 600, 6, '218,180')
    return keys, touch


def save():
    keys, touch = town()
    # from in front of the town hall (113, 56): south past the house's west side, east along its front to its door at
    # (119, 109) (ACWW_POSLOG positions)
    keys += [(22400, 545, DOWN), (23000, 65, RIGHT), (23120, 150, UP)]
    keys += every(24000, 24241, 60, 6, A)      # opens the door
    keys += [(24900, 150, DOWN)]               # back out
    keys += every(25300, 31841, 60, 6, A)      # Tom Nook's talk
    keys += [(32300, 6, START)]
    keys += every(32500, 33501, 200, 6, A)     # "Save and end"
    return keys, touch


def cont():
    keys = every(2400, 30000, 60, 6, A)
    touch = [(2200, 10, '128,96')]
    return keys, touch


def menu():
    # with the save file from `save`: continue (the player is out of the house at about frame 4100), Y opens the pause
    # menu on the pockets at 4400 (shown from about 4450); X would open it on the map
    keys = every(2400, 3500, 60, 6, A) + [(4400, 6, Y)]
    return keys, [(2200, 10, '128,96')]


def moves(frame, seq):
    """keys from a list of moves from `frame` on: ('A', n) / ('D', n): press A or Down (6 frames), then wait n frames;
    (direction, n): hold it n frames, then 40 frames; ('W', n): wait n frames"""
    keys = []
    for k, n in seq:
        if k == 'W':
            frame += n
        elif k in ('A', 'D'):
            keys.append((frame, 6, A if k == 'A' else DOWN))
            frame += 6 + n
        else:
            keys.append((frame, n, {'U': UP, 'L': LEFT, 'R': RIGHT, 'S': DOWN}[k]))
            frame += n + 40
    return keys


# from in front of the player's house (119, 110), once the player is in town: west, north past the town hall's west
# side, around the trees to the north cliff, west to the gate house at (80, 33), in, up to the inner doors (16, 17) and
# right to Copper
TO_COPPER = [('W', 400), ('L', 80), ('U', 480), ('L', 170), ('U', 220), ('L', 140), ('U', 200), ('S', 50), ('L', 120),
             ('U', 200), ('W', 300), ('U', 60), ('W', 50), ('U', 60), ('W', 50), ('U', 60), ('W', 50), ('U', 60),
             ('S', 10), ('R', 40), ('W', 30)]


def gate_host():
    keys = every(2400, 3500, 60, 6, A)
    # Copper's talk to his menu (13 A), "Invite guests!", then up to the gate opening (22 A: one more starts a new
    # talk, and a host in a talk keeps the visitor waiting), then away from Copper
    keys += moves(4000, TO_COPPER + [('A', 84)] * 13 + [('D', 84)] + [('A', 84)] * 22 + [('W', 200), ('S', 300)])
    return keys, [(2200, 10, '128,96')]


def gate_visit():
    keys = every(2400, 3500, 60, 6, A)
    # "I wanna go out!" (DS to DS), the search, the town found, the trip (54 A in all); at the host's gate: back
    # into its gate house, up to the doors, right to Copper, and his talk about going home
    keys += moves(4000, TO_COPPER + [('A', 84)] * 54 + [('W', 300), ('U', 100), ('W', 300), ('U', 115), ('W', 60),
                                                         ('R', 40), ('W', 30)] + [('A', 84)] * 30)
    return keys, [(2200, 10, '128,96')]


SCRIPTS = {'play': play, 'town': town, 'save': save, 'continue': cont, 'menu': menu, 'gate-host': gate_host,
           'gate-visit': gate_visit}


def fmt(entries, hexval):
    out = []
    for f, n, v in sorted(entries, key=lambda e: e[0]):
        out.append(f'{f}:{n}:{v:x}' if hexval else f'{f}:{n}:{v}')
    return ';'.join(out)


def main():
    if len(sys.argv) != 2 or sys.argv[1] in ('-h', '--help'):
        print(__doc__)
        return 0
    if sys.argv[1] == '--list':
        print(' '.join(SCRIPTS))
        return 0
    keys, touch = SCRIPTS[sys.argv[1]]()
    print(f"export ACWW_KEYS='{fmt(keys, True)}'")
    print(f"export ACWW_TOUCH='{fmt(touch, False)}'")
    return 0


if __name__ == '__main__':
    sys.exit(main())
