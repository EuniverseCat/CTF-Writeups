# TheZZAZZGlitch's April Fools Events
My first real foray into programming was back in 2018, for ZZAZZ's 2018 April Fools event. These events are games which are built into Pokemon save files, and they each have a few hacking challenges built into them. I managed to get 2 of the 4 challenges solved in the 2018 event, the second with some horribly ugly code that was really the first real code I ever wrote. I didn't really start programming in earnest until Feb 2019, and I've been actively improving ever since. While I didn't participate much in the 2021 event at the time, I ended up going through all the April Fools events in the past shortly afterward, and managed to complete every challenge in them. It's never been so clear to me how far I've come as a programmer -- back when I actually did the 2018/19 challenges, I wouldn't have had the slightest clue on where to start for the harder ones. I look forward to participating in future events!

## 2017 - missingno.sav

This year's save was structured quite differently than future ones, and is based off of glitch knowledge, rather than just being a game to play through. Cool concept, but unfortunately inaccessible to all but a tiny group of people.

### 31337
Difficulty: 4/10  
This achievement is unobtainable through normal means. You know what to do!

Here we go! At the end of the game, a password is generated. This password includes an encoded version of your in-game time and the in-game achievements you've accomplished. You enter this password on a website to receive a score. I found the password code by putting a read breakpoint on the in-game time. The password code I analyzed did something like this:

- Check if you've gotten the pokedex flag achievement
- Do some stuff that I didn't bother analyzing with the in-game time
- For each achievement, it checks if a specific byte is equal to a specific value.
  - If it is, it sets a bit of a variable.

I figure if I set that variable to 0xFF, I'll get the final achievement. I try that and…. my password doesn't change. Hmm. A quick look at the code shows the instruction `and a,3F` -- ignoring the top 2 bits of the variable even if it's set. I edited the code to `and a,FF`, entered the resulting password on the site, and it worked!  
![](/ZZAZZ/2017/31337.png)

Main difficulty here was just finding the password code -- the way it checks achievements is rather indirect, so my initial attempts to find the code based on the other achievements were all unsuccessful. The only ways I found to quickly find the code were through the in-game time and pokedex flags, both of which directly accessed as part of the password algorithm. The overall code was quite easy, I think I solved this about 5 minutes after finding the code.


## 2018 - Glitchland
Probably my favorite save to date - there's a client application which allows you to connect the game to a server via the game boy link cable. I'd recommend playing it on your own: https://fools2018.stranck.ovh/ (requires a ROM of Pokemon Yellow)

### Cracker Cavern 1 - I know how to edit memory
Difficulty: 1/10  
Use whatever tools you desire to go through the rock barrier below. Note: basic protections against cheating are implemented.  
![](/ZZAZZ/2018/Images/CRACKER_CAVERN_I.webp)

Easiest hacking challenge in one of these by far - I used the same approach as I did for the first room in the 2017 challenge and just edited the map.
https://www.youtube.com/watch?v=C6BOSZ10Bp4  
(The anti-cheating protections just prevent walk-through-walls cheats)


### Cracker Cavern 2 - I got better at editing memory
Difficulty: 6/10 (at least with my approach)  
As you may have noticed, this entire world is created of many different small maps. Use whatever tools you desire to visit a map with the hex identifier 0x1337.

I initially tried to start by putting a breakpoint on the fade-to-black code, which plays when you enter a new room. After around 30 minutes of messing around with this, I realized I wasn't getting anywhere. I kept landing in a bunch of code which was part of the communication protocol, and the way the save loaded maps was separate from the way the base game did, so I couldn't really use that as a lead.

So I decided to reverse the communication protocol instead! Surely not the easiest approach, but it gave me a head start on CC4, so may as well kill two birds with one stone.

I started with the client application first - it's written in Python, so I could mess with the code without much effort. I rigged it to just print out any data it was sending to or receiving from the game.

<img src="/ZZAZZ/2018/Images/CC2-server.png" height="350"/>

*A typical exchange with the server when it loads a room*

<img src="/ZZAZZ/2018/Images/CC2-packets.png" height="400"/>

*I could also use the functionalities in the emulator I'm using to watch the data transfers, so the Python client mostly just served to confirm what I was seeing*

I decided to focus on the first of the two outward data transfers. I was able to put a write breakpoint on the byte used for data transfer itself, and could find the block of data that was being sent pretty easily from that. Some analysis of this data later, I was able to put together a picture of what data was being sent when a room is loaded:

