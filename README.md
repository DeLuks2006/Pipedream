<div align=center>
   <h1>$$\text{\LARGE Pipedream}$$</h1>
   <i><p>$$\text{Weird Hallucinating Loader}$$</p></i>
</div>

## $$\text{About:}$$

Nothing special. Just a fully recursive portable executable (PE) loader written in C++. 

The idea for this project popped up in my head while solving random easy Codewars 
problems with recursion to practice for my exam, the exact thought was:
>  I wonder how nice resolving imports & performing relocations would look if I did it using recursion...

And here we are.

## $$\text{Features:}$$
- No CRT (?) <- in progress
- Maybe API hashing (??) <- later
<!-- - Anti-Debugging using smelly_vx's `feverdream` technique (code runs only once machine is locked)-->
<!-- - Thread hidden from debugger-->
- And more! ( I am too lazy to think of more and acutally implement it then )

<!-- and also **MAYBE** (idk if it would be bad) use pipes and have 2 processes map one executable in memory-->
## $$\text{Tasks Left:}$$

- [ ] Add NT-definitions
- [ ] Resolve Functions using PEB
- [ ] Implement Hash algorithm
- [ ] Implement API-Hashing
- [ ] Test it out!!

Later...
- [ ] Test against Elastic????

## $$\text{Credits:}$$

- Full Credits for the [feverdream technique](https://vx-api.gitbook.io/vx-api/my-projects/fever-dream-code-executing-when-the-windows-machine-is-locked) go to [@smelly_vx](https://github.com/smellyvx) [here].
- The ASM code was stolen from [TitanLdr](https://github.com/kyleavery/TitanLdr)
- The Linker script was stolen from the [Startdust Template](https://github.com/Cracked5pider/Stardust) (will likely be modified later)
