<div align=center>
   <h1>$$\text{\LARGE Pipedream}$$</h1>
   <i><p>$$\text{Weird Hallucinating Loader}$$</p></i>
</div>

## $$\text{About:}$$

Nothing special. Just a fully recursive portable executable (PE) loader written in C. 

The idea for this project popped up in my head while solving random easy Codewars 
problems with recursion to practice for my exam, the exact thought was:
>  I wonder how nice resolving imports & performing relocations would look if I did it using recursion...

And here we are.

## $$\text{Features:}$$
- No CRT (?)
- Maybe API hashing (tiger??)
- Anti-Debugging using smelly_vx's `feverdream` technique (code runs only once machine is locked)
- Thread hidden from debugger
- And more! ( I am too lazy to think of more and acutally implement it then )

<!-- and also **MAYBE** (idk if it would be bad) use pipes and have 2 processes map one executable in memory-->
## $$\text{Tasks Left:}$$

- [ ] Test out and rewrite `ResolveImport` & `PerformRelocs`
  - [ ] Create `relocs_ctx` and `imports_ctx` structures to not pass billion parameters manually
- [ ] Actually finish the Project
- [ ] Test against Elastic????

## $$\text{Credits:}$$

Full credits go to [@smelly_vx](https://github.com/smellyvx) for his awesome feverdream technique described [here](https://vx-api.gitbook.io/vx-api/my-projects/fever-dream-code-executing-when-the-windows-machine-is-locked)