- length (2 bytes, little endian, includes these 2 bytes themselves)
- ???\? (2 bytes, I suspect that these are a checksum)
- ?? (1 byte, hardcoded to 0x03 for this first data transfer and 0x04 for the second)
- Inventory (41 bytes, copied from the player's inventory) 
- ???\? (2 bytes, unknown)

Those last 2 bytes look pretty suspicious to me. It's getting them from somewhere in pokemon box data, where this save stores a lot of code and variables. I try editing that data to 0x1337, and....  
<img src="/ZZAZZ/2018/Images/CC2-wrongmap.png" height="250"/>  
Hm. I'm clearly on the right path. Maybe it's little endian?  
<img src="/ZZAZZ/2018/Images/CC2-mysterious.png" height="250"/>  
I enter 0x3713 instead, and it loads a map named "Mysterious" and congratulates me for solving the challenge!  


### Cracker Cavern 4 - I reversed the communication protocol
Difficulty: 6/10  
Make the final step forward and reverse-engineer the transmission protocol used to send data to the game server. To clear the final challenge, please send a packet with the command ID 0x77. The message body should contain three bytes: 0x13, 0x37, 0xCC.

This only took about 5 minutes to solve after finishing CC2, with all the work I'd already done towards it. I expect the data I need to send to be as follows:
- Length: 0x08, 0x00
- Checksum: 2 bytes, I'll let the game calculate this for me.
- Command (so that's what this is!) 0x77
- Content: 0x13, 0x37, 0xCC

The code I've been working with does something along these lines:
- Set length to 0x30, 0x00
- Set command to 0x03
- Copy data from the player's inventory.

I edit the code to set the length and command to the values I want, and edit my inventory so that the first three bytes are 0x13, 0x37, and 0xCC. I let the code run… and the game crashes! Wonderful! But looking at my stats on the game server, I actually did get the achievement before the crash. 


### Cracker Cavern 3 - I know how to solve crackmes
Difficulty: 8/10  
I have a nice image for you. Unfortunately, it is encrypted. Figure out the key that makes the image look as intended. You can view the image and test various keys by interacting with the sign next to this one. The key is all numeric.

For this challenge, along with PK3 and PK4, I used the same method to find the code. By placing a read breakpoint on variables relating to which buttons are currently pressed, I was able to land in the input handling code right before the code I was looking for, and could find the code without too much trouble.

The decryption code takes a 10-byte key and does the following:
```python
for i in range(25):
    #What exactly ScrambleKey() does is not relevant to my solution
    ScrambleKey()
#This goes through each byte of the encrypted photo
#and xors each byte with a byte of the scrambled key
XorPhoto()
```
I replicated the [code](/ZZAZZ/2018/Code/CC3.cs) in C# without too much difficulty. Now I needed some way of measuring entropy in the image, to figure out whether a generated image was in fact the correct image. I ended up just checking how many bytes were equal to 0 or 0xFF, which I expected to be common in the decrypted image… and immediately got some weird results. I was getting a lot of results that had 10 or more bytes equal to 0 or 0xFF, which would be statistically impossible if you assumed all incorrect keys would basically output random data.

After a walk and some discussion with my dad, I had the idea that each correct byte of the key would correctly decrypt several bytes of the image. See, in the key scrambling portion of the decryption algorithm, different bytes of the key can affect each other, resulting in a highly unpredictable result. However, while the key is still modified during the part where it XORs the photo, it's much more predictable there. If the 0th byte of the photo is XORed with 0x01, then the 10th will always be XORed with 0xF1, the 20th with 0xA9, the 30th with 0x77, and so on. If a single byte is decrypted correctly, then every 10th byte afterward will also be decrypted correctly.

Using this knowledge, I was able to calculate what the key should look like after the key scrambling process. (commented code in AnalyzePhoto() and AnalyzeLists()). I tried entering that key, and…  
<img src="/ZZAZZ/2018/Images/CC3-solution.png" height="250"/>  
Yay! Doesn't solve the problem (I only know what the key looks like after the scrambling process, not before) but it sure makes it a lot easier to check if an answer is right, and skips running a quarter of the code while I'm at it.

I tried reversing the key scrambling algorithm -- it might be possible but I'm unsure, and I didn't have any luck with it, so I just let my computer bruteforce it, which took about 2 hours, and read a book in the meantime.

#### Post-completion improvements
The key scrambling algorithm is reversible, although it's a little tricky. It looks intimidating at first glance - some XORs, a rotate, and replacing bytes with ones from a lookup table. However, all of that is more of a distraction. The scrambling can be reduced to this:
```python
previousByte = key[len(key) - 1]
for i in range(len(key)):
    key[i] = Transform(key[i], previousByte)
    previousByte = key[i]
```
Where Transform() is the XORs/rotate/lookup table/etc, all of which can be easily inverted. The difficult part is the presence of the previous byte from the key in the scrambling process. However, I was able to invert this by realizing that the previous bytes show up in the key after one round of key scrambling -- I could just go through the key backwards, using the previous scrambled byte as the second Transform() parameter. This reduces to this:
```python
i = len(key) - 1
while (i >= 0):
    if (i != 0):
        previousByte = key[i - 1]
    else:
        previousByte = key[len(key) - 1]
    key[i] = InverseTransform(key[i], previousByte)
    i--
```
which is easily solvable.


## 2019 - Glitch Islands
My second favorite save(s). Also would recommend playing these, although fair warning that the final boss is probably a little too hard. https://fools2019.stranck.ovh/ (Requires a Pokemon Crystal ROM.) Each area of the game has its own specific save, and you can only save after finishing an area. You upload your saves to the website to progress and get achievements.

### Pwnage Kingdom 1 - Blue Sailors of Death
Difficulty: 2/10  
To your right, you can see a pair of friendly, nice trainers. Just kidding, they will crash the game if you try to fight them! Find a way to bypass both trainers and read the sign on the other side.  
<img src="/ZZAZZ/2019/Images/PK1-Start.png" height="250"/>

After a little combing of [the disassembly](https://github.com/pret/pokecrystal/tree/master/engine/overworld), I found a section of memory related to map objects. Changing the data of the 1st and 2nd objects (the trainers) allow you to bypass them, by say, changing their coordinates, or more comically, just making them face away from you.  
<img src="/ZZAZZ/2019/Images/PK1-Solved.png" height="250"/>

Note: my first solution attempt was to interrupt the code that checked if a trainer would see me. It worked…. but not quite well enough. https://www.youtube.com/watch?v=gmCY7XhedG4


### Pwnage Kingdom 2 - Unreferenced
Difficulty: 3/10 with a good guess, 6/10 without  
This save file contains exactly two maps. You're currently standing in one of them, but the other one is inaccessible. Or is it? Perhaps there's a way to access it? That's your job. Enter the lost, hidden second map to proceed.

There are two variables - wMapGroup and wMapNumber - where the current map is stored. (I kinda just poked around in map loading code until I found them.) Change these to, say, 1 and 1, and you too can spawn on the counter in the Olivine City Pokecenter!  
<img src="/ZZAZZ/2019/Images/PK2-Olivine.png" height="250"/>  
Now if you had thought of it, you could try setting the map ID to 1 higher than the map where you spawn in. Yeah, that's the answer. You spawn in map 9163, the map you want to load is 9164. Needless to say, I did not think of this.

By looking at what reads wMapGroup/wMapNumber, I was able to find a 9 byte map header stored in Pokemon box data. Wasn't able to make much sense of it, but I figured it must have been copied there at some point. I put a write breakpoint on that, and… wait what?  
![](/ZZAZZ/2019/Images/PK2-Copy.png)  
It's copying 100 bytes (0x64) in. Now this could just be more map data, but I'd expect all the headers to be next to each other in the save (that's where it's copying from). The way the game finds the headers is effectively (some pointer found based on wMapGroup) + (9 * wMapNumber). This means that increasing the map number to somewhere between 64 and 73 would mean the game would still be reading from this data it copied in. Worth a shot I guess? I try 64, and  
<img src="/ZZAZZ/2019/Images/PK2-Solved.png" height="250"/>  
oh. Didn't really expect that to actually work. Nice. If it hadn't, my next plan of action would have been to check where the game kept headers for other maps, which I could check in other save files.

(Note: I solved this in 2019 on pure accident. I found wMapGroup and wMapNumber and nothing beyond that. I had set the two variables to different values, just messing around, and meant to set them back to their initial values. I must have typoed and entered the map number instead. I didn't actually realize how I'd "solved" it until much later.)


### Pwnage Kingdom 3 - Encryptic
Difficulty: 10/10  
This save file contains exactly two maps. You're currently standing in one of them. The other one is encrypted. Thankfully, you won't have to break the encryption. The algorithm and the key are all available to you! You can just walk up to the rock in front of the entrance, and decryption will begin. The problem is, the algorithm might take some time. Just a tiny bit. By tiny bit, I mean a couple thousand years. Or maybe there's a way to speed up the decryption? That's for you to find out! Decrypt the map and visit it to proceed.

<img src="/ZZAZZ/2019/Images/PK3-Start.png" height="250"/>

I've never worked with optimizing something like this, so I'm already a little worried before starting. Found the code through input handling code again -- [here's the asm](/ZZAZZ/2019/Code/pk3.asm) with a few added labels.

A lot of the code is just doing 32-bit operations on a gameboy (multiplication via repeated addition, ew), so porting it to my computer brings the time down to… 100 years. I mean it's not an insignificant improvement, but…

(Note: I messed up the endianness of all the 32-bit variables in the code at first. It just makes the explanation more confusing, so I'm going to act like I didn't.)

Let's look at what this code does. It's basically two random number generators inside of one another. For each of the roughly 2 billion (!) loops of the outer RNG, it generates a random int, which is used to seed the inner RNG. For each of the 450 million loops of the inner RNG, it XORs a byte of the map with the lower byte of a randomly generated short. 2 billion * 450 million comes out to approximately *punches buttons wildly on calculator* way too many iterations, so I'm gonna have to find a way to cut some out.

The first flaw I noticed was that only 3 of the 4 bytes of the outer RNG are used to seed the inner RNG. With 2^24 possible inner RNG seeds over 2^31 iterations, there would have to be repeats. Since XORing is its own inverse, if the inner RNG was seeded the same way twice, the two times would cancel each other out. This means that instead of running the entire 2 billion loops, I could store the number of times each seed was generated, and only run the seeds that were generated an odd number of times. It turns out that only 1270 seeds were being generated an odd number of times -- few enough to bring the runtime down to an hour and a half.

(I didn't realize this at the time because of the aforementioned endianness errors -- I thought I still had to go through around a million seeds, so I kept looking for improvements)

The next flaw I noticed was in the inner RNG. Both the inner and outer RNGs are linear congruential generators. ([LCGs](https://en.wikipedia.org/wiki/Linear_congruential_generator)) LCGs generate random numbers based on the formula ![md should really support latex](https://latex.codecogs.com/svg.latex?%5Cinline%20x%3D%28ax&plus;b%29%5Cmod%20m), where a and b are constants. (For the outer 32 bit LCG, m is 2^32, and for the inner 16 bit LCG, m is 2^16). Eventually, a number will repeat. The maximum period between repeats would be equal to m.

With the inner LCG repeating over 2^28 times, even the maximum length period of 2^16 would loop a huge number of times. My idea here is that successive loops of the same period would cancel each other out. You can see this quite easily with a period length of 512. (The encrypted map is 512 bytes long.) The first 512 iterations of the LCG XORs each byte of the map with a random number, but the next 512 iterations completely undo that.

However, with a period length of 513, this becomes much more complicated. After 513 iterations, while the random numbers start repeating, they are being XORed with different bytes of the encrypted map. In fact, it will take ![like seriously, this looks terrible](https://latex.codecogs.com/gif.latex?%5Cinline%20%5Cfrac%7B2LP%7D%7B%5Cgcd%28L%2CP%29%7D) iterations for a period to fully cancel itself out, where L is the map length and P the period length.

Even for the worst possible period length, eliminating these redundant iterations is a 4-fold improvement, and since the vast majority of period lengths are either short or powers of 2, this brought down the total required time to under 3 minutes on my computer.

(At this point I realized my endianness mistakes, and fixing those along with an off-by-one that took about 40 minutes to track down let me solve this challenge!)  
<img src="/ZZAZZ/2019/Images/PK3-Solved.png" height="250"/>

#### Post-completion improvements
It turns out there's one more pretty substantial improvement that can be made to the outer loop, which I did after completing the challenge. The outer loop has a period of length 2^30. This explains why only 1270 seeds had to be run -- the outer loop iterates 2^31 - 1270 times, so all but the last 1270 cancelled each other out. Knowing this, I can avoid having to keep track of how many times each seed was run, and just run the last 1270 seeds. This brought the runtime down to 47 seconds on my computer.

But wait! We can do better! At this point, this is just turning into a challenge to see how fast I can make this. I'm only running the last 1270 seeds… which would be equivalent to running 1270 seeds from the start, but inverting the RNG algorithm. This is possible with the use of a modular multiplicative inverse, which is some math that definitely went a bit over my head, but brought the runtime down to 33 seconds.

At this point I'm pretty sure over half of the runtime is just checking for whether seeds have been generated yet in a list. I change that to an array instead, and… ok it was more than half. Runtime is down to 350 ms. I think I'm done here. [Here's my final code.](/ZZAZZ/2019/Code/PK3.cs)


### Pwnage Kingdom 4 - Master of Saving
Difficulty: 9.5/10  
Reverse-engineer the game saving system. To prove your understanding of the save mechanics, you need to create a very special save file for me. Each completed save file contains a blob encoding variables related to your game progression. The server then decodes it and updates your progress. To pass the challenge, every byte in the DECODED blob has to be equal to its offset in the data, mod 256. So, byte on offset $3F should have the value $3F. And on $1C3, it should be $C3. There are two exceptions. First, any checksum bytes are exempt from this rule. Second, the four decoded bytes at offset $1A5 should have special values: (these vary for each user). This is to make sure it's impossible to upload files created by other users. Submit this special file to Pwnage Kingdom IV to finish the challenge.

Okay… this sounds intimidating, but let's get an idea what I'm looking at. I try to save the game, and "The aura of Missingno.'s corruption prevents you from saving" pops up. Funny. I'll just analyze the save in a location where I can actually save the game; I don't see any reason to try to bypass this.

I again find the save code by tracing execution from input handling code. The start of the save routine looks like this:  
![](/ZZAZZ/2019/Images/PK4-ROP.png)  
So it prints some text, does some graphics stuff (Request1bpp and AEDB both do graphical stuff), and then oh what the *fuck*.
```
ld sp,AE18
ret
```
It's 10 pm, I'm too tired for this shit.

The next day:  
So I know about [ROP](https://en.wikipedia.org/wiki/Return-oriented_programming) in theory, but actually encountering it is a totally different thing. The concept is, by hijacking the stack pointer, you can write a program using existing code by returning to just before a return instruction, executing a few instructions, and repeating. With control of the call stack, you effectively have total control of what code is executed. It's just really hard to wrap your head around, since it executes code in a completely non-linear order. 

But it gets worse. I start looking at what the ROP does, and it's not pretty. There's a jumptable located right after this code (More precisely, a table of stack pointer values.) What it does (or at least, what it *would* do, if this was written in a sane way) is call a function in that jumptable, based on the value at the address in the de register. ([my transcribed assembly is here](/ZZAZZ/2019/Code/pk4.asm) - I'm talking about ropFuncAE18.) We've basically got a glorified bytecode interpreter, written in ROP.

After a few solid hours of transcribing the actual code being run and analyzing it, I put together 13 separate ROP functions. Some of these (01, 02, 07, 0B) are pretty tame, just copy data from one place to another. 03, 0D and 0E seem to have indirects, which is messy, 08 and 09 involve (a single instruction) of self rewriting code, which is highly cursed but manageable, and 04 seems to be a sort of "return" byte, which I didn't really analyze. The last three are the interesting ones. 05 is *really* long and incredibly hard to follow, but appears to encrypt… something. Not really sure. (A look at the source code after I solved this confirmed that it was intentionally obfuscated.) 06 rotates a 0x1B0 byte portion of memory, which I'm guessing is the save data. And 0C is a loop instruction. (The loop variable is stored in the bytecode itself, which is really ugly. Also 00 and 0A don't correspond to functions, not sure why.)

At this point, I figured I had two options - recreate the bytecode interpreter, or make a bytecode decompiler. I ended up going with the former -- nothing much came of it and I had to write the decompiler anyway, but if you're interested, [here's the code for both.](/ZZAZZ/2019/Code/PK4.cs)

[Here's what my decompiler spat out](/ZZAZZ/2019/Code/DecompiledBytecode.py), with some edits made for clarity. I was able to map out what bytes corresponded to what in the save data, and it came out to the correct length of 0x1B0, which was a good sign:
```
00-8F party pokemon
90 checksum
91-A4 progress flags
A5 checksum
A6-A8 money
A9-140 items
141 checksum
142-162 party nicknames
163 checksum
164-1A3 pokedex
1A4 checksum
1A5-1A8 user-specific data
1A9 checksum
1AA-1AB RNG + checksum
1AC-1AD timer registers + checksum
1AE-1AF overall checksum
```
I copied all the data into the relevant places (00010203 into pokemon, 919293 into progress flags, etc), made a savestate, saved the game, and uploaded the file.  
![](/ZZAZZ/2019/Images/PK4-Expired.png)  
Oh well. Didn't really expect to get it first try. I'll just feed [loud bastard child](https://cdn.discordapp.com/attachments/693835817751281664/834634269296164868/IMG_20210421_203714684.jpg) and try again.

WAIT. It didn't say my save was corrupted or anything. It said it was in the wrong location. I enter Pwnage Kingdom 4 on the site, upload the same file and it works!  
![](/ZZAZZ/2019/Images/PK4-Solved.png)  
I now feed [loud bastard child](https://cdn.discordapp.com/attachments/658175508856569857/834459119632515072/IMG_20210421_085328287.jpg) in celebration, who really wants dinner and could not care less about the achievement I just accomplished.


## 2021 - Road to Infinity
Cool conceptually, but not the most interesting to actually play through. You play in a procedurally generated map, and have to explore through differently themed areas to progress.

### Hacking Challenge 1 - Far Lands
Difficulty: 2/10 (0/10 with a walk through walls cheat and some time)  
There are no infinite worlds, computers are discrete machines. So maybe this world has an edge too? Walk 524272 steps in any cardinal directions to find out.

Some people actually solved this by using a walk-through-walls cheat and using something to weigh down their directional and fast forward keys. Can't say I endorse that method.

First off, 524272 is roughly 2^19, which is a weird number. Don't really know what to make of that yet. 

I started with the variables wYCoord and wXCoord. They're locked between 0x10 and 0x1F for some reason, if you move outside of a 16x16 area, they loop. I put a breakpoint on the Y coordinate and pretty quickly found some code that was causing the loop. I didn't bother analyzing the code -- I just walked north, and every time the code was run, one of the values in a register decreased by one. I figure this value has something to do with what map the player is on - if each map is 16x16, then the challenge becomes "Walk 2^15 - 1 maps in any direction," which would make perfect sense if the variables that keep track of which map you're on are signed shorts. I try setting that register equal to 0x7FFF, and I end up here:  
<img src="/ZZAZZ/2021/Images/HC1-fail.png" height="250"/>

Well it worked. I'm just in a tree. I tried this in another location, where I didn't warp into a tree… and immediately got a game over because the wild encounter was so strong. I tried again, this time using savestates to avoid encounters, and  
<img src="/ZZAZZ/2021/Images/HC1-solved.png" height="250"/>  
Congratulations, you reached the end of the world. Kind of. The world will loop back around. Glitchtopia isn't flat. That said, here's a reward for coming this far! (challenge 1 password)

(Alternative, slightly funnier solution: I figure it's checking whether the map coordinate is equal to 0x7FFF, and I found the code that checks whether to give you the achievement by just searching for `cp a,7F`. Modifying this code lets you "reach" the edge of the world without even moving.)


### Hacking Challenge 2 - YEDONG'S TAIL
Difficulty: 3/10  
One of the achievements is programmed to be obtainable through normal means. But if you were to understand the game's password system, maybe you could create and submit a password that unlocks that special reward…

This is essentially a nerfed version of the 2017 challenge. The password is just in straight hex, so it was trivial to search for it in RAM and put a write breakpoint on it, which landed me at roughly the start of the password generation routine. The password routine sets up an unencrypted password before encrypting it -- the first byte is the play time in minutes, second is a bunch of flags which I assume are the achievements in question, 3rd is.... some variable, I'm not sure, 4th is random. Change the second byte to 0xFF, and I got the achievement.

(Note: looking at the source, the 3rd byte is the number of bosses defeated. I did this from an otherwise 100% complete savestate, so I didn't have to worry about this.)


### Hacking Challenge 3 - Worry Seed
Difficulty: 7/10 in terms of actual difficulty, but so much of this challenge is essentially busywork so effectively higher.  
I was playing this event, and I found a very interesting seed! The problem is… I don't remember it. But I have a screenshot of the location I spawned in at. Maybe you can use it to recover the seed I used? I also happen to remember that the last digit of the seed was "1".  
<img src="/ZZAZZ/2021/Images/HC3-goal.png" height="250"/>

I was excited about this one at first. I've done [work to search for fast seeds in randomizers before](https://www.youtube.com/watch?v=PuPxjKeVMvY), so this seems right up my alley.

Oh you sweet summer child. You babe, swaddled in the cashmere blankets of ignorance. This challenge *sucked*. It's cool in theory… and then you realize you're dealing with about 800 lines of label-less assembly. (I found the code by just searching for the seed in memory.) That does not sound like my idea of fun, so I just looked at the [source](https://github.com/zzazzdzz/fools2021/blob/main/fools2021-src/engine/map_gen.asm), which definitely saved me a few hours. (The source was only published after the event ended.) At least doing this saved me a bunch of time on the 4th challenge.

I considered some alternate approaches to avoid having to go through all that code -- running a brute force script in an emulator would probably be doable, but I'd have to figure out how to even implement that. I messed with the idea of autogenerating some really ugly C from the assembly and actually kinda got that working, but I figured I'd have to analyze the code to figure out where it outputted data, so I didn't finish it. Luna in the GCRI discord found out how to analyze the save in ghidra, which I tried, but even after fixing some stuff, most of the output was messy to the point of not being usable. (I did use the ghidra output for a few simpler functions.)

After a solid few days of working on this off and on, I finally got some C code ([here](/ZZAZZ/2021/Code/foolsHC3.c)) ([but look at HC4 code instead](/ZZAZZ/2021/Code/foolsHC4.c)) that could replicate a map generated in the game. Nothing too interesting about most of it. I set that up to bruteforce the seed, and… got 780 results. Ah.

<img src="/ZZAZZ/2021/Images/HC3-falsepositive.png" height="250"/>

*One of the false positives*

It's important to understand how the game determines where to place you. After the starting map is generated, it goes through each tile of the map, and places you on the first normal ground tile it finds. In the goal image, since there's a normal ground tile on the row above the spawn point, I know I'm spawning in on the top row of the map.  
<img src="/ZZAZZ/2021/Images/HC3-chunkboundary.png" height="250"/>

*We could technically be on the rightmost column instead, but I'm politely ignoring that possibility for now.*

In the interest of not having to work in C anymore, I made the assumption that only two maps were being shown at the spawn point - the starting map and the one above it. (This was a pretty big assumption -- notably any spawn in the the first 2 blocks of a map would show 4 maps - but it ended up being correct.) At the time, I was only checking for matches in the bottom map, hence all the false positives. I added a check in for the top map, ran my code, and after a few minutes, it got a match! It was 1 am. My sleep schedule is still a bit messed up as I write this.


### Hacking Challenge 4 - Real ACE Hours
Difficulty: Yes  
Gen I Pokemon games are notoriously glitchy. And if you're build a new game on top of an already broken engine, things might break. Spectacularly. The final task is to find and exploit an arbitrary code execution vulnerability in the save file itself.  
To solve this task, you'll need to make a TAS which records from power on, and use any actions you need to take control of the instruction pointer.  
Your payload should jump to address $B882. It contains a small routine that just shows some text on the screen: "Congratulations. You got ACE!". The text does not have to appear on screen - as long as the routine runs and doesn't crash in the process, you're good to go.

Well.

Ok then.

Arbitrary code exploits in games are among the most interesting exploits out there, in my opinion. Early games often have extremely little controllable data to work with, making running even simple code quite complex. Here are a couple of my favorites -- would highly recommend checking these out if this kinda stuff interests you.

Super Mario World: https://www.youtube.com/watch?v=vAHXK2wut_I  
Zelda 1: https://www.youtube.com/watch?v=fj9u00PMkYU  
Ocarina of Time: https://www.youtube.com/watch?v=wdRJWDKb5Bo

I also would recommend looking at other people's writeups - to my knowledge, every single person who solved this challenge did so differently.  
Stranck: https://stranck.ovh/writeups/data/fools2021/#Fourth-hacking-challenge  
Buurazu: https://pastebin.com/UH8bJBz8  
jfb1337: https://github.com/joefarebrother/Fools2021  
Couldn't find anything from Drenn or NieDzejkob.

I've never really touched any arbitrary code stuff before, and I was much more interested in that than looking for an entry point, so I used Stranck's writeup for a clue to a starting place. He wrote that he "knew from the GCRI discord that there was a bug in the inventory: if you have exactly 40 items and you try to open the item menu the game crashes." The inventory actually would have been my first place to look at anyway -- dunno if I would have found my entry point without that hint though.

I elected not to poke around the source for this one, as I was worried there would be hints to the entry point in the source, and I did want to give finding the entry point an honest try myself. I started by memory editing myself a bunch of skip sandwiches -- but before I could really try anything with them, I opened my menu, and they were all overwritten with the pattern 00 FF. I poked around in the code that did that, and ended up at a bit of a loss. It seemed like it was doing some form of menu setup, but I couldn't really think of why it would.

After a little bit of messing around with some other menu stuff and sitting on it a bit, it hit me. The [99 stack glitch](https://glitchcity.wiki/99_item_stack_glitch) is kinda an unloved child in Pokemon glitching. It's pretty much useless in the actual game, but it does allow a small degree of controllable memory corruption. After collecting 99 of an item, the game looks for another stack of items to place another item in. However, it's possible that it will miss a terminator byte (0xFF) at the end of the inventory, and corrupt a part of memory after the inventory. The 00 FF pattern effectively makes it impossible to exploit this glitch if you ever open your menu. (Unless you collect around 3000 items, which a couple people actually did.)

I started looking into what memory could be corrupted with this glitch, and I found three potentially interesting uses - Reviver Seeds and Bravery Potions could corrupt password generation code (which I recognized from the second challenge!) and Life Seeds could corrupt some item handling code. Awfully convenient that that code happened to follow the inventory. :) However, these came with some pretty significant limitations -- I could only increase one byte of code with each item, and I couldn't increase anything higher than 99, or it would look for another stack of "items" to increment.

With these limitations, I had to toss the Life Seeds idea out - it just didn't have access to any interesting opcodes. The other two, however, could potentially modify the stack pointer. The most useful thing seemed to be collecting Reviver Seeds to change a value from 0x11 to 0x31 -- corrupting the opcode `ld de,F9D4` to `ld sp,F9D4`. (Note: using any 2-byte opcode here would have made the following opcode `ld sp,hl`, placing the stack pointer in more controllable memory. This was the intended solution, and I believe I was the only person who didn't use it.)

(Note: addresses E000-FDFF are equivalent to addresses C000-DDFF on the gameboy -- reading data from F9D4 is the same as reading from D9D4.)

D9D4 is in map data, and is therefore partially controllable. I started analyzing the map code further, to see what bytes could appear in the map (aka where I could return to), and only corruption biomes seemed to have much potential.

<img src="/ZZAZZ/2021/Images/HC4-corruption.png" height="250"/>

*Corruption biomes have these glitchy looking tiles which have higher IDs than other tiles - these are interesting, as they could cause a return into RAM.*

The most interesting places I could return to were DBxx (item data) and C9xx/CAxx/CBxx/D9xx (map data, again). Item data didn't seem promising -- there are only 18 kinds of items in the game, and I can't have more than 99 of an item -- so I started looking at what code could run in map data. My initial idea was that I could return to D9xx and get the code to survive a couple hundred bytes of map data without crashing, after which it would reach the in-game timer, which I could control. Unfortunately, corruption biomes are quite prone to crashing -- some of the aforementioned glitchy tiles are returns or non-existent opcodes, so that seemed like a bust. However, corruption biomes had some potentially interesting opcodes, notably 0x31, which could load a value to the stack pointer. (This isn't the first time this opcode has come up and it won't be the last.)

I went through every stack pointer that I could set it to and after testing a few potentially interesting results, I came up with… exactly one thing which seemed useful. Setting the stack pointer to 0AEC would return to F0D1 (aka D0D1). This was mostly just a bunch of zeros, with an occasional arithmetic instruction, but the code reached a step counter and a timer -- two bytes which I could completely control. I did some analysis of register values, and figured that if I could load a good value into the L register, I could then `jp (hl)` to right before the global RNG seed, which you could set at the start of the game to any value. That should be enough for arbitrary code, right?

(Note on gameboy CPU registers: The H and L registers are 8 bits, and when combined, form a 16-bit HL register. In this case, H will almost always be 0xDA. The RNG seed is located at DAB8, so I want to set L to B8 or thereabouts, so that `jp (hl)` will end up at the seed.)

Well, kind of. I managed to jump to the goal function at B882 by memory hacking the right values into the right places, but there were still some *major* issues with my setup.

Issue 1: The stack pointer. Part of this setup involved setting the stack pointer to 0AEC, which is a ROM address. If I wanted the game to not crash after jumping to the goal function, I'd have to put the stack pointer back in RAM. With 4 bytes in the seed, I can form the instructions `ld sp,hl` and `jp B882` which works... assuming the H register is unmodified. The opcode at DAB1 - 7 bytes before the seed - modifies the H register. This isn't looking great. I came up with two ways to potentially combat this, although they both have issues of their own:  
1 - Restrict the coordinates: The current map's x-coord is located at DAB4, and the y-coord at DAB6. This means they would be run as code. The most useful value would be 0x31 (sound familiar?) as the first byte of the y-coord, which would set the stack pointer to a location dependent on the second byte of the y-coord and the first byte of the seed.  
2 - Restrict the jump to the seed: If the jump to the seed went straight to DAB8, it would bypass the issue entirely. Unfortunately, this exact thing is

Issue 2: The jump to the seed. While the H register should nearly always be 0xDA after the code exits map data, few other registers are as predictable. Getting any register (or the value at the address in the HL register) between 8B and B8 is enough for the jump to the seed, but if I wanted to jump straight to the seed, this would become a major issue. I do have one trick up my sleeve for adjusting register values - the code run at D0D1 is some move data of the last pokemon that appeared. This actually includes the pokemon which appear on the title screen, and if Pidgeotto or Rhydon appeared last on the title screen, their data could give me a small amount of control over registers.

Issue 3: The RNG. I need several sequences of bytes to appear at certain places in certain maps. That's already unlikely enough, and any restrictions on the map's coordinates or the seed just make maps harder to find. I'd also prefer that my final TAS didn't take 5 hours, so I'd also like the x and y coordinates to be pretty close to the spawn.

In summary, my theoretical exploit was as follows:
```
Possibly wait on title screen for the right pokemon moves at D0D1
Set seed to (varies depending on setup) (any jump or call opcode) 82 B8
Pick up 131 (99 + 0x20) reviver seeds 
    This corrupts a byte in password code to 0x31 (ld sp, value)
Navigate to the correct map. Map requirements:
    Map has the byte 0xD9 at D9D5.
    At D900 + (D9D4), map data forms the opcode ld sp,0AEC 
        followed by a return opcode.
    Possibly location restrictions, depending on the setup.
Walk until the step counter is 0xE9 (opcode jp (hl))
Wait until the ignore input counter will load a useful value into L
Retire (generate a password)
```
With a theoretical exploit found, I set out to find a working map. I'd already ported most of the map generation code to C, [so it wasn't too bad to port the remaining biomes.](/ZZAZZ/2021/Code/foolsHC4.c) After some fairly disappointing results, I finally settled on a working seed with the y-coordinate restriction -- seed E8C382B8 would have a working map at (0x52, 0xF931). That's 1743 maps north of the spawn location. So much for having my TAS being a reasonable length, but at least my exploit would work in practice. My route is now as follows:
 
    Wait for Rhydon on title screen    
    Set seed to E8C382B8
    Never get an encounter or open the menu, and collect 131 reviver seeds.
    Navigate to (x, y) = (0x52, 0xF932)
    Walk until step counter is 0xE9 (jp hl)
    Wait until ignore input counter is 0x6E (ld l, (hl))
    Wait until wPasswordRandom is 0x8B-0xB6
    Win

And the code that runs during ACE is:

    ld sp,F9D4     (corrupted pw code)
    ret            (pw code)
    @F9D4: D90F    (manipulated map data)
    ld sp,0AEC     (manipulated map data)
    ret            (manipulated map data)
    @0AEC: F0D1    (some vanilla code, don't know, don't care)
    ldd (hl),a     (rhydon's moves, hl = wPasswordRandom)
    ld l,(hl)      (ignore input counter, 8B >= (hl) >= B6)
    jp hl          (step counter)
    ld hl,0002     (regrettably) (text data)
    ld d,d         (x coord)
    nop            (x coord)
    ld sp,E8F9     (y coord + seed byte 1)
    jp B882        (seed)

Note the presence of a new variable, wPasswordRandom -- this was actually a pretty big oversight on my part. If the code gets through map data without the L register being modified (much more likely than not), decrementing L will cause HL to point to a RNG dependent (read: manipulable) byte in the password. (Decrementing HL is a side effect of the ldd (hl),a instruction.) This was actually part of the intended solution to this challenge -- interesting that I still used it indirectly.

I modified my map generation code to generate the entire portion of the map I'd be traversing, then ran a [breadth first search on the map in C#.](/ZZAZZ/2021/Code/HC4_PathGenerator.cs) I constructed a [file composed of UDLR characters](/ZZAZZ/2021/Files/path.txt) to represent the path I would take, and wrote [a Lua script that could convert that to TAS inputs.](/ZZAZZ/2021/Code/HC4Auto.lua) This took a little while -- I had almost no prior experience with Lua, and I also had to write code that could find/pick up reviver seeds along the way, as well as some Lua code to manipulate RNG. Nothing particularly worth documenting though, and my script only desynced once. (Items use hashing to determine whether you've already picked them up, and there must been a hash collision.)

I ended up with a 4 hour, 57 minute TAS. What an experience.

<img src="/ZZAZZ/2021/Images/HC4-solved.png" height="100"/>

https://www.youtube.com/watch?v=nlY6l7P-SJE (please don't actually watch this)

It was really cool how the previous challenges all mattered for this final one -- my solution to the first let me test things faster, the second involved code which was involved in the ACE exploit, and the third ended up being the bulk of the code needed to find a working map for my exploit. This was definitely among the hardest programming-related things I've done, but it ended up being really fun to work on. (Except for the Lua. I will gladly never work with Lua again.)

There were a few oversights I made with my solution which could have made this easier. First, the intended use of the 99 stack glitch was to make the opcode `ld sp,hl` appear, which gave much more control over the stack pointer than map data did. I'm kinda glad I missed this tbh - it forced me to come up with what I think is a more interesting way of obtaining ACE. Second, I kind of fixated on 0xD9 appearing at the stack pointer, in order to return to map data. However, the values 0xC9, 0xCA, and 0xCB would have also returned to map data, and I completely forgot about them. Third, I didn't realize the potential of using Rhydon's moves to decrement the HL register, causing HL to point to wPasswordRandom. While I did end up using this, I could have used this to jump directly to the seed, bypassing any need for a y-coordinate restriction. Overall, the second and third oversights combine for a 1024-fold improvement on the number of maps available, which would have saved hours on the TAS. Still, I'm incredibly proud of the work I put into solving this.

(Also, [here's my notes for the last challenge.](/ZZAZZ/2021/Files/HC4%20notes.txt))

Hopefully I'll be able to participate in the event as it's going on next year! Looking forward to it.
