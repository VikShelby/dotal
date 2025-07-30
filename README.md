# DOTAL <img src="./favicon.ico" width="40" height="40" alt="DOTAL Logo">

Welcome to **DOTAL**, a dynamic, object-oriented programming language with a clean, Albanian-based syntax. DOTAL is built from the ground up in C, featuring a high-performance bytecode virtual machine, a single-pass compiler, and a mark-and-sweep garbage collector.

This project was created from scratch by **Emanuel Dervishi** ([@Vikshelby](https://github.com/VikShelby)).

It is a fully-featured, Turing-complete language designed to be powerful, expressive, and fun to use.

---

## Key Features

*   **🇦🇱 Albanian-based Syntax:** Write code in a natural, intuitive way with keywords like `shpall`, `funksion`, `nese`, `perndryshe`, `tip`, and `kthe`.
*   **Multi-paradigm:** Seamlessly blend procedural and object-oriented programming styles.
*   **Rich Data Types:**
    *   **Numbers:** `10`, `3.14`
    *   **Booleans:** `vertet`, `gabuar`
    *   **Strings:** `"Përshendetje, Botë!"`
*   **Powerful Data Structures:**
    *   **Lists (Arrays):** `shpall lista = [1, "dy", tre];`
*   **Full Control Flow:** `nese`/`perndryshe` (if/else), `derisa` (while), and `per` (for) loops.
*   **Complete OOP System:**
    *   **Classes:** `tip Kafe { ... }`
    *   **Instances:** `shpall porosia = Kafe();`
    *   **Fields:** `porosia.lloji = "espres";`
    *   **Methods:** `porosia.servir();`
    *   **Inheritance:** `tip Makiatos < Kafe { ... }`
*   **High-Performance Backend:** DOTAL code is compiled to a compact bytecode representation and executed on a fast, stack-based Virtual Machine.
*   **Automatic Memory Management:** A built-in Mark and Sweep Garbage Collector handles memory for you.
*   **Extensible Standard Library:** Includes native functions written in C, such as `koha()` (time) and `lexo()` (read input).

---

## Quick Example: The Masterpiece (`kryeveper.al`)

Here's a taste of what DOTAL code looks like. This example showcases functions, loops, lists, and classes.

```albanian
# Gjuha DOTAL - Prova Finale

funksion fib(n) {
  # Llogarit numrin e n-te ne serinë e Fibonaccit
  nese (n < 2) {
    kthe n;
  }
  kthe fib(n - 1) + fib(n - 2);
}

shpall lista_e_numrave =;

per (shpall i = 0; i < lista_e_numrave.gjatesia(); i = i + 1) {
  shpall numri = lista_e_numrave[i];
  printo "Fibonacci per " + numri + " eshte: " + fib(numri);
}

tip Pije {
  pershkruaj() {
    printo "Une jam nje pije e thjeshte.";
  }
}

tip Kafe < Pije {
  servir() {
    this.pershkruaj(); # Therrasim metoden tjeter duke perdorur 'this'
    printo "Ju u sherbye!";
  }
}

shpall porosia_ime = Kafe();
porosia_ime.servir();
```
> [See more examples here!](https://github.com/VikShelby/dotal/tree/main/examples)

---

## Getting Started

### Building from Source

DOTAL is written in standard C99 and can be compiled with GCC or Clang. No external dependencies are required.

```bash
# Clone the repository
git clone https://github.com/VikShelby/dotal.git
cd dotal-lang

# Compile the source code
gcc -Wall -O2 -o dotal src/*.c

# You now have the DOTAL executable!
```

### Running a Script

To execute a DOTAL script (files should end in `.al`), simply pass the file path to the executable:

```bash
./dotal examples/kryeveper.al
```

### Windows Installer

A pre-compiled installer for Windows is available in the [Releases](https://github.com/VikShelby/dotal-lang/releases) section. The installer will automatically add `dotal` to your PATH and associate `.al` files with a custom icon.

---

## The Journey

This language was built from the ground up as a journey through the world of compilers and virtual machines, inspired by the incredible book ["Crafting Interpreters"](https://craftinginterpreters.com/) by Bob Nystrom. Every component, from the scanner to the garbage collector, was implemented from scratch.

## License

DOTAL is open-source software licensed under the **MIT License**. See the `LICENSE` file for details.

Copyright (c) 2025 Emanuel Dervishi