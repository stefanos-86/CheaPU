# CheaPU - almost an emulator
"Programming ... via the front panel could be a tedious and time-consuming process...", says the [Wikipedia](https://en.wikipedia.org/wiki/Altair_8800).
Now you too can live the tedium using this basic old microcomputer simulator (which does *NOT* emulate the Altair 8800, nor any other real architecture).

## The Computer(R) user handbook
This software emulates an extremely basic computer. The kind of thing that people who took up electronics as an hobby in the '70s would construct out of buttons, lamps, integrated circuits and unbelivable patience. I am way younger than that, but I believe the passion and skills of those pioneers were something extraordinary.

All the input and output of this futuristic machine passes trough its front panel. 
![Front panel buttons](https://github.com/stefanos-86/CheaPU/blob/master/docs/ButtonsGuide.png "")

The reset button starts the computation. The program should start on the 1st address (0x00). There is no single-step button (you will regret it when you try to debug anything).
The LEDs match the accumulator content. It's in binary. It's not exactly "high definition video".

There are only a few instructions, you can see the opcodes [in the silicon itself](https://github.com/stefanos-86/CheaPU/blob/master/CheaPU_simulation/CPU.h#L32).
I hope you remember the hex->binary conversion rules.

Since toggling the front panel buttons _is_ tedious (the Wikipedia editor was under-selling it, in my opinion), you can use the other input facility of The Computer.
You can punch your code on the (simulated) paper tape on the right. It's not really that much better, but at least you can see what you are doing. Black holes are 0s, white squares are 1s. Use the LOADTAPE button once you are done.

Here is an example: count from 0 to 255, overflow, restart from zero, repeat forever. Use the HALT button when you are tired of watching the [blinkenlights](http://www.catb.org/~esr/jargon/html/B/blinkenlights.html).

![Programming example](https://github.com/stefanos-86/CheaPU/blob/master/docs/SimpleCount.png "")

A more difficult exercise - what does this code do?
![More difficult programming example](https://github.com/stefanos-86/CheaPU/blob/master/docs/Quiz.png "")

Now you may rethink your position on the "CISC vs. RISC" debate.

## About the real program
The most important part is the [CPU simulation](https://github.com/stefanos-86/CheaPU/blob/master/CheaPU_simulation/CPU.cpp).


It is very basic. The entry point is the cycle() method. That is the single step of the processor, just one machine cycle.
Instructions that take longer than that remain suspended (thanks to some [questionable coroutines](https://github.com/stefanos-86/CheaPU/blob/master/CheaPU_simulation/StepByStep.h)).

Everything else is plain and simple code. Loops, ifs and arrays. There are no other strange programming tricks (well, the [text rendering](https://github.com/stefanos-86/CheaPU/blob/master/CheaPU_UI/UserInterface.h#L98), maybe...).

I wanted to to a (simple) emulator for a long time. Well, I have gone and made it.

Finally, the pun in the name requires to say "CPU" as an Italian would. It almost sounds like "Cheap e-u".


## Where to go from here?
* If you want to see real emulators or know more about microcomputers of the '70s and '80s, just google "retrocomputing" and follow up from there; there is a huge and very active community and endless documentation.
* If you want to be serious about low-level programming on real computers, again there are plenty of resources on the web. Two of favourites are [Programming From The Ground Up](https://www.onlineprogrammingbooks.com/programming-from-the-ground-up/) the [X86 Disassembly](https://en.wikibooks.org/wiki/X86_Disassembly) wikibook.
* If you are curious about how real CPUs are put together: back at Univeristy I enjoyed the "Structured Computer Organization" book by Tanenbaum (Italian edition); I read more books later on... I'll update the references here soon.

## Answer to the programming quiz
It sums the numbers from 4 to 1 (4 + 3 + 2 + 1 = 10). The running sum is at 0x15 (the last tape byte), there is a "1" hardcoded at 0x13 (don't forget to count from 0!) to do the +1 and -1 at every loop and the counter is at 0x14. 
